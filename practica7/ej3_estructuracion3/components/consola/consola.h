#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "esp_event.h"
#include "mock-flash.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

ESP_EVENT_DECLARE_BASE(ANIOT_CONSOLE_EVENT);

enum
{
    CMD_MONITOR_EVENT
};

esp_event_loop_handle_t aniot_console_init();
void aniot_console_start();
void aniot_console_stop();

#endif // #ifndef CONSOLA_H_
