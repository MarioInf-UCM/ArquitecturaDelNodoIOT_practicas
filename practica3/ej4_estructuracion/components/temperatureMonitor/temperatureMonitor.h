#include "esp_system.h"
#include "sdkconfig.h"
#include "esp_event_base.h"


esp_event_loop_handle_t TemperatureMonitor_init();

esp_err_t TemperatureMonitor_readTemperature();

esp_err_t TemperatureMonitor_readHumidity();