#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "esp_event_base.h"

#include "../si7021/si7021.h"
#include "../i2c_config/i2c_config.h"
#include "../../events/temperature_monitor_events.h"

static esp_event_loop_handle_t eventLoop;
    ESP_EVENT_DEFINE_BASE(HALL_EVENT);
const static char *TAG="TemperatureMonitor";


esp_event_loop_handle_t TemperatureMonitor_init(){

    esp_event_loop_args_t eventLoop_args ={
        .queue_size = 20,
        .task_name = "temperatureMonitor_loopTask",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 3072,
        .task_core_id = tskNO_AFFINITY
    };
    esp_err_t result;

    result = esp_event_loop_create(&eventLoop_args, &eventLoop);
        if(result !=ESP_OK){ESP_LOGE(TAG, "ERROR..: No se pudo crear el bucle de eventos."); return NULL;}
    
    return eventLoop;
}


esp_err_t TemperatureMonitor_readTemperature(){

    float temperature = 0.0f;
    esp_err_t result = readTemperature(I2C_MASTER_NUM, &temperature);
        if(result !=ESP_OK){ ESP_LOGE(TAG, "ERROR..: No se pudo realizar la lectura de temperatura."); return result; }

    result = esp_event_post_to(eventLoop, TEMPERATURE_MONITOR_EVENTS, TEMPERATURE_READED_EVENT, &temperature, sizeof(temperature), 0);
        if(result !=ESP_OK){ ESP_LOGE(TAG, "ERROR..: No se pudo lanzar el evento TEMPERATURE_MONITOR_EVENTS - TEMPERATURE_READED_EVENT."); return result; }

    return ESP_OK;
}


esp_err_t TemperatureMonitor_readHumidity(){
    
    float humidity = 0.0f;
    esp_err_t result = readHumidity(I2C_MASTER_NUM, &humidity);
        if(result !=ESP_OK){ ESP_LOGE(TAG, "ERROR..: No se pudo realizar la lectura de humedad."); return result; }

    esp_event_post_to(eventLoop, TEMPERATURE_MONITOR_EVENTS, HUMIDITY_READED_EVENT, &humidity, sizeof(humidity), 0);
        if(result !=ESP_OK){ ESP_LOGE(TAG, "ERROR..: No se pudo lanzar el evento TEMPERATURE_MONITOR_EVENTS - HUMIDITY_READED_EVENT."); return result; }

    return ESP_OK;
}