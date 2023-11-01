# Componente Consola - Guía de uso

El presente componente gestiona el comportamiento de la consola de comandos.

<br /> 

## Variables de configuración

 - **CONSOLE_MAX_COMMAND_LINE_LENGTH:** Tamaño máximo de entrada en la línea de comandos.
    - **Referencia:** Maximum command line length.
    - **Valor por defecto:** 1024.
    - **Límites del valor:** Indeterminados.
    - **Campo de ayuda:** This value marks the maximum length of a single command line. Once it is reached, no more characters will be accepted by the console.

<br />



## Eventos implementados

- **ANIOT_CONSOLE_EVENT:** Familia de eventos que gestionan las acciones llevadas a cabo por la consola.
    - **CMD_MONITOR_EVENT:** Evento emitido para volver al modo de monitorización.

<br /> 



## Funciones definidas
```C
esp_event_loop_handle_t aniot_console_init();
```
Inicializa el componente para poder llevar a cabo la interacción con la consola. Esta función debe ejecutarse antes de empezar a operar con el componente.

**Parámetros:** Ninguno.

**Retorno (esp_event_loop_handle_t):** 
 - Loop de control de eventos utilizado para poder suscribirse a los diferentes eventos emitidos por el componente.



<br />

```C
esp_err_t aniot_console_start();
```
Comienza la interacción con la con la consola de comandos.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):** Void


<br />

```C
void aniot_console_stop();
```
Finaliza la interacción con la con la consola de comandos.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):** Void.




<br />