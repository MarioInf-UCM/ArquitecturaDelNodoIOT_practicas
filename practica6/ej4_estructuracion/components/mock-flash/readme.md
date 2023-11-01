# Componente mockFLASH - Guía de uso

El presente componente simula el comportamiento de una memoria flash en la que almacenar información.

<br /> 


## Variables de configuración

El presente componente no dispone de variables de configuración definidas.

<br /> 



## Eventos implementados

El presente componente no dispone de eventos definidos.

<br /> 



size_t getDataLeft();



## Funciones definidas
```C
esp_err_t circularBuffer_init(size_t capacity);
```
Inicializa el componente para poder llevar a cabo la simulación WIFI. Esta función debe ejecutarse antes de empezar a operar con el componente.

**Parámetros:** 
 - **size_t capacity:** Define la capacidad con al que se creará la memoria flash.

**Retorno (esp_event_loop_handle_t):** 
 - Loop de control de eventos utilizado para poder suscribirse a los diferentes eventos emitidos por el componente.



<br />

```C
void circularBuffer_destroy();
```
Elimina permanentemente el buffer de la memoria flash.

**Parámetros:** Ninguno.

**Retorno (esp_eer_t):**
 - ESP_OK: Finalización con éxito.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.



<br />

```C
esp_err_t void *readFromFlash(size_t size);
```

Lee el dato situado en la cima de la pila flash.

**Parámetros:**
 - **size_t size:** Tamaño del dato a introducir. El tamaño del dato no debe sobrepasar la capacidad libre de la pila flash.

**Retorno (esp_eer_t):**
 - ESP_OK: Finalización con éxito.
 - ESP_ERR_INVALID_SIZE: El tamaño del elemento a introducir es mayor que el tamaño libre de la pila.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.



<br />

```C
esp_err_t writeToFlash(void *data, size_t size);
```
Escribe en la memoria flash el dato pasado como parámetro.

**Parámetros:** 
 - **void *data:** Dato a introducir en la memoria flash.
 - **size_t size:** Tamaño del dato a introducir. El tamaño del dato no debe sobrepasar la capacidad libre de la pila flash.

**Retorno (esp_eer_t):**
 - ESP_OK: Finalización con éxito.
 - ESP_ERR_INVALID_SIZE: El tamaño del elemento a introducir es mayor que el tamaño libre de la pila.
 - Posibles estados de finalización de error, consultar documentación **esp_err_t**.



<br />

```C
size_t getDataLeft();
```
Comprueba es el espacio libre restante en la pila flash.

**Parámetros:** Ninguno.

**Retorno (size_t):** Tamaño del espacio libre restante en la pila flash.



