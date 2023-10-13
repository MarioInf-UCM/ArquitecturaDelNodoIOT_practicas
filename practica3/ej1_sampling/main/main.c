#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "i2c_config.h"
#include "../components/si7021/si7021.h"


#define READ_PERIOD CONFIG_READ_PERIOD
#define TASK_PRIORITY CONFIG_TASK_PRIORITY

static const char *TAG = "ej1_sampling";
static float temperature = 0;

void taskFunction(void *parameters);


void app_main(void)
{

    i2c_master_init();
    xTaskCreatePinnedToCore(&taskFunction, "TareaMuestreo", 3072, (void *) READ_PERIOD, TASK_PRIORITY, NULL, 0);
    
    while(1){
        vTaskDelay(READ_PERIOD*1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Prioridad tarea Main: %d segundos.  Temperatura: %f", uxTaskPriorityGet(NULL), temperature);
    }
    vTaskDelete(NULL);
}



void taskFunction(void *parameters){

    int period = (int) parameters;
    while(1){
        readTemperature(I2C_MASTER_NUM, &temperature);
        ESP_LOGI(TAG, "Prioridad tarea Secundaria: %d.    Periodo de lectura %d segundos.", uxTaskPriorityGet(NULL), period);
        vTaskDelay(period*1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}