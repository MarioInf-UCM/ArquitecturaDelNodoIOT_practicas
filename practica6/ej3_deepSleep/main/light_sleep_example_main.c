/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "light_sleep_example.h"
#include <inttypes.h>
#include "esp_system.h"

#define TIME_TO_SLEEP_AFTER_INITIAL_CONFIGURATION 3000
#define TIMER_PRINTTIME_PERIOD 2000000
#define NUM_TIMES_LIGHT_SLEEP 3
#define NUM_TIMES_DEEP_SLEEP 3


void timerHandler_printTime(){
    printf("Timer de ejecución periódica. Tiempo: %lld\n", esp_timer_get_time());
    return;
}


static void light_sleep_task(void *args){

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &timerHandler_printTime,
        .name = "timer_printTime"};
    esp_timer_handle_t periodic_timer;
    esp_timer_create(&periodic_timer_args, &periodic_timer);
    esp_timer_start_periodic(periodic_timer, TIMER_PRINTTIME_PERIOD);


    printf("Comenzamos a dormir después de la configuración inicial\n");
    vTaskDelay( TIME_TO_SLEEP_AFTER_INITIAL_CONFIGURATION / portTICK_PERIOD_MS);
    printf("Finalizamos de dormir después de la configuración inicial\n");  
   

    for(int i=0 ; i<NUM_TIMES_LIGHT_SLEEP ; i++){
        printf("*** Entramos en el modo Light Sleep ***\n");  

        uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
        int64_t t_before_us = esp_timer_get_time();
        esp_light_sleep_start();

        
        int64_t t_after_us = esp_timer_get_time();
        const char* wakeup_reason;
        switch (esp_sleep_get_wakeup_cause()) {
            case ESP_SLEEP_WAKEUP_TIMER:
                wakeup_reason = "timer";
                break;
            case ESP_SLEEP_WAKEUP_GPIO:
                wakeup_reason = "pin";
                break;
            case ESP_SLEEP_WAKEUP_UART:
                wakeup_reason = "uart";
                vTaskDelay(1);
                break;

            #if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
                case ESP_SLEEP_WAKEUP_TOUCHPAD:
                    wakeup_reason = "touch";
                    break;
            #endif

            default:
                wakeup_reason = "other";
                break;
        }

        #if CONFIG_NEWLIB_NANO_FORMAT
            printf("*** Despertamos del modo Light Sleep (%d), motivo: %s, t=%d ms. Se ha dormido por %d ms ***\n\n",
                    i+1,wakeup_reason, (int) (t_after_us / 1000), (int) ((t_after_us - t_before_us) / 1000));
        #else
            printf("*** Despertamos del modo Light Sleep (%d), motivo: %s, t=%lld ms. Se ha dormido por %lld ms ***\n\n",
                    i+1, wakeup_reason, t_after_us / 1000, (t_after_us - t_before_us) / 1000);
        #endif

        if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO) {
            example_wait_gpio_inactive();
        }
    }

    printf("\n\n\n***Entramos en el modo Deep Sleep\n");  

    uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
    int64_t t_before_us = esp_timer_get_time();
    esp_deep_sleep_start();




    printf("Finalizando ejecución.\n");
    esp_timer_stop(periodic_timer);
    esp_timer_delete(periodic_timer);
    printf("Timer periódico eliminado.\n");
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    printf("Fuentes para despertar el SoC eliminadas.\n");
    printf("Programa finalizado con éxito.\n");
    
    vTaskDelete(NULL);
}


void app_main(void)
{
    /* Enable wakeup from light sleep by gpio */
    example_register_gpio_wakeup();
    /* Enable wakeup from light sleep by timer */
    example_register_timer_wakeup();
    /* Enable wakeup from light sleep by uart */
    example_register_uart_wakeup();

    #if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
        /* Enable wakeup from light sleep by touch element */
        example_register_touch_wakeup();
    #endif
      
    xTaskCreate(light_sleep_task, "light_sleep_task", 4096, NULL, 6, NULL);
}

