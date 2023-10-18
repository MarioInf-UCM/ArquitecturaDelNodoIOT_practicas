# Componente mockWIFI - Guía de uso

El presente componente simula el comportamiento de una conexión a la red WIFI como Punto de Acceso. 


<br /> 

## Variables de configuración

 - **CONNECT_DELAY:** Define el tiempo que transcurre entre que comienza a realizarse la conexión WIFI y esta finaliza con éxito.
    - **Referencia:** CONNECT DELAY.
    - **Valor por defecto:** 5.
    - **Límites del valor:** Indeterminados.
    - **Campo de ayuda:** Delay seconds between wifi_connect() call and sending event back.

<br />

 - **IP_DELAY:** Define el tiempo que transcurre entre que se realiza con éxito la conexión WIFI y se obtienen los parámetros de configuración IP.
    - **Referencia:** IP_DELAY.
    - **Valor por defecto:** 3.
    - **Límites del valor:** Indeterminados.
    - **Campo de ayuda:** Delay seconds between WIFI_MOCK_EVENT_WIFI_CONNECTED and WIFI_MOCK_EVENT_WIFI_GOT_IP events

<br />

 - **DISCONNECT_DELAY:** Define el tiempo que transcurre entre desconexiones.
    - **Referencia:** DISCONNECT_DELAY (s).
    - **Valor por defecto:** 15.
    - **Límites del valor:** Indeterminados.
    - **Campo de ayuda:** Delay seconds between WIFI_MOCK_EVENT_WIFI_CONNECTED and WIFI_MOCK_EVENT_WIFI_DISCONNECTED events

<br />



## Eventos implementados

- **ESP_EVENT_DECLARE_BASE:** Familia de eventos que gestionan las acciones llevadas a cabo en la simulación WIFI.
    - **WIFI_MOCK_EVENT_WIFI_CONNECTED:** Evento lanzado cuando se realiza la conexión de manera exitosa con el Punto de Acceso.
    - **WIFI_MOCK_EVENT_WIFI_GOT_IP:** Evento lanzado cuando el sistema a obtenido los parámetros de configuración IP del Punto de Acceso.
    - **WIFI_MOCK_EVENT_WIFI_DISCONNECTED:** Evento lanzado cuando se lleva a cabo la desconexión Punto de Acceso.

<br /> 



## Funciones definidas
```C
esp_event_loop_handle_t wifi_mock_init(void);
```
Inicializa el componente para poder llevar a cabo la simulación WIFI. Esta función debe ejecutarse antes de empezar a operar con el componente.

**Parámetros:** Ninguno.

**Retorno (esp_event_loop_handle_t):** 
 - Loop de control de eventos utilizado para poder suscribirse a los diferentes eventos emitidos por el componente.


<br />

```C
esp_err_t wifi_connect(void);
```
Realiza la simulación de conexión con el Punto de Acceso.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):**
 - ESP_OK: Finalización con éxito.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.


<br />

```C
esp_err_t wifi_disconnect(void);
```
Realiza la simulación de desconexión con el Punto de Acceso.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):**
 - ESP_OK: Finalización con éxito.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.



<br />

```C
esp_err_t send_data_wifi(void *data, size_t size);
```

Realiza el envío de información al Punto de Acceso. Para esto es necesario que primero se hayan realizado la conexión con el Punto de Acceso y obtenido los parámetros de configuración IP.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):**
 - ESP_OK: Finalización con éxito.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.