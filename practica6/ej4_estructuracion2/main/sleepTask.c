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
                TemperatureMonitor_readTemperatureAndHumidity();
                break;

            case ESP_SLEEP_WAKEUP_GPIO:
                ESP_LOGI(TAG, "Saliendo del modo Light Sleep. Motivo: GPIO wakeup.");
                break;

            default:
                ESP_LOGI(TAG, "Saliendo del modo Light Sleep. Motivo: Desconocido.");
                xQueueSendToBack(queue, &readValue, 0);
                break;
        }

        /* if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER) {
             example_wait_gpio_inactive();
        } */


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

    if (idEvent == TEMPERATURE_READED_EVENT){/*  */
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



/* 

 file included from /home/mario/esp/esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos/semphr.h:42,
                 from /home/mario/esp/esp-idf/components/driver/uart/include/driver/uart.h:17,
                 from /home/mario/Documentos/universidad/ArquitecturaDelNodoIOT/ArquitecturaDelNodoIOT_practicas/practica6/ej4_estructuracion2/main/sleepTask.c:12:
/home/mario/Documentos/universidad/ArquitecturaDelNodoIOT/ArquitecturaDelNodoIOT_practicas/practica6/ej4_estructuracion2/main/sleepTask.c: In function 'temperatureReaded_handler':
/home/mario/esp/esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos/queue.h:456:36: warning: passing argument 2 of 'xQueueGenericSend' makes pointer from integer without a cast [-Wint-conversion]
  456 |     xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_BACK )
      |                                    ^~~~~~~~~~~~~~~~~
      |                                    |
      |                                    int
/home/mario/Documentos/universidad/ArquitecturaDelNodoIOT/ArquitecturaDelNodoIOT_practicas/practica6/ej4_estructuracion2/main/sleepTask.c:140:5: note: in expansion of macro 'xQueueSendToBack'
  140 |     xQueueSendToBack(queue, true, 0);
      |     ^~~~~~~~~~~~~~~~
/home/mario/esp/esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos/queue.h:726:50: note: expected 'const void * const' but argument is of type 'int'
  726 |                               const void * const pvItemToQueue,
      |                               ~~

 */