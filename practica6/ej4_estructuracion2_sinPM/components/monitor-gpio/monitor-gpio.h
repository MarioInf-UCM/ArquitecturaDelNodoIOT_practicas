#ifndef MONITOR_GPIO_H_
#define MONITOR_GPIO_H_

#include "esp_event.h"
#include "esp_timer.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

ESP_EVENT_DECLARE_BASE(MONITOR_GPIO);
enum
{
    MONITOR_GPIO_BUTTON_PRESSED,
    MONITOR_GPIO_BUTTON_PRESSED_FAIL
};

esp_event_loop_handle_t monitor_gpio_init();
void example_wait_gpio_inactive(void);
void gpio_timer_callback(void *arg);
esp_err_t monitor_gpio_stop();
esp_err_t monitor_gpio_resume();

#endif // #ifndef MOCK_WIFI_H_
