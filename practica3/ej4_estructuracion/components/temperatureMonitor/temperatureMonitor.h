#include "esp_system.h"
#include "sdkconfig.h"


esp_err_t monitor_init();

esp_err_t monitor_readTemperature(float *outputData);

esp_err_t monitor_readHumidity(float *outputData);