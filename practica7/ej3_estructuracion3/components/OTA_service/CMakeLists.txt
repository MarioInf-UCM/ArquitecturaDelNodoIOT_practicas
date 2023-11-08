set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

idf_component_register( SRCS "OTA_service.c" 
                            "ble_helper/bluedroid_gatts.c" "ble_helper/nimble_gatts.c" "ble_helper/ble_api.c"
                        INCLUDE_DIRS "." "./ble_helper/include/"
                        REQUIRES "driver" "esp_event" "esp_timer" "nvs_flash" "app_update" "esp_http_client" "esp_https_ota" "esp_netif" "protocol_examples_common" "esp_wifi"
                        EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem
                        )

