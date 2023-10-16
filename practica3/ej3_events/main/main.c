#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "i2c_config.h"

#include "../components/si7021/si7021.h"
#include "event_source.h"
#include "esp_event_base.h"


#define READ_PERIOD CONFIG_READ_PERIOD
#define TASK_PRIORITY CONFIG_TASK_PRIORITY

void taskFunction(void *parameters);
void event_handler(void *registerArgs, esp_event_base_t baseEvent, int32_t idEvent, void* eventArgs);

static const char *TAG = "ej3_events";
static esp_event_loop_handle_t eventLoop;
ESP_EVENT_DEFINE_BASE(HALL_EVENT);



void app_main(void)
{

    float temperature=0.0;
    esp_event_loop_args_t eventLoop_args ={
        .queue_size = 10,
        .task_name = "eventLoop_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 3072,
        .task_core_id = tskNO_AFFINITY
    };
    esp_event_loop_create(&eventLoop_args, &eventLoop);
    esp_event_handler_register_with(eventLoop, HALL_EVENT, HALL_EVENT_NEWSAMPLE, event_handler, eventLoop);

    i2c_master_init();
    xTaskCreatePinnedToCore(&taskFunction, "TareaMuestreo", 3072, (void *) READ_PERIOD, TASK_PRIORITY, NULL, 0);
    
    ESP_LOGI(TAG, "***Tarea Principal preparada. Prioridad: %d.***", uxTaskPriorityGet(NULL));
    vTaskDelete(NULL);
}



void event_handler(void *registerArgs, esp_event_base_t baseEvent, int32_t idEvent, void *eventArgs){

    float temperature=0.0f;
    if(registerArgs == eventLoop){
        temperature = *(float *)eventArgs;
        ESP_LOGI(TAG, "Temperatura: %f", temperature);    
    }else{
        ESP_LOGI(TAG, "Evento lanzado desde el lugar equivocado."); 
    }
}



void taskFunction(void *parameters){

    int period = (int) parameters;
    float temperature = 0.0;
    ESP_LOGI(TAG, "***Tarea Secundaria (muestradora) preparada. Prioridad: %d. Periodo: %d s.***", uxTaskPriorityGet(NULL), period);
    while(1){
        readTemperature(I2C_MASTER_NUM, &temperature);
        esp_event_post_to(eventLoop, HALL_EVENT, HALL_EVENT_NEWSAMPLE, &temperature, sizeof(temperature), 0);
        vTaskDelay(period*1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}