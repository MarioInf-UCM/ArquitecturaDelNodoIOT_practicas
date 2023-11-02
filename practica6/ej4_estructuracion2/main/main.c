#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_pm.h"
#include "esp_sleep.h"
#include "driver/uart.h"

#include "mock_wifi.h"
#include "temperatureMonitor.h"
#include "mock-flash.h"
#include "monitor-gpio.h"
#include "consola.h"
#include "esp_event_base.h"

static const char *TAG = "MAIN";
#define TIME_TO_DEEP_SLEEP 1 * 60 * 1000000         // Espera de 1 minuto
//#define TIME_TO_DEEP_SLEEP 12 * 3600 * 1000000    // Espera de 12 horas


// Event loops
esp_event_loop_handle_t loop_wifi_mock;
esp_event_loop_handle_t loop_monitor_gpio;
esp_event_loop_handle_t eventLoop_temperatureMonitor;
esp_event_loop_handle_t loop_console;

static void temperatureReaded_handler(void *registerArgs, esp_event_base_t baseEvent, int32_t idEvent, void *eventArgs);
static void task_mock_wifi_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
static void task_monitor_gpio_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
static void task_console_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
static void deepSleep_event_handler();


// Send data when connected
bool send_data = 0;

// Modes & current state
enum state
{
    MONITOR,
    CONSOLE
};
enum state current_state = MONITOR;
esp_timer_handle_t deepSleep_timer;

void app_main()
{
  
    ESP_LOGI(TAG, "Comenzando el proceso de inicialización.");
    esp_err_t result;

    // Configuración del Power Manager
    esp_pm_config_t pmConfig = {
        .max_freq_mhz = 240,
        .min_freq_mhz = 10,
        .light_sleep_enable = true
    };
    result = esp_pm_configure(&pmConfig);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR (%s)..: No se pudo configurar adecuadamente el Power Manager.", esp_err_to_name(result));
        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "El Power Manager ha sido configurado adecuadamente.");


    //Configuración del evento de entrada al modo Deep Sleep
    const esp_timer_create_args_t deepSleep_timer_args = {
        .callback = &deepSleep_event_handler,
        .name = "temperature"};

    result = esp_timer_create(&deepSleep_timer_args, &deepSleep_timer);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR (%s)..: No se pudo crear el timer para la entrada en el Deep Sleep.", esp_err_to_name(result));
        vTaskDelete(NULL);
    }
    
    result = esp_timer_start_periodic(deepSleep_timer, TIME_TO_DEEP_SLEEP);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR (%s)..: No se pudo activar el timer para acceso al Deep Sleep.", esp_err_to_name(result));
        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "El timer de acceso al modo Deep Sleep ha sido configurado adecuadamente.");

    //*************************************************************
    // Inicialización del comoponentes Mock WIFI - INICIO
    loop_wifi_mock = wifi_mock_init();

    result = esp_event_handler_register_with(loop_wifi_mock, WIFI_MOCK, WIFI_MOCK_EVENT_WIFI_CONNECTED, task_mock_wifi_handler, &loop_wifi_mock);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo registrar la manejadora del evento WIFI_MOCK - WIFI_MOCK_EVENT_WIFI_CONNECTED.");
        vTaskDelete(NULL);
    }

    result = esp_event_handler_register_with(loop_wifi_mock, WIFI_MOCK, WIFI_MOCK_EVENT_WIFI_GOT_IP, task_mock_wifi_handler, &loop_wifi_mock);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo registrar la manejadora del evento WIFI_MOCK - WIFI_MOCK_EVENT_WIFI_GOT_IP.");
        vTaskDelete(NULL);
    }
    result = esp_event_handler_register_with(loop_wifi_mock, WIFI_MOCK, WIFI_MOCK_EVENT_WIFI_DISCONNECTED, task_mock_wifi_handler, &loop_wifi_mock);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo registrar la manejadora del evento WIFI_MOCK - WIFI_MOCK_EVENT_WIFI_DISCONNECTED.");
        vTaskDelete(NULL);
    }

    wifi_connect();
    // Inicialización del comoponentes Mock WIFI - FIN
    //*************************************************************


    //*************************************************************
    // Inicialización del comoponentes Read Temperature - INICIO
    eventLoop_temperatureMonitor = TemperatureMonitor_init();

    result = esp_event_handler_register_with(eventLoop_temperatureMonitor, TEMPERATURE_MONITOR_EVENTS, TEMPERATURE_READED_EVENT, temperatureReaded_handler, eventLoop_temperatureMonitor);
    if (result != ESP_OK)
    {
        ESP_LOGE(TAG, "ERROR..: No se pudo registrar la manejadora del evento TEMPERATURE_MONITOR_EVENTS - TEMPERATURE_READED_EVENT.");
        vTaskDelete(NULL);
    }

    result = esp_event_handler_register_with(eventLoop_temperatureMonitor, TEMPERATURE_MONITOR_EVENTS, HUMIDITY_READED_EVENT, temperatureReaded_handler, eventLoop_temperatureMonitor);
    if (result != ESP_OK)
    {
        ESP_LOGE(TAG, "ERROR..: No se pudo registrar la manejadora del evento TEMPERATURE_MONITOR_EVENTS - HUMIDITY_READED_EVENT.");
        vTaskDelete(NULL);
    }
    // Inicialización del comoponentes Read Temperature - FIN
    //*************************************************************


    //*************************************************************
    // Inicialización del comoponentes Mock Flas - INICIO
     result = circularBuffer_init(100);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo inicializar el buffer de memoria flash");
        vTaskDelete(NULL);
    }
    // Inicialización del comoponentes Mock Flas - FIN
    //*************************************************************


    //*************************************************************
    // Inicialización del comoponentes Monitor GPIO - INICIO
    loop_monitor_gpio = monitor_gpio_init();

    result = esp_event_handler_register_with(loop_monitor_gpio, MONITOR_GPIO, MONITOR_GPIO_BUTTON_PRESSED, task_monitor_gpio_handler, loop_monitor_gpio);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo registrar la manejadora del evento MONITOR_GPIO - MONITOR_GPIO_BUTTON_PRESSED.");
        vTaskDelete(NULL);
    }
    // Inicialización del comoponentes Monitor GPIO - FIN
    //*************************************************************


    //*************************************************************
    // Inicialización del comoponentes Consola - INICIO
     loop_console = aniot_console_init();

    esp_event_handler_register_with(loop_console, ANIOT_CONSOLE_EVENT, CMD_MONITOR_EVENT, task_console_handler, loop_console);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo registrar la manejadora del evento MONITOR_GPIO - MONITOR_GPIO_BUTTON_PRESSED.");
        vTaskDelete(NULL);
    }
    // Inicialización del comoponentes Consola - FIN
    //*************************************************************

    ESP_LOGI(TAG, "Proceso de inicialización finalizado con éxito.\n");
    vTaskDelete(NULL);
}



static void deepSleep_event_handler(){
    ESP_LOGI(TAG, "Entrando en el modo Deep Sleep.\n");  
    wifi_disconnect();
    circularBuffer_destroy();
    TemperatureMonitor_stop();
    monitor_gpio_stop();
    aniot_console_stop();
    uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
    esp_deep_sleep_start();
}



static void task_mock_wifi_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{

    if (base != WIFI_MOCK)
    {
        ESP_LOGE(TAG, "ERROR..: Familia de eventos desconocida");
        return;
    }

    switch (id)
    {
    case WIFI_MOCK_EVENT_WIFI_CONNECTED:
        ESP_LOGI(TAG, "WIFI CONNECTED");
        break;
    case WIFI_MOCK_EVENT_WIFI_GOT_IP:
        ESP_LOGI(TAG, "WIFI GOT IP");
        send_data = 1;
        break;
    case WIFI_MOCK_EVENT_WIFI_DISCONNECTED:
        ESP_LOGW(TAG, "WIFI DISCONNECTED");
        send_data = 0;
        if (current_state == MONITOR)
        {
            ESP_LOGW(TAG, "CALLING WIFI_CONNECT() TO RECONNECT");
            wifi_connect();
        }
        break;
    default:
        ESP_LOGE(TAG, "ERROR..: UNKNOWN EVENT FROM WIFI_MOCK EVENT BASE");
        break;
    }
}



static void temperatureReaded_handler(void *registerArgs, esp_event_base_t baseEvent, int32_t idEvent, void *eventArgs)
{
    float data = 0.0f;
    if (baseEvent != TEMPERATURE_MONITOR_EVENTS)
    {
        ESP_LOGE(TAG, "ERROR..: Familia de eventos desconocida");
        return;
    }

    data = *(float *)eventArgs;
    if (idEvent == TEMPERATURE_READED_EVENT)
    {
        ESP_LOGI(TAG, "Temperatura: %f", data);
    }
    else if (idEvent == HUMIDITY_READED_EVENT)
    {
        ESP_LOGI(TAG, "Humedad: %f", data);
    }
    else
    {
        ESP_LOGE(TAG, "ERROR..: ID del evento desconocida");
        return;
    }

    if (send_data == 1)
    {
        while (getDataLeft() > 0)
        {
            float pendingData;
            pendingData = *(float *)readFromFlash(sizeof(float));
            send_data_wifi(&pendingData, sizeof(pendingData));
        }
        send_data_wifi(&data, sizeof(data));
    }
    else
    {
        writeToFlash(&data, sizeof(data));
    }
    return;
}



static void task_monitor_gpio_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{

    if (base != MONITOR_GPIO)
    {
        ESP_LOGE(TAG, "ERROR..: UNKNOWN EVENT FAMILY");
        return;
    }

    if (id == MONITOR_GPIO_BUTTON_PRESSED)
    {
        ESP_LOGW(TAG, "BUTTON PRESSED, GOING ON CLONSOLE MODE...");
        current_state = CONSOLE;
        send_data = 0;
        monitor_gpio_stop();
        TemperatureMonitor_stop();
        wifi_disconnect();
        aniot_console_start();
    }
    else
    {
        ESP_LOGE(TAG, "ERROR..: EVENT UNKNOWN");
        return;
    }
    return;
}

static void task_console_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{

    if (base != ANIOT_CONSOLE_EVENT)
    {
        ESP_LOGE(TAG, "ERROR..: UNKNOWN EVENT FAMILY");
        return;
    }

    if (id == CMD_MONITOR_EVENT)
    {
        ESP_LOGW(TAG, "EXIT FROM CONSOLE MODE...");
        aniot_console_stop();
        current_state = MONITOR;
        TemperatureMonitor_start();
        wifi_connect();
        monitor_gpio_resume();
    }
    else
    {
        ESP_LOGE(TAG, "ERROR..: EVENT UNKNOWN");
        return;
    }
    return;
}