#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#include "i2c_config.h"
#include "si7021.h"
#include "temperatureMonitor.h"
#include "../events/temperature_monitor_events.h"
#include "esp_event_base.h"

#define READ_PERIOD CONFIG_READ_PERIOD

static void temperatureReaded_handled(void *registerArgs, esp_event_base_t baseEvent, int32_t idEvent, void *eventArgs);
static const char* TAG = "ej4_estructuracion";
esp_event_loop_handle_t eventLoop_temperatureMonitor;
ESP_EVENT_DEFINE_BASE(TEMPERATURE_MONITOR_EVENTS);

void app_main(){

    esp_err_t result;
    result  = i2c_master_init();
        if(result !=ESP_OK){ ESP_LOGE(TAG, "ERROR..: No se pudo inicializar el comunicador i2c."); vTaskDelete(NULL); }

    eventLoop_temperatureMonitor = TemperatureMonitor_init();
        //if(result !=ESP_OK){ ESP_LOGE(TAG, "ERROR..: No se pudo inicializar el componente TemperatureMonitor."); vTaskDelete(NULL);}   

    result = esp_event_handler_register_with(eventLoop_temperatureMonitor, TEMPERATURE_MONITOR_EVENTS, TEMPERATURE_READED_EVENT, temperatureReaded_handled, eventLoop_temperatureMonitor);
        if(result !=ESP_OK){ ESP_LOGE(TAG, "ERROR..: No se pudo registrar la manejadora del evento TEMPERATURE_MONITOR_EVENTS - TEMPERATURE_READED_EVENT."); vTaskDelete(NULL);}   

    result = esp_event_handler_register_with(eventLoop_temperatureMonitor, TEMPERATURE_MONITOR_EVENTS, HUMIDITY_READED_EVENT, temperatureReaded_handled, eventLoop_temperatureMonitor);
        if(result !=ESP_OK){ ESP_LOGE(TAG, "ERROR..: No se pudo registrar la manejadora del evento TEMPERATURE_MONITOR_EVENTS - HUMIDITY_READED_EVENT."); vTaskDelete(NULL);}   


    while (1)
    {
        TemperatureMonitor_readTemperature();
        vTaskDelay(READ_PERIOD*1000 / portTICK_PERIOD_MS);
        TemperatureMonitor_readHumidity();
        vTaskDelay(READ_PERIOD*1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}



static void temperatureReaded_handled(void *registerArgs, esp_event_base_t baseEvent, int32_t idEvent, void *eventArgs){

    float data=0.0f;
    if(baseEvent != TEMPERATURE_MONITOR_EVENTS){
        ESP_LOGE(TAG, "ERROR..: Familia de eventos desnocida");
        return;
    }

    data = *(float *)eventArgs;
    if(idEvent == TEMPERATURE_READED_EVENT){
        ESP_LOGI(TAG, "Temperatura: %f", data);    
    }else if(idEvent == HUMIDITY_READED_EVENT){
        ESP_LOGI(TAG, "Humedad: %f", data);    
    }else{
        ESP_LOGE(TAG, "ERROR..: ID del evento desnocida");
        return;
    }

    return;
}

