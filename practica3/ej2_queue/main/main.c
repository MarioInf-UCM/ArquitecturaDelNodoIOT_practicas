#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "i2c_config.h"
#include "../components/si7021/si7021.h"


#define READ_PERIOD CONFIG_READ_PERIOD
#define TASK_PRIORITY CONFIG_TASK_PRIORITY
#define QUEUE_SIZE CONFIG_QUEUE_SIZE

static const char *TAG = "ej2_queue`";

void taskFunction(void *parameters);


struct taskParam{
    QueueHandle_t queue;
    int period;
}TaskParam;



void app_main(void)
{

    i2c_master_init();
    QueueHandle_t queue = xQueueCreate(QUEUE_SIZE, sizeof(float));
    struct taskParam params = {
        .queue=queue, 
        .period=READ_PERIOD
    };
    float readValue = 0.0;
    BaseType_t statusReturn;
    const TickType_t ticksToWait = pdMS_TO_TICKS(100);

    xTaskCreatePinnedToCore(&taskFunction, "TareaMuestreo", 3072, (void *) &params, TASK_PRIORITY, NULL, 0);
    ESP_LOGI(TAG, "***Tarea Principal preparada. Prioridad: %d.***", uxTaskPriorityGet(NULL));
    while(1){
        statusReturn =  xQueueReceive(queue, &readValue, ticksToWait);
        if(statusReturn == pdTRUE){
            ESP_LOGI(TAG, "Temperatura: %f", readValue);
        }
    }
    vTaskDelete(NULL);
}



void taskFunction(void *parameters){

    struct taskParam *params = (struct taskParam *) parameters;
    float temperature = 0.0;

    ESP_LOGI(TAG, "***Tarea Secundaria (muestradora) preparada. Prioridad: %d. Periodo: %d s.***", uxTaskPriorityGet(NULL), params->period);
    while(1){
        readTemperature(I2C_MASTER_NUM, &temperature);
        xQueueSendToBack(params->queue, &temperature, 0);
        vTaskDelay(params->period*1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}