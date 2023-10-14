#include "../si7021/si7021.h"
#include "../i2c_config/i2c_config.h"

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"


void monitor_init(){
    i2c_master_init();
    
}


float monitor_readTemperature(){

    float temperature = 0.0f;
    readTemperature(I2C_MASTER_NUM, &temperature);
    
    return temperature;
}