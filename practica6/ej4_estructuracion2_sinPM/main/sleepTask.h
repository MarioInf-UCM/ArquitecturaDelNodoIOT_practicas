void sleepTask_function(void *parameters);


void temperatureReaded_handler(void *registerArgs, esp_event_base_t baseEvent, int32_t idEvent, void *eventArgs);
void task_mock_wifi_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
void task_monitor_gpio_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
void task_console_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
