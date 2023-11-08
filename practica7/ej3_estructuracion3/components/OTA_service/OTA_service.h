#ifndef OTA_SERVICE_H_
#define OTA_SERVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

ESP_EVENT_DECLARE_BASE(OTA_SERVICE_EVENTS);
enum
{
    OTA_SERVICE_FAIL_EVENT
};


void OTA_service_mainApp(void);
esp_event_loop_handle_t OTA_service_init(void);

#endif