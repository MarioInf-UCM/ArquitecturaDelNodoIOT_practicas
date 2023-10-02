#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BLINK_GPIO CONFIG_LED_GPIO

static uint8_t s_led_state = 0;

static const char *TAG = "led";

static void configure_led(void)
{
    ESP_LOGI(TAG, "Configure to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

static void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(BLINK_GPIO, s_led_state);
}

static void periodic_timer_callback(void *arg);

void app_main(void)
{
    /* Configuramos el led */
    configure_led();

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        .name = "periodic"};
    esp_timer_handle_t periodic_timer;
    esp_timer_create(&periodic_timer_args, &periodic_timer);

    esp_timer_start_periodic(periodic_timer, 1000000);
    // esp_timer_stop(periodic_timer);
    // esp_timer_delete(periodic_timer);
}

static void periodic_timer_callback(void *arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Llamada periodica al timer, tiempo desde el arranque %lld us\n", time_since_boot);

    ESP_LOGI(TAG, "Cambiando el estado del LED a %s!", s_led_state == true ? "ENCENDIDO" : "APAGADO");
    blink_led();

    /* Cambiamos el estado del led */
    s_led_state = !s_led_state;
}
