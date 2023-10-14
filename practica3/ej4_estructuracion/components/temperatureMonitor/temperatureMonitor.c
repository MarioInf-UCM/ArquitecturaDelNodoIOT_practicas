#include "../si7021/si7021.h"
#include "../i2c_config/i2c_config.h"

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"


esp_err_t monitor_init(){

    esp_err_t result = i2c_master_init();
    return result;
}


esp_err_t monitor_readTemperature(float *outputData){

    esp_err_t result = readTemperature(I2C_MASTER_NUM, &outputData);
    return result;
}


esp_err_t monitor_readHumidity(float *outputData){

    esp_err_t result = readHumidity(I2C_MASTER_NUM, &outputData);
    return result;
}