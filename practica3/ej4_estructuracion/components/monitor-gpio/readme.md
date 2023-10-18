# Componente Monitor GPIO - Guía de uso

El presente componente gestiona la interacción del usuario con los diferentes pines GPIO que están involucrados en la aplicación.


<br /> 

## Variables de configuración

 - **NBUTTON:** Define la frecuencia con la que se escanea el estado de los pines GPIO.
    - **Referencia:** SCANNING FREQUENCY.
    - **Valor por defecto:** 1.
    - **Límites del valor:** Indeterminados.
    - **Campo de ayuda:** Scanning frecuency for button.

<br />

 - **BUTTON_GPIO_PORT:** Define la frecuencia con la que se escanea el estado de los pines GPIO.
    - **Referencia:** GPIO INPUT PORT FOR BUTTON.
    - **Valor por defecto:** 25.
    - **Límites del valor:** Indeterminados.
    - **Campo de ayuda:** GPIO input port for button.
    
<br />



## Eventos implementados

 - **MONITOR_GPIO:** Familia de eventos relacionada con la interacción GPIO.
    - **MONITOR_GPIO_BUTTON_PRESSED:** Evento lanzado cuando se pulsa el botón que activa el modo consola.


<br />

## Funciones implementadas
```C
esp_event_loop_handle_t monitor_gpio_init();
```
Inicializa el componente para poder llevar a cabo la interacción con los pines GPIO. Esta función debe ejecutarse antes de empezar a operar con el componente.

**Parámetros:** Ninguno.

**Retorno (esp_event_loop_handle_t):** 
 - Loop de control de eventos utilizado para poder suscribirse a los diferentes eventos emitidos por el componente.



<br />

```C
esp_err_t monitor_gpio_resume();
```
Comienza la detección periódica de la interacción con los pines GPIO.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):**
 - ESP_OK: Finalización con éxito.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.



<br />

```C
esp_err_t monitor_gpio_stop();
```
Finaliza la detección periódica de la interacción con los pines GPIO.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):**
 - ESP_OK: Finalización con éxito.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.




<br />

