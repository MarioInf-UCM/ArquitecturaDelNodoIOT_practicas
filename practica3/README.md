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

Ambas funciones difieren únicamente en la posibilidad de indicar el núcleo exclusivo que ejecutará dicha tarea, por lo que podemos utilizarlas de forma indiferente en este ejercicio (más información [aquí](https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/system/freertos.html)). Los parámetros pasados a las mismas son:
 1) Función asociada a la tarea.
 2) Identificador de la tarea.
 3) Tamaño de la pila de la tarea.
 4) Parámetros de entrada de la tarea (los cuales deben ser formateados posteriormente).
 5) La prioridad de la tarea.
 6) La función manejadora del evento de finalización de dicha tarea.
 7) Identificador del núcleo (0 o 1) que se encargará de la ejecución de la tarea (parámetro exclusivo de la función **xTaskCreatePinnedToCore()**).

Una vez creada la tarea secundaria, únicamente tendremos que entrar en un bucle infinito para la lectura de la variable global encargada del almacenamiento de la temperatura.

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
Para poder llevar a cabo la ejecución necesitaremos realizar un montaje de nuestro sistema, para lo cual utilizaremos los siguientes componentes:
 - 1 STM32.
 - 1 Sensor de temperatura y humedad si7021.

 Por otra parte, las conexiones realizadas son las siguientes:
 - Sensor si7021, pin SDA <-(Cable azul)-> STM32, pin GPIO 26
 - Sensor si7021, pin SCL <-(Cable naranja)-> STM32, pin GPIO 27
 - Sensor si7021, pin GND <-(Cable negro)-> STM32, pin GND
 - Sensor si7021, pin VIN <-(Cable verde)-> STM32, pin 5V

En la siguiente imagen podemos ver un ejemplo de montaje correspondiente a las especificaciones anteriores.

<img src="images/montajeEjercicio1.jpeg" alt="drawing" style="width:50%; 
    display: block;
    margin-left: auto;
    margin-right: auto;
    margin-top: 1%;
    margin-botton: 1%;
"/>

Una vez realizado el montaje del sistema, podremos ejecutarlo para obtener la siguiente salida, en la que podremos ver como es la tarea secundaria la que realiza la lectura y como es la principal la que la muestra por pantalla. También podemos observas información adicional como la prioridad de dichas tareas o el tiempo de muestreo configurable por el usuario.

```BASH
I (342) main_task: Calling app_main()
I (342) ej1_sampling: ** Prioridad de la tarea Main: 1 **
I (442) ej1_sampling: Prioridad tarea Secundaria: 5.    Periodo de lectura 2 segundos.
I (2342) ej1_sampling: Prioridad tarea Main: 1 segundos.  Temperatura: 37.202591
I (2542) ej1_sampling: Prioridad tarea Secundaria: 5.    Periodo de lectura 2 segundos.
I (4342) ej1_sampling: Prioridad tarea Main: 1 segundos.  Temperatura: 37.181141
I (4642) ej1_sampling: Prioridad tarea Secundaria: 5.    Periodo de lectura 2 segundos.
I (6342) ej1_sampling: Prioridad tarea Main: 1 segundos.  Temperatura: 37.159691
```

>Cuestión
>
>- ¿Qué prioridad tiene la tarea inicial que ejecuta la función app_main()? ¿Con qué llamada de ESP-IDF podemos conocer la prioridad de una tarea?

En Espressif, cada tarea tiene asociada una prioridad específica, la cual se expresa mediante un número entero entre 1 y 23, de modo que cuanto mayor sea este, mayor prioridad tendrá dicha tarea. Por defecto, Espressif asocia a cada tarea creada una prioridad concreta dependiendo del tipo de la misma. En la siguiente imagen podemos ver un pequeño resumen sobre la prioridad asociada por defecto a las diferentes tareas según el tipo del que sean (más información [aquí](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/speed.html)):

<img src="images/InformacionPrioridadTareas.png" alt="drawing" style="width:50%; 
    display: block;
    margin-left: auto;
    margin-right: auto;
    margin-top: 1%;
    margin-botton: 1%;
"/>

Como podemos ver tanto en la documentación oficial de Espressif como en la salida obtenida del ejercicio, la tarea que ejecuta la función **app_main()** recibe una prioridad 1, es decir, la menor posible. Esto es con el objetivo de que no acabe por dejar en inanición al resto de posibles tareas, tanto creadas por el usuario como generadas automáticamente por el sistema.

Para obtener la prioridad de una determinada tarea utilizamos la función **uxTaskPriorityGet()**, la cual podemos ver como la invocamos dentro de las funciones de cambas tareas y como nos devuelve un valor concreto para cada una. En el siguiente cuadro podemos ver la información sobre dicha función, obtenida directamente de la documentación de FreeRTOS (más información [aquí](https://www.freertos.org/a00128.html)):

```TXT
UBaseType_t uxTaskPriorityGet( TaskHandle_t xTask );

INCLUDE_uxTaskPriorityGet must be defined as 1 for this function to be available. See the RTOS Configuration documentation for more information. Obtain the priority of any task.

Parameters:
xTask 	Handle of the task to be queried. Passing a NULL handle results in the priority of the calling task being returned.

Returns:
The priority of xTask.
```

>Cuestión
>
>- ¿Cómo sincronizas ambas tareas? ¿Cómo sabe la tarea inicial que hay un nuevo dato generado por la tarea muestreadora?

Tal y como se encuentra diseñado el ejercicio actual, no existe ningún punto de sincronización entre ambas tareas, de modo que le permita la tarea principal saber cuando se ha registrado una nueva temperatura por parte de la tarea secundaria. Esto quiere decir que ambas tareas están constantemente escribiendo y leyendo dicha variable sin saber si esta ha sido interaccionada o no por la otra tarea, lo cual es posible gracias a que en Espressif las tareas comparten un mismo ámbito de variables.

Debemos tener en cuenta que debido a que Espressif otorga la ejecución a aquellas tareas que con mayor prioridad, la tarea principal únicamente pasará a estado **running** cuando la tarea que realza la lectura de la temperatura, la cual tiene una mayor prioridad, pase a un estado **suspended** o **blocked**. Esto quiere decir que mientras la tarea secundaria se encuentra escribiendo en la variable global que registra la temperatura, la parea principal encargada de leerla, al tener una menor prioridad, no podrá acceder a la misma.

En el caso de que se quisiera que la tarea principal lea la temperatura únicamente cuando esta ha sido actualizada por la tarea que la mide, deberemos implementar un mecanismo de sincronización como las colas, o en su defecto semáforos.

>Cuestión
>
>- Si además de pasar el período como parámetro, quisiéramos pasar como argumento la dirección en la que la tarea muestreadora debe escribir las lecturas, ¿cómo pasaríamos los dos argumentos a la nueva tarea?

Debemos tener en cuenta que las funciones encargadas de crear tareas reciben un parámetro, el cual pasarán a la función asociada a la ejecución de dicha tarea que van a crear. Este parámetro es del tipo ``(void *)`` loq ue nos quiere decir que le podemos agregar cualquier tipo de dato, con la condición de que para poder utilizarlo lo formateemos previamente.

Dicho esto, una opción viable para pasar más de un argumento como parámetro de entrada a una tarea es la creación de una estructura que podamos formatear una vez se ha invocado a la misma. En el siguiente bloque podemos ver un ejemplo de esto:

```C
struct Data{
    int par1;
    float par2;
    char * par3;
}Data;

void fuctionTask(void *parameters){
    Data data = (struct Data *) parameters;
    printData(data);
}

```