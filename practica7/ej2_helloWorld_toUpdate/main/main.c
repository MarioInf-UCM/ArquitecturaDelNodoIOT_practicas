#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


const char * TAG = "MAIN";

void app_main(void){

    ESP_LOGI(TAG,"THIS IS A NEW FIRMWARE VERSION -> HELLO WORLD :)\n");
    vTaskDelete(NULL);
}
