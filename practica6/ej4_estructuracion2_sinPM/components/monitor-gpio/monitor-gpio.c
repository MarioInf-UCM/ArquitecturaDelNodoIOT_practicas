#include <stdio.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_event_base.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "monitor-gpio.h"
#include "esp_check.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define NBUTTON CONFIG_NBUTTON
#define BUTTON_GPIO_PORT CONFIG_BUTTON_GPIO_PORT
#define GPIO_WAKEUP_LEVEL CONFIG_GPIO_WAKEUP_LEVEL

static const char *TAG = "MONITOR-GPIO";
esp_event_loop_handle_t loop_gpio;
ESP_EVENT_DEFINE_BASE(MONITOR_GPIO);
esp_timer_handle_t gpio_timer;
uint64_t us_gpio_delay = NBUTTON * 1000000;
void gpio_timer_callback(void *arg);


esp_event_loop_handle_t monitor_gpio_init()
{

    esp_err_t status;

    esp_event_loop_args_t loop_gpio_args = {
        .queue_size = 5,
        .task_name = "gpio_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 3072,
        .task_core_id = tskNO_AFFINITY
    };
    ESP_ERROR_CHECK(esp_event_loop_create(&loop_gpio_args, &loop_gpio));


    gpio_config_t config = {
            .pin_bit_mask = BIT64(BUTTON_GPIO_PORT),
            .mode = GPIO_MODE_INPUT,
            .pull_down_en = false,
            .pull_up_en = true,
            .intr_type = GPIO_INTR_DISABLE
    };

    status = gpio_config(&config);
    if(status != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo llevar a cabo la inicialición del GPIO %d", BUTTON_GPIO_PORT);
    }

    status = gpio_wakeup_enable(BUTTON_GPIO_PORT, GPIO_WAKEUP_LEVEL == 0 ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
    if(status != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo habilitar el GPIO wakeup");
    }
 
    status = esp_sleep_enable_gpio_wakeup();
    if(status != ESP_OK){
        ESP_LOGE(TAG, "ERROR..: No se pudo configurar el GPIO wakeup");
    }

    /* Make sure the GPIO is inactive and it won't trigger wakeup immediately */
    example_wait_gpio_inactive();
    ESP_LOGI(TAG, "GPIO wakeup inicializado con éxito.");

    return loop_gpio;
}



void example_wait_gpio_inactive(void){
    ESP_LOGI(TAG, "Esperando por que el GPIO %d vuelva a su posición original.", BUTTON_GPIO_PORT);
    while (gpio_get_level(BUTTON_GPIO_PORT) == GPIO_WAKEUP_LEVEL) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_LOGI(TAG, "GPIO %d Ha restablecido su posición original.", BUTTON_GPIO_PORT);
}


void gpio_timer_callback(void *arg)
{
    ESP_LOGI(TAG, "Checking button status");
    if (gpio_get_level(CONFIG_BUTTON_GPIO_PORT))
    {
        ESP_LOGI(TAG, "Button pressed");
        esp_event_post_to(loop_gpio, MONITOR_GPIO, MONITOR_GPIO_BUTTON_PRESSED, NULL, 0, portMAX_DELAY);
    }else{
        esp_event_post_to(loop_gpio, MONITOR_GPIO, MONITOR_GPIO_BUTTON_PRESSED_FAIL, NULL, 0, portMAX_DELAY);
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
