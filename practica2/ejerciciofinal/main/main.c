#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "i2c_config.h"
#include "si7021.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "ejerciciofinal";

static float temperature = 0;

#define LED1_GPIO CONFIG_LED1_GPIO
#define LED2_GPIO CONFIG_LED2_GPIO
#define LED3_GPIO CONFIG_LED3_GPIO
#define LED4_GPIO CONFIG_LED4_GPIO

bool leds_state[4];

static void configure_leds(void)
{
    ESP_LOGI(TAG, "Configuramos GPIO LEDs!");
    gpio_reset_pin(LED1_GPIO);
    gpio_reset_pin(LED2_GPIO);
    gpio_reset_pin(LED3_GPIO);
    gpio_reset_pin(LED4_GPIO);

    /* Configuramos los puertos GPIO como salida */
    gpio_set_direction(LED1_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED2_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED3_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED4_GPIO, GPIO_MODE_OUTPUT);
}

static void update_leds(void)
{
    /* Actualizamos el estado de GPIO para cada led*/
    gpio_set_level(LED1_GPIO, leds_state[0]);
    gpio_set_level(LED2_GPIO, leds_state[1]);
    gpio_set_level(LED3_GPIO, leds_state[2]);
    gpio_set_level(LED4_GPIO, leds_state[3]);
}

static void periodic_timer_callback(void *arg);
static void periodic_uart_timer_callback(void *arg);

void app_main(void)
{
    i2c_master_init();

    /* Configuramos los leds */
    configure_leds();

    // Configuramos el timer que muestrea la temperatura
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        .name = "periodic_temp_sample"};
    esp_timer_handle_t periodic_timer;
    esp_timer_create(&periodic_timer_args, &periodic_timer);

    esp_timer_start_periodic(periodic_timer, 1000000);

    // Configuramos el timer que muestra a través de la UART la última temperatura leida
    const esp_timer_create_args_t periodic_uart_timer_args = {
        .callback = &periodic_uart_timer_callback,
        .name = "periodic_temp_uart"};
    esp_timer_handle_t periodic_timer_uart;
    esp_timer_create(&periodic_uart_timer_args, &periodic_timer_uart);

    esp_timer_start_periodic(periodic_timer_uart, 10000000);
}

static void periodic_timer_callback(void *arg)
{
    // Leemos la temperatura
    readTemperature(I2C_MASTER_NUM, &temperature);
    printf("Timer Muestreo | Temperatura %f\n", temperature);

    // Actualizamos el estado de los leds en función de la temperatura
    leds_state[0] = (temperature >= 20) ? true : false;
    leds_state[1] = (temperature >= 22) ? true : false;
    leds_state[2] = (temperature >= 24) ? true : false;
    leds_state[3] = (temperature >= 26) ? true : false;

    // Actualizamos el estado de GPIO para los leds
    update_leds();
}

static void periodic_uart_timer_callback(void *arg)
{
    // Mostramos la última temperatura leida
    printf("Timer UART | Temperatura %f\n", temperature);
}
