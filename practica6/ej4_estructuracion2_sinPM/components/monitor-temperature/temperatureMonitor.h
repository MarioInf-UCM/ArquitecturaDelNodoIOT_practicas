#ifndef TEMPERATUREMONITOR_H_
#define TEMPERATUREMONITOR_H_

#include "esp_event.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "sdkconfig.h"
#include "esp_event_base.h"


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

// Declarations for the event source
#define TASK_ITERATIONS_COUNT 10 // number of times the task iterates
#define TASK_PERIOD 500          // period of the task loop in milliseconds

ESP_EVENT_DECLARE_BASE(TEMPERATURE_MONITOR_EVENTS);

typedef struct Data{
    float temperature;
    float humidity;
}data;

enum{
    TEMPERATURE_READED_EVENT,               // Lanzado tras realizar la lectura de la temperatura
    HUMIDITY_READED_EVENT ,                 // Lanzado tras realizar la lectura de la humedad
    EMPERATURE_AND_HUMIDITY_READED_EVENT,   // Lanzado tras realizar la lectura de la temeratura y la humedad
};

esp_event_loop_handle_t TemperatureMonitor_init();
esp_err_t TemperatureMonitor_readTemperature();
esp_err_t TemperatureMonitor_readHumidity();
esp_err_t TemperatureMonitor_readTemperatureAndHumidity();
esp_err_t TemperatureMonitor_stop();
esp_err_t TemperatureMonitor_start();

#endif // #ifndef EVENT_SOURCE_H_