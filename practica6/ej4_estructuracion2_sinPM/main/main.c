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


static const char *TAG = "MAIN";

// Event loops
esp_event_loop_handle_t loop_wifi_mock;
esp_event_loop_handle_t loop_monitor_gpio;
esp_event_loop_handle_t eventLoop_temperatureMonitor;
esp_event_loop_handle_t loop_console;




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
/*     loop_console = aniot_console_init();

    esp_event_handler_register_with(loop_console, ANIOT_CONSOLE_EVENT, CMD_MONITOR_EVENT, task_console_handler, loop_console);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo registrar la manejadora del evento MONITOR_GPIO - MONITOR_GPIO_BUTTON_PRESSED.");
        vTaskDelete(NULL);
    } */
    // Inicialización del comoponentes Consola - FIN
    //*************************************************************

    ESP_LOGI(TAG, "Finalizada inicialización de los componentes. Creando tarea para la gestión del modo Light Sleep");
    xTaskCreatePinnedToCore(&sleepTask_function, "SleepTask", 3072, (void *) NULL,  uxTaskPriorityGet(NULL), NULL, 0);


    vTaskDelete(NULL);
}