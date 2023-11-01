idf_component_register(SRCS "monitor-gpio.c"
                    INCLUDE_DIRS "."
                    REQUIRES "driver" "esp_event" "esp_timer")