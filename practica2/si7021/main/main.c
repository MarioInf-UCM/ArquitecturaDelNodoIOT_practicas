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

#define TIME_WAIT_TASK 2

void taskShowTemperature();

void app_main(void)
{

    i2c_master_init();
    xTaskCreate(&taskShowTemperature, "taskShowTemperatura", 2048, NULL, 5, NULL);
}

void taskShowTemperature()
{

    while (1)
    {
        float temperature = 0;
        readTemperature(I2C_MASTER_NUM, &temperature);

        printf("Temperatura %f\n", temperature);

        vTaskDelay(TIME_WAIT_TASK * 1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
