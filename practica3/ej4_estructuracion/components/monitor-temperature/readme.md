# Componente TemperatureMonitor - Guía de uso

El componente **TemperatureMonitor** implementa la funcionalidad necesaria para realizar lecturas de temperatura y humedad, empleando para ello el sensor **Si7021**. 

## Eventos implementados

- **TEMPERATURE_MONITOR_EVENTS:** Familia de eventos relacionados con las lecturas de temperatura y humedad:
    - **TEMPERATURE_READED_EVENT:** Evento lanzado tras realizar una la lectura de la temperatura.
    - **HUMIDITY_READED_EVENT:** Evento lanzado tras realizar una la lectura de la humedad.

## Funciones implementadas

```C
esp_event_loop_handle_t TemperatureMonitor_init();
```
Inicializa el componente para poder llevar a cabo las lecturas y recibir los eventos de las mismas. Esta función debe ejecutarse antes de empezar a operar con el componente.

**Parámetros:** Ninguno.

**Retorno (esp_event_loop_handle_t):** 
 - Loop de control de eventos utilizado para poder suscribirse a los diferentes eventos emitidos por el componente.



<br />

```C
esp_eer_t TemperatureMonitor_start();
```
Comienza la realización de las lecturas periódicas de temperatura y humedad.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):**
 - ESP_OK: Finalización con éxito.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.


<br />

```C
esp_err_t TemperatureMonitor_readTemperature();
```
Realiza una la lectura de temperatura.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):** 
 - ESP_OK: Finalización con éxito.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.


<br />

```C
esp_err_t TemperatureMonitor_readHumidity();
```
Realiza una la lectura de humedad.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):** 
 - ESP_OK: Finalización con éxito.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.



<br />

```C
esp_err_t TemperatureMonitor_stop();
```
Finaliza la realización de las lecturas periódicas de temperatura y humedad.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):**
 - ESP_OK: Finalización con éxito.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.