#include <stdio.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_event_base.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "monitor-gpio.h"

static const char *TAG = "MONITOR-GPIO";

esp_event_loop_handle_t loop_gpio;

// Define event base
ESP_EVENT_DEFINE_BASE(MONITOR_GPIO);

// Input params from menuconfig
#define NBUTTON CONFIG_NBUTTON
#define BUTTON_GPIO_PORT CONFIG_BUTTON_GPIO_PORT

// Define timers
esp_timer_handle_t gpio_timer;

uint64_t us_gpio_delay = NBUTTON * 1000000;

static void gpio_timer_callback(void *arg);

esp_event_loop_handle_t monitor_gpio_init()
{

    esp_event_loop_args_t loop_gpio_args = {
        .queue_size = 5,
        .task_name = "gpio_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 3072,
        .task_core_id = tskNO_AFFINITY};

    ESP_ERROR_CHECK(esp_event_loop_create(&loop_gpio_args, &loop_gpio));

    const esp_timer_create_args_t gpio_timer_args = {
        .callback = &gpio_timer_callback,
        .name = "gpio"};
    esp_timer_create(&gpio_timer_args, &gpio_timer);
    esp_timer_start_periodic(gpio_timer, us_gpio_delay);

    // Initialize GPIO Port
    gpio_reset_pin(CONFIG_BUTTON_GPIO_PORT);
    gpio_set_direction(CONFIG_BUTTON_GPIO_PORT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(CONFIG_BUTTON_GPIO_PORT, GPIO_PULLUP_PULLDOWN);

    ESP_LOGI(TAG, "GPIO Initialized");

    return loop_gpio;
}

static void gpio_timer_callback(void *arg)
{
    //ESP_LOGI(TAG, "Checking button status");
    if (gpio_get_level(CONFIG_BUTTON_GPIO_PORT))
    {
        ESP_LOGI(TAG, "Button pressed");
        esp_event_post_to(loop_gpio, MONITOR_GPIO, MONITOR_GPIO_BUTTON_PRESSED, NULL, 0, portMAX_DELAY);
    }
}

esp_err_t monitor_gpio_stop()
{
    esp_timer_stop(gpio_timer);

    return ESP_OK;
}

esp_err_t monitor_gpio_resume()
{
    esp_timer_start_periodic(gpio_timer, us_gpio_delay);
    return ESP_OK;
}