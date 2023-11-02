#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "esp_event_base.h"
#include "esp_sleep.h"
#include "esp_check.h"

#include "../i2c_config/i2c_config.h"
#include "../si7021/si7021.h"
#include "temperatureMonitor.h"

static esp_event_loop_handle_t eventLoop;

// Define event base
ESP_EVENT_DEFINE_BASE(TEMPERATURE_MONITOR_EVENTS);
const static char *TAG = "TemperatureMonitor";

// Input params from menuconfig
#define READ_PERIOD CONFIG_READ_PERIOD

// Define timers
esp_timer_handle_t temperature_timer;
esp_timer_handle_t humidity_timer;

uint64_t us_read_period = READ_PERIOD * 1000000;


esp_event_loop_handle_t TemperatureMonitor_init()
{

    esp_err_t result;
    result = i2c_master_init();
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo inicializar el comunicador i2c.");
        vTaskDelete(NULL);
    }

    esp_event_loop_args_t eventLoop_args = {
        .queue_size = 20,
        .task_name = "temperatureMonitor_loopTask",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 3072,
        .task_core_id = tskNO_AFFINITY};

    result = esp_event_loop_create(&eventLoop_args, &eventLoop);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo crear el bucle de eventos.");
        return NULL;
    }

    //Definición del timer de Wakeup
    ESP_RETURN_ON_ERROR(esp_sleep_enable_timer_wakeup(us_read_period), TAG, "ERROR..: No se pudo configurar el timer Wakeup para la lectura de temperatura y humedad.");

    return eventLoop;
}


esp_err_t TemperatureMonitor_readTemperature()
{

    float temperature = 0.0f;
    esp_err_t result = readTemperature(I2C_MASTER_NUM, &temperature);
    if (result != ESP_OK)
    {
        ESP_LOGE(TAG, "ERROR..: No se pudo realizar la lectura de temperatura.");
        return result;
    }

    result = esp_event_post_to(eventLoop, TEMPERATURE_MONITOR_EVENTS, TEMPERATURE_READED_EVENT, &temperature, sizeof(temperature), 0);
    if (result != ESP_OK)
    {
        ESP_LOGE(TAG, "ERROR..: No se pudo lanzar el evento TEMPERATURE_MONITOR_EVENTS - TEMPERATURE_READED_EVENT.");
        return result;
    }

    return ESP_OK;
}


esp_err_t TemperatureMonitor_readHumidity()
{
    float humidity = 0.0f;
    esp_err_t result = readHumidity(I2C_MASTER_NUM, &humidity);
    if (result != ESP_OK)
    {
        ESP_LOGE(TAG, "ERROR..: No se pudo realizar la lectura de humedad.");
        return result;
    }

    result = esp_event_post_to(eventLoop, TEMPERATURE_MONITOR_EVENTS, HUMIDITY_READED_EVENT, &humidity, sizeof(humidity), 0);
    if (result != ESP_OK)
    {
        ESP_LOGE(TAG, "ERROR..: No se pudo lanzar el evento TEMPERATURE_MONITOR_EVENTS - HUMIDITY_READED_EVENT.");
        return result;
    }

    return ESP_OK;
}


esp_err_t TemperatureMonitor_readTemperatureAndHumidity(){

    struct Data data={
        .temperature = 0.0f,
        .humidity = 0.0f,
    };
    esp_err_t result;
    
    result = readTemperature(I2C_MASTER_NUM, &data.temperature);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo realizar la lectura de temperatura.");
        return result;
    }
    
    result = readHumidity(I2C_MASTER_NUM, &data.humidity);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo realizar la lectura de humedad.");
        return result;
    }

    result = esp_event_post_to(eventLoop, TEMPERATURE_MONITOR_EVENTS, EMPERATURE_AND_HUMIDITY_READED_EVENT, &data, sizeof(data), 0);
    if (result != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo lanzar el evento TEMPERATURE_MONITOR_EVENTS - EMPERATURE_AND_HUMIDITY_READED_EVENT.");
        return result;
    }

    return ESP_OK;
}


esp_err_t TemperatureMonitor_stop()
{
    esp_timer_stop(temperature_timer);
    esp_timer_stop(humidity_timer);

    return ESP_OK;
}


esp_err_t TemperatureMonitor_start()
{
    esp_timer_start_periodic(temperature_timer, us_read_period);
    esp_timer_start_periodic(humidity_timer, us_read_period);

    return ESP_OK;
}