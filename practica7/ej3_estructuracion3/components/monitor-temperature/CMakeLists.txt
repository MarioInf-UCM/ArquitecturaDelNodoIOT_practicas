idf_component_register(SRCS "temperatureMonitor.c"
                    INCLUDE_DIRS "."
                    REQUIRES "driver" "esp_event" "esp_timer" "i2c_config" "si7021")