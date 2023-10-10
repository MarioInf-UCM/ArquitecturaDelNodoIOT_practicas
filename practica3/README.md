# Ejercicios básicos

## Ejercicio 1 - Creación de una tarea para realizar un muestreo

>Tarea
>
>La tarea creada leerá el valor del sensor con un período que se pasará como argumento a la tarea. La tarea inicial recogerá el valor muestreado y lo mostrará por puerto serie.

Para poder llevar a cabo el siguiente ejercicio, el primer paso será definir las variables que necesitamos utilizar mediante el menú de configuración. En este caso únicamente necesitaremos definir dos: 
 - El intervalo de muestreo para llevar a cabo la medición. 
 - La prioridad con la que crearemos la tarea que realice la medición del sensor.

En el siguiente cuadro podemos ver la definición de las variables en el menú de configuración:

```C
menu "Example Configuration"

    config READ_PERIOD
        int "Periodo de lectura"
        range 1 60
        default 2
        help
            Define el periodo de lectura, medido en segundos, con el que se obtienen datos desde el sensor Si7021.

    config TASK_PRIORITY
        int "Prioridad de la tarea encargada de realizar el muestreo"
        range 1 22
        default 5
        help
            Define la prioridad de la tarea que lleva a cabo el muestreo del sensor Si7021.

endmenu
```
>Cuestión>Cuestión>Cuestión>Cuestión
Una vez definidas las variables de configuración crearemos la función main, la cual será ejecutada por la tarea principal y cuya finalidad es la de mostrar el resultado de la medición (que se llevará a cabo en la tarea secundaria) mediante el uso de una variable global. Para esto necesitaremos crear primero la tarea secundaria mediante la función **xTaskCreate()** o **xTaskCreatePinnedToCore()**.

Ambas funciones difieren únicamente en la posibilidad de indicar el núcleo exclusivo que ejecutará dicha tarea, por loq ue podemos utilizarlas de forma indiferente en este ejercicio. Los parámetros pasados a las mismas son:
 1) Función asociada a la tarea.
 2) Identificador de la tarea.
 3) Tamaño de la pila de la tarea.
 4) Parámetros de entrada de la tarea (los cuales deben ser formateados posteriormente).
 5) La prioridad de la tarea.
 6) La función manejadora del evento de finalización de dicha tarea.
 7) Identificador del núcleo (0 o 1) que se encargará de la ejecución de la tarea (parámetro exclusivo de la función **xTaskCreatePinnedToCore()**).

 Una vez creada la tarea secundaria, únicamente tendremos que entarr en un bucle infinito para la lectura de la variable global encargada del almacenamiento de la temperatura.

```C
static const char *TAG = "ej1_sampling";
static float temperature = 0;

void app_main(void)
{
    
    i2c_master_init();
    ESP_LOGI(TAG, "** Prioridad de la tarea Main: %d **", uxTaskPriorityGet(NULL));
    xTaskCreatePinnedToCore(&taskFunction, "TareaMuestreo", 3072, (void *) READ_PERIOD, TASK_PRIORITY, NULL, 0);

    while(1){
        vTaskDelay(READ_PERIOD*1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Prioridad tarea Main: %d segundos.  Temperatura: %f", uxTaskPriorityGet(NULL), temperature);
    }
    vTaskDelete(NULL);
}
```

Por otra parte, también necesitaremos definir la función ejecutada por la tarea secundaria encargada de la medición de temperatura mediante el sensor **si7021**. Esta función unicamente necesitará entrar en un bucle infinito donde medir la temperatura del sensor mediante la llamada a al función **readTemperature()**. En el siguiente cuadro podemos ver el contenido de dicha función.

```C
void taskFunction(void *parameters){

    int period = (int) parameters;
    while(1){
        readTemperature(I2C_MASTER_NUM, &temperature);
        ESP_LOGI(TAG, "Prioridad tarea Secundaria: %d.    Periodo de lectura %d segundos.", uxTaskPriorityGet(NULL), period);
        vTaskDelay(period*1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
```


>Cuestión
>
>- ¿Qué prioridad tiene la tarea inicial que ejecuta la función app_main()? ¿Con qué llamada de ESP-IDF podemos conocer la prioridad de una tarea?

>Cuestión
>
>- ¿Cómo sincronizas ambas tareas?¿Cómo sabe la tarea inicial que hay un nuevo dato generado por la tarea muestreadora?

>Cuestión
>
>- Si además de pasar el período como parámetro, quisiéramos pasar como argumento la dirección en la que la tarea muestreadora debe escribir las lecturas, ¿cómo pasaríamos los dos argumentos a la nueva tarea?


ds