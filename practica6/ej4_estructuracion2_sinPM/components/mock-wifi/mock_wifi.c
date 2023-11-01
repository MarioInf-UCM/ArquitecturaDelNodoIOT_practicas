#include <stdio.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_event_base.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "mock_wifi.h"

static const char *TAG = "MOCK_WIFI";

esp_event_loop_handle_t loop_connect;

// Define event base
ESP_EVENT_DEFINE_BASE(WIFI_MOCK);

// Input params from menuconfig
#define CONNECT_DELAY CONFIG_CONNECT_DELAY
#define IP_DELAY CONFIG_IP_DELAY
#define DISCONNECT_DELAY CONFIG_DISCONNECT_DELAY

// Define timers
esp_timer_handle_t ip_timer;
esp_timer_handle_t disconnection_timer;

uint64_t ms_connect_delay = CONNECT_DELAY * 1000;
uint64_t us_ip_delay = IP_DELAY * 1000000;
uint64_t us_disconnect_delay = DISCONNECT_DELAY * 1000000;

enum mock_wifi_state wifi_state = NOT_INITIALIZED;

const char *stateNames[] = {"NOT_INITIALIZED", "INITIALIZED", "CONNECTED", "CONNECTED_WITH_IP", "DISCONNECTED"};

static void ip_timer_callback(void *arg);
static void disconnection_timer_callback(void *arg);

esp_event_loop_handle_t wifi_mock_init()
{

    esp_event_loop_args_t loop_connect_args = {
        .queue_size = 3,
        .task_name = "mock_wifi_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 3072,
        .task_core_id = tskNO_AFFINITY};

    ESP_ERROR_CHECK(esp_event_loop_create(&loop_connect_args, &loop_connect));

    wifi_state = INITIALIZED;

    const esp_timer_create_args_t ip_timer_args = {
        .callback = &ip_timer_callback,
        .name = "ip"};
    esp_timer_create(&ip_timer_args, &ip_timer);

    const esp_timer_create_args_t disconnection_timer_args = {
        .callback = &disconnection_timer_callback,
        .name = "disconnection"};
    esp_timer_create(&disconnection_timer_args, &disconnection_timer);

    ESP_LOGI(TAG, "Wifi Initialized");

    return loop_connect;
}

esp_err_t wifi_connect(void)
{
    vTaskDelay(pdMS_TO_TICKS(ms_connect_delay));
    wifi_state = CONNECTED;
    ESP_LOGI(TAG, "Wifi Connected");
    esp_timer_start_periodic(ip_timer, us_ip_delay);
    esp_timer_start_periodic(disconnection_timer, us_disconnect_delay);
    esp_event_post_to(loop_connect, WIFI_MOCK, WIFI_MOCK_EVENT_WIFI_CONNECTED, NULL, 0, portMAX_DELAY);
    return ESP_OK;
}

esp_err_t wifi_disconnect(void)
{
    wifi_state = DISCONNECTED;
    esp_timer_stop(ip_timer);
    esp_timer_stop(disconnection_timer);
    ESP_LOGI(TAG, "Wifi Disconnected, call wifi_connect() to reconnect");
    return ESP_OK;
}

esp_err_t send_data_wifi(void *data, size_t size)
{
    if (wifi_state != CONNECTED_WITH_IP)
    {
        ESP_LOGI(TAG, "Error sending data, invalid state -> %s", stateNames[wifi_state]);
        return ESP_ERR_INVALID_STATE;
    }
    else
    {
        ESP_LOGI(TAG, "Data '%.6f' sent successfully", *((float *)data));
        return ESP_OK;
    }
}

static void ip_timer_callback(void *arg)
{
    ESP_LOGI(TAG, "Direccion IP obtenida");
    wifi_state = CONNECTED_WITH_IP;
    esp_event_post_to(loop_connect, WIFI_MOCK, WIFI_MOCK_EVENT_WIFI_GOT_IP, NULL, 0, portMAX_DELAY);
    esp_timer_stop(ip_timer);
}

void wifi_getIp()
{
    ESP_LOGI(TAG, "Direccion IP obtenida");
    wifi_state = CONNECTED_WITH_IP;
    esp_event_post_to(loop_connect, WIFI_MOCK, WIFI_MOCK_EVENT_WIFI_GOT_IP, NULL, 0, portMAX_DELAY);
    esp_timer_stop(ip_timer);
}

static void disconnection_timer_callback(void *arg)
{
    ESP_LOGI(TAG, "Wifi Disconnected, call wifi_connect() to reconnect");
    wifi_state = DISCONNECTED;
    esp_event_post_to(loop_connect, WIFI_MOCK, WIFI_MOCK_EVENT_WIFI_DISCONNECTED, NULL, 0, portMAX_DELAY);
    esp_timer_stop(disconnection_timer);
    esp_timer_stop(ip_timer);
}