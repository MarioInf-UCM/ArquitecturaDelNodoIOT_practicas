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
#include "esp_sleep.h"
#include "driver/uart.h"

#include "mock_wifi.h"
#include "temperatureMonitor.h"
#include "mock-flash.h"
#include "monitor-gpio.h"
#include "consola.h"
#include "esp_event_base.h"

static const char *TAG = "MAIN";

// Event loops
esp_event_loop_handle_t loop_wifi_mock;
esp_event_loop_handle_t loop_monitor_gpio;
esp_event_loop_handle_t eventLoop_temperatureMonitor;
esp_event_loop_handle_t loop_console;


static void goToSleep();
static void temperatureReaded_handler(void *registerArgs, esp_event_base_t baseEvent, int32_t idEvent, void *eventArgs);
static void task_mock_wifi_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
static void task_monitor_gpio_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
static void task_console_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);

// Send data when connected
bool send_data = 0;

// Modes & current state
enum state{
    MONITOR,
    CONSOLE
};
enum state current_state = MONITOR;


void app_main(){

    ESP_LOGI(TAG, "Comenzando inicialización de componentes.");
    esp_err_t result;


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

    result = esp_event_handler_register_with(eventLoop_temperatureMonitor, TEMPERATURE_MONITOR_EVENTS, EMPERATURE_AND_HUMIDITY_READED_EVENT, temperatureReaded_handler, eventLoop_temperatureMonitor);
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

    ESP_LOGI(TAG, "Finalizada inicialización de los componentes.");
    goToSleep();

    vTaskDelete(NULL);
}


static void goToSleep(){

    ESP_LOGI(TAG, "Preparándose para entrar en modo Light Sleep.");
    wifi_disconnect();
    ESP_LOGI(TAG, "Entrando en modo Light Sleep.\n");
    uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
    esp_light_sleep_start();
    
    do{

        switch (esp_sleep_get_wakeup_cause()) {
            case ESP_SLEEP_WAKEUP_TIMER:
                ESP_LOGI(TAG, "Saliendo del modo Light Sleep. Motivo: Timer wakeup.");
                TemperatureMonitor_readTemperatureAndHumidity();
                break;

            case ESP_SLEEP_WAKEUP_GPIO:
                ESP_LOGI(TAG, "\nSaliendo del modo Light Sleep. Motivo: GPIO wakeup.");
                break;

            default:
                ESP_LOGI(TAG, "\nSaliendo del modo Light Sleep. Motivo: Desconocido.");
                break;
        }

        if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER) {
           // example_wait_gpio_inactive();
        }

    }while(esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER && esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_GPIO);

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



static void temperatureReaded_handler(void *registerArgs, esp_event_base_t baseEvent, int32_t idEvent, void *eventArgs){

    if (baseEvent != TEMPERATURE_MONITOR_EVENTS){
        ESP_LOGE(TAG, "ERROR..: Familia de eventos desconocida");
        return;
    }

    struct Data data = { 
        .temperature = 0.0f,
        .humidity = 0.0f,
    };
    float pendingData;

    wifi_connect();
    if (send_data == 1){
        while (getDataLeft() > 0){
            pendingData = *(float *)readFromFlash(sizeof(float));
            send_data_wifi(&pendingData, sizeof(pendingData));
        }
    }

    if (idEvent == TEMPERATURE_READED_EVENT){
        data.temperature = *(float *)eventArgs;
        ESP_LOGI(TAG, "Temperatura: %f", data.temperature);
        if (send_data == 1){
            send_data_wifi(&data.temperature , sizeof(data.temperature ));
        }else{
           writeToFlash(&data.temperature, sizeof(data.temperature));         
        }

    }else if (idEvent == HUMIDITY_READED_EVENT){
        data.humidity = *(float *)eventArgs;
        ESP_LOGI(TAG, "Humedad: %f", data.humidity);
        if (send_data == 1){
            send_data_wifi(&data.humidity , sizeof(data.humidity ));
        }else{
           writeToFlash(&data.humidity, sizeof(data.humidity));         
        }

    }else if (idEvent == EMPERATURE_AND_HUMIDITY_READED_EVENT){
        data = *(struct Data *)eventArgs;
        ESP_LOGI(TAG, "Temperatura: %f", data.temperature);
        ESP_LOGI(TAG, "Humedad: %f", data.humidity);
        if (send_data == 1){
            send_data_wifi(&data.temperature , sizeof(data.temperature ));
            send_data_wifi(&data.humidity , sizeof(data.humidity ));
        }else{
            writeToFlash(&data.temperature, sizeof(data.temperature));         
            writeToFlash(&data.humidity, sizeof(data.humidity));         
        }

    }else{
        ESP_LOGE(TAG, "ERROR..: ID del evento desconocida");
        return;
    }


    goToSleep();
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
        //TemperatureMonitor_stop();
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
        //TemperatureMonitor_start(); 
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