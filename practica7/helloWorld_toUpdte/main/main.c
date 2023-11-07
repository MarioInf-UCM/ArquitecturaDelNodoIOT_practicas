#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


const char * TAG = "MAIN";

void app_main(void){

    ESP_LOGI(TAG,"THIS IS A NOEW FIRMWARE VERSION -> HELLO WORLD :)\n");
    printf("Otra cadena\n");
    vTaskDelete(NULL);
}
