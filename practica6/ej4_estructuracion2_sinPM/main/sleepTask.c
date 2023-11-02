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
#include "sleepTask.h"

#define BUTTON_GPIO_PORT CONFIG_BUTTON_GPIO_PORT
#define GPIO_WAKEUP_LEVEL CONFIG_GPIO_WAKEUP_LEVEL
#define QUEUE_SIZE 10

// Modes & current state
enum state{
    MONITOR,
    CONSOLE
};
static enum state current_state = MONITOR;
static const char *TAG = "MAIN";
static bool send_data = 0;     // Send data when connected
static QueueHandle_t queue;


void sleepTask_function(void *parameters){

    BaseType_t statusReturn;
    bool readValue = true;
    queue = xQueueCreate(QUEUE_SIZE, sizeof(bool));

    do{
        ESP_LOGI(TAG, "Preparándose para entrar en modo Light Sleep.");
        wifi_disconnect();
        ESP_LOGI(TAG, "Entrando en modo Light Sleep.\n");
        uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
        esp_light_sleep_start();
        

        switch (esp_sleep_get_wakeup_cause()) {
            case ESP_SLEEP_WAKEUP_TIMER:
                ESP_LOGI(TAG, "Saliendo del modo Light Sleep. Motivo: Timer wakeup.");
                wifi_connect();
                wifi_getIp();
                TemperatureMonitor_readTemperatureAndHumidity();
                break;

            case ESP_SLEEP_WAKEUP_GPIO:
                ESP_LOGI(TAG, "Saliendo del modo Light Sleep. Motivo: GPIO wakeup.");
                wifi_connect();
                wifi_getIp();
                gpio_timer_callback(NULL);
                break;

            default:
                ESP_LOGI(TAG, "Saliendo del modo Light Sleep. Motivo: Desconocido.");
                xQueueSendToBack(queue, &readValue, 0);
                break;
        }
        

        if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO) {
            example_wait_gpio_inactive();
        }

        statusReturn =  xQueueReceive(queue, &readValue, pdMS_TO_TICKS(10000));
        if(statusReturn != pdTRUE){
            ESP_LOGE(TAG, "ERROR (%d): No se pudo leer de la cola de retorno al modo Light Sleep. Se pasará al modo forzosamente", statusReturn);
        }

    }while(true);

    vTaskDelete(NULL);
}



void temperatureReaded_handler(void *registerArgs, esp_event_base_t baseEvent, int32_t idEvent, void *eventArgs){

    if (baseEvent != TEMPERATURE_MONITOR_EVENTS){
        ESP_LOGE(TAG, "ERROR..: Familia de eventos desconocida");
        return;
    }

    struct Data data = { 
        .temperature = 0.0f,
        .humidity = 0.0f,
    };
    float pendingData;
    bool sendValue = true;

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

    xQueueSendToBack(queue, &sendValue, 0);
    return;
}



void task_mock_wifi_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data){
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

    return;
}



void task_monitor_gpio_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data){

    bool sendValue = true;

    if (base != MONITOR_GPIO)
    {
        ESP_LOGE(TAG, "ERROR..: UNKNOWN EVENT FAMILY");
        return;
    }

    if (id == MONITOR_GPIO_BUTTON_PRESSED)
    {
        ESP_LOGI(TAG, "BUTTON PRESSED, GOING ON CLONSOLE MODE");
        current_state = CONSOLE;
        send_data = 0;
        monitor_gpio_stop();
        wifi_disconnect();
        aniot_console_start();
    }else if(id == MONITOR_GPIO_BUTTON_PRESSED_FAIL){
        ESP_LOGW(TAG, "BUTTON NOT PRESSED, RETURNING");
        xQueueSendToBack(queue, &sendValue, 0);
    }
    else
    {
        ESP_LOGE(TAG, "ERROR..: EVENT UNKNOWN");
        return;
    }
    return;
}



void task_console_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data){

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