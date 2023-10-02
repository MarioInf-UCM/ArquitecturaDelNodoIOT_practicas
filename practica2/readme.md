# Aplicación Consola
>Cuestión
>
>¿Qué componente se está incluyendo además de los que siempre se incluyen por defecto?



>Cuestión
>
>¿Qué funcionalidad se importa de dicho componente?



>Cuestión
>
>¿Qué particiones se crean al volcar el proyecto en nuestro dispositivo?






# Importar código externo como componente (SI7021)
En el proyecto si7021 se ha creado la carpeta components/si7021 junto a los ficheros [si7021.c](si70121/components/si70121/si7021.c) y [si7021.h](si70121/components/si70121/si7021.h). También se ha creado el fichero [CMakeLists.txt](si70121/components/si70121/CMakeLists.txt) con el siguiente contenido:
```BASH
idf_component_register(SRCS "si7021.c"
                    INCLUDE_DIRS "."
                    REQUIRES "driver")
```

Por otro lado en la carpeta main se han incluido los dos ficheros [i2c_config.c](si70121/main/i2c_config.c) e [i2c_config.h](si70121/main/i2c_config.h) del campus.

Dentro del fichero [main.c](si70121/main/main.c) se ha definido el tiempo de espera de la tarea taskShowTemperature que muestra la temperatura cada 2 segundos.

Salida:

```BASH
I (326) app_start: Starting scheduler on CPU0
I (331) app_start: Starting scheduler on CPU1
I (331) main_task: Started on CPU0
I (341) main_task: Calling app_main()
I (341) main_task: Returned from app_main()
Temperatura 24.235947
Temperatura 24.235947
Temperatura 24.268122
```
