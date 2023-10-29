# PRÁCTICA 6 - Modos de bajo consumo


## Ejercicio 1 - Uso básico del modo Light Sleep

>Tareas
>
>Hacer funcionar el ejemplo, permitiendo que volvamos de light-sleep únicamente por untimer o por GPIO.

Para llevar a cabo el presente ejercicio vamos a utilizar como base el ejemplo **system/light_sleep**, el cual implementa la funcionalidad básica para llevar a nuestra placa a modo de ahorra de energía **light_sleep**, además de configurar una serie de fuentes que pueden ser utilizadas para despertar al sistema. En nuestro caso utilizare únicamente dos de ellas: El timer y el GPIO.

El primer paso para poder desarrollar el proyecto será e montaje de los componentes hardware necesarios para su funcionamiento, los cuales vienen especificados en el fichero **README.md** del ejemplo. Sin embargo, estos se pueden resumir en los siguientes:
- Un SoC compatible con el ejemplo.
- Un interruptor, para poder interaccionar con el pin GPIO que despertará la placa.
- Una resistencia de pull-up de 10k ohmios.
- Un capacitador de 100nF para evitar rebotes.

El montaje debe llevarse a cabo conectando el interruptor en serie con la resistencia de pull-up, ambos realizando un enlace entre el pin de 3.3v y el pin GPIO especificado para despertar a la placa. Por otra parte, el capacitador debe conectarse al interruptor y a tierra, de la misma manera que puede verse en al siguiente imagen: 

<img src="images/" alt="drawing" style="width:100%; 
    display: block;
    margin-left: auto;
    margin-right: auto;
    margin-top: 1%;
    margin-botton: 1%;
"/>

Una vez realizado el montaje, vamos a ligeramente el ejemplo para poder determinar como se llevarán a cabo los eventos que despierten la placa. En nuestro caso vamos a especificar que el timer que lleva a cabo el proceso de despertar de forma periódica se lleve a cabo cada 5 segundos, mientras que el pin GPIO utilizado para poder utilizar nuestro interruptor será el número 14 (esto teniendo en cuenta que estamos utilizando una placa STM32 y sobre todo por contar con una mayor comodidad a la hora de ejecutar el ejemplo).

En los siguientes cuadros podemos ver cambas especificaciones mediante el uso de variables globales, las cuales se encuentran en lso ficheros **timer_wakeup.c** y **gpio_wakeup.c** respectivamente:

```C
#define TIMER_WAKEUP_TIME_US    (5 * 1000 * 1000)
```

```C
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32H2 \
    || CONFIG_IDF_TARGET_ESP32C6
#define BOOT_BUTTON_NUM         9
#else
#define BOOT_BUTTON_NUM         14
#endif
```

Una vez hecho esto, estamos listos para comprobar el funcionamiento de nuestro código. De este modo, en el siguiente cuadro podemos ver un ejemplo de ejecución que nos muestra tanto el despertar periódico cada 5 segundos como el uso del pin GPIO mediante el interruptor para poder llevar a cabo el despertar de la placa cuando nosotros queramos.

En el siguiente cuadro podemos ver la salida obtenida tras ejecutar el programa de ejemplo. Debemos fijarnos en como el timer periódico despierta el SoC cada 5 segundos, mientras que en la parte central de la salida podemos apreciar como se ha llevado a cabo el despertar mediante el pulsador y como se rompe la secuencialidad establecida cuando este no es utilizado.

```BASH
I (361) main_task: Calling app_main()
I (361) gpio: GPIO[14]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
Waiting for GPIO14 to go high...
I (371) gpio_wakeup: gpio wakeup source is ready
I (381) timer_wakeup: timer wakeup source is ready
I (381) uart: queue free spaces: 20
I (401) uart_wakeup: uart wakeup source is ready
Entering light sleep
Returned from light sleep, reason: timer, t=5094 ms, slept for 5000 ms
Entering light sleep
Returned from light sleep, reason: timer, t=10103 ms, slept for 5000 ms
Entering light sleep
Returned from light sleep, reason: timer, t=15111 ms, slept for 5000 ms
Entering light sleep
Returned from light sleep, reason: pin, t=13511 ms, slept for 3399 ms
Waiting for GPIO14 to go high...
Entering light sleep
Returned from light sleep, reason: pin, t=13531 ms, slept for 3 ms
Waiting for GPIO14 to go high...
Entering light sleep
Returned from light sleep, reason: pin, t=13552 ms, slept for 2 ms
Waiting for GPIO14 to go high...
Returned from light sleep, reason: timer, t=20120 ms, slept for 5000 ms
Entering light sleep
Returned from light sleep, reason: timer, t=25129 ms, slept for 5000 ms
```

<br />

>Cuestión
>
>¿Qué número de GPIO está configurado por defecto para despertar al sistema? ¿Está conectado dicho GPIO a algún elemento de la placa ESP Devkit-c que estamos usando? Puedes tratar de responder consultando el esquemático de la placa

En nuestro caso hemos llevado a cabo una modificación del pin GPIO utilizado para despertar la placa (sobre todo por comodidad), sin embargo, en el siguiente cuadro podemos ver la configuración establecida por defecto y como esta depende del modelo concreto de placa que estemos utilizando. En nuestro caso, teniendo en cuenta que estamos utilizado el SoC STM32, el pin utilizado por defecto es el número 0.

```C
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32H2 \
    || CONFIG_IDF_TARGET_ESP32C6
#define BOOT_BUTTON_NUM         9
#else
#define BOOT_BUTTON_NUM         0
#endif
```

Teniendo en cuenta esto, en la siguiente imagen obtenida del siguiente [enlace](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html), podemos ver todos los elementos a los que se encuentran conectados todos y cada uno de los pines GPI de los que disponemos en el SoC en dicho SoC:

<img src="images/esp32-devkitC-v4-pinout.png" alt="drawing" style="width:60%; 
    display: block;
    margin-left: auto;
    margin-right: auto;
    margin-top: 1%;
    margin-botton: 1%;
"/>

De esta manera manera podemos ver como el pin GPIO 0 esta conectado a varios elementos de la placa, los cuales a demás son utilizados en el presente ejemplo. Dichos elementos son los siguientes:
- **El convertidor Analógico-Digital 1**.
- **El sensor táctil 1**, el cual es utilizado para poder despertar la placa mediante un panel táctil (según se indica en la memoria del ejemplo).
- **El Control de BOOT**.


<br />

>Cuestión
>
>¿Qué flanco provocará que salgamos de light-sleep tras configurar el GPIO con `gpio_wakeup_enable(GPIO_WAKEUP_NUM, GPIO_WAKEUP_LEVEL == 0 ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL)` ?

Para poder responder a esta pregunta primero miraremos los manuales de Espressif referentes a la configuración de los pines GPIO RTC, al cual podemos acceder en el siguiente (enlace)[https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html]. En la siguientes imágenes podemos ver tanto la descripción de la función mencionada en el enunciado como de las dos macros utilizadas para definir el nivel de tensión en el que se lanzará el proceso para despertar la placa.

<img src="images/function_GPIOwakeupEnable.png" alt="drawing" style="width:60%; 
    display: block;
    margin-left: auto;
    margin-right: auto;
    margin-top: 1%;
    margin-botton: 1%;
"/>

<img src="images/functionGPIP_lowAndHighLevel.png" alt="drawing" style="width:60%; 
    display: block;
    margin-left: auto;
    margin-right: auto;
    margin-top: 1%;
    margin-botton: 1%;
"/>

Visto que el segundo argumento es aquel que define el tipo de tensión en el que se ejecutará ducho proceso, y que este se encuentra especificado según la variable `GPIO_WAKEUP_LEVEL`, podemos determinar en base a su valor original, el cual se puede ver en el siguiente cuadro, que el proceso de salida del light-sleep mediante GPIO se produce por el flanco de bajada.

```C
/* Use boot button as gpio input */
#define GPIO_WAKEUP_NUM         BOOT_BUTTON_NUM
/* "Boot" button is active low */
#define GPIO_WAKEUP_LEVEL       1
```


<br />

## Ejercicio 2 - Uso de timers en el modo light Sleep

>Tareas
>
>Incluir un timer en el código. La aplicación arrancará, configurará un timer para que se ejecute su callback cada 0.5 segundos, y se dormirá durante 3 segundos (con vTaskDelay() ). Tras despertar del delay, pasará a light-sleep (configuraremos el mecanismo de despertar para que lo haga en 5 segundos, si no usamos el GPIO correspondiente). El callback del timer simplemente imprimirá un mensaje que incluirá el valor devuelto por esp_timer_get_time() 

En el presente ejercicio modificaremos el código de ejemplo para que se lleven a cabo los siguientes pasos:
 1) Realizar una configuración inicial donde se registren los métodos para despertar al SoC y el timer periódico que imprimirá el tiempo.
 2) Llevar a cabo un periodo donde el SoC se encuentre dormido por 3 segundos.
 3) Entrar en el modo Deep Sleep.
 4) Salir del modo Deep Sleep debido al timer configurado para despertar al SoC o la pulsación del botón mediante GPIO.
 5) Finalizamos la ejecución.

Debemos tener en cuenta que en este caso el timer encargado de imprimir el tiempo no debe estar configurado para despertar al SoC, por lo que deben convivir dos timers distintos en la aplicación.

Teniendo en cuenta lo dicho anteriormente, tanto el primer como el segundo paso se realizarán dentro de la función ejecutada por la tarea que lleva a cabo la entrada en el modo Light Sleep, antes de que este se lleva a cabo. Para ello necesitaremos crear el timer y asociarlo a la función encargada de imprimir el mensaje de manera periódica. En el siguiente cuadro podremos ver dicha configuración:


```C
#define TIME_TO_SLEEP_AFTER_INITIAL_CONFIGURATION 3000
#define TIMER_PRINTTIME_PERIOD 500000


void timerHandler_printTime(){
    printf("Timer de ejecución periódica. Tiempo: %lld\n", esp_timer_get_time());
    return;
}


static void light_sleep_task(void *args){

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &timerHandler_printTime,
        .name = "timer_printTime"};
    esp_timer_handle_t periodic_timer;
    esp_timer_create(&periodic_timer_args, &periodic_timer);
    esp_timer_start_periodic(periodic_timer, TIMER_PRINTTIME_PERIOD);


    printf("Comenzamos a dormir después de la configuración inicial\n");
    vTaskDelay( TIME_TO_SLEEP_AFTER_INITIAL_CONFIGURATION / portTICK_PERIOD_MS);
    printf("Finalizamos de dormir después de la configuración inicial\n");  
```

Para la correcta visualización del ejercicio, hemos decidido que no se vuelva a entrar en el modo Light Sleep una vez se ha salido del mismo. Para esto deberemos romper el bucle en el que se encuentra la tarea y eliminar tanto el timer creado para la impresión de los mensajes como todos aquellos recursos establecidos para despertar el sistema. En el siguiente cuadro podemos ver dicha configuración situada al final de la función `light_sleep_task()`:

```C
    printf("Finalizando ejecución.\n");
    esp_timer_stop(periodic_timer);
    esp_timer_delete(periodic_timer);
    printf("Timer periódico eliminado.\n");
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    printf("Fuentes para despertar el SoC eliminadas.\n");
    printf("Programa finalizado con éxito.\n");
    
    vTaskDelete(NULL);
}
```

Una vez hecho esto y configurado el timer que despierta al SoC cada 5 segundos, ejecutamos el ejemplo y obtenemos la siguiente salida, donde podemos ver la ejecución periódica del timer mediante los mensajes que este imprime por pantalla:

```BASH
I (361) main_task: Calling app_main()
I (361) gpio: GPIO[14]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
Waiting for GPIO14 to go high...
I (371) gpio_wakeup: gpio wakeup source is ready
I (381) timer_wakeup: timer wakeup source is ready
I (381) uart: queue free spaces: 20
I (401) uart_wakeup: uart wakeup source is ready
Comenzamos a dormir descpués de la configuración inicial
I (401) main_task: Returned from app_main()
Timer de ejecución periodica. Tiempo: 579867
Timer de ejecución periodica. Tiempo: 1079831
Timer de ejecución periodica. Tiempo: 1579831
Timer de ejecución periodica. Tiempo: 2079831
Timer de ejecución periodica. Tiempo: 2579831
Finalizamos de dormir despues de la configuración inicial
Timer de ejecución periodica. Tiempo: 3079831
Timer de ejecución periodica. Tiempo: 8086219
Timer de ejecución periodica. Tiempo: 8086425
Timer de ejecución periodica. Tiempo: 8086610
Timer de ejecución periodica. Tiempo: 8087699
Timer de ejecución periodica. Tiempo: 8091879
Timer de ejecución periodica. Tiempo: 8096046
Timer de ejecución periodica. Tiempo: 8100212
Timer de ejecución periodica. Tiempo: 8104379
Timer de ejecución periodica. Tiempo: 8108545
Timer de ejecución periodica. Tiempo: 8112712
Returned from light sleep, reason: timer, t=8091 ms, slept for 5004 ms
Finalizando ejecución.
Timer periódico eliminado.
Fuentes para despertar el SoC eliminadas.
Programa finalizado con exito.
```