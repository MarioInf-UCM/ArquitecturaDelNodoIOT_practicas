/* esp_event (event loop library) basic example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef EVENT_SOURCE_H_
#define EVENT_SOURCE_H_

#include "esp_event.h"
#include "esp_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

// Declarations for the event source
#define TASK_ITERATIONS_COUNT        10      // number of times the task iterates
#define TASK_PERIOD                  500     // period of the task loop in milliseconds

ESP_EVENT_DECLARE_BASE(TEMPERATURE_MONITOR_EVENTS);

enum {
    TEMPERATURE_READED_EVENT,                   // Lanzado tras realizar la lectura de la temperatura
    HUMIDITY_READED_EVENT                       // Lanzado tras realizar la lectura de la humedad
};

#ifdef __cplusplus
}
#endif

#endif // #ifndef EVENT_SOURCE_H_
