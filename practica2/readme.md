# Aplicación Consola
>Cuestión
>
>¿Qué componente se está incluyendo además de los que siempre se incluyen por defecto?

Los componentes osn aquellas unidades básicas que compoenne los proyectos de **Espressif**, y estos deben contener todos un fichero **CMakeList.txt**. Cuando creamos un proyecto desde cero, el mismo ya contiene su propio fichero **CMakeList.txt**, el cual es utilizado apra establecer alguans configuraciones relacionadas con al compilación y ejecución.

Si analizamos el fichero **CMakelists.txt** que esta contenido en el directorio raíz del ejemplo **basic**, podremos ver el siguiente contenido:

```C
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/system/console/advanced/components)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(console)
```

En la segunda línea del fichero se está introduciendo una dirección local donde se encuentran unos componentes a importar. Debemos de tener en cuenta que la dirección de los componetes que debe importar el proyecto viene dada por dos variables:
 - **COMPONENT_DIRS:** Variable que referencia los directorios en los cuales el proyecto buscará los compoenntes predeterminados.
 - **EXTRA_COMPONENT_DIRS:** Variable en la cual podremos incluir directorios adicionales donde el proyecto debe buscar componentes a la hora de compolar el proyecto.

Si en el caso de observar la dirección especificada, podremos ver el siguiente resultado:

```BASH
debian12:~/esp/esp-idf/examples/system/console/advanced/components$ ls -l
total 12
drwxr-xr-x 2 mario mario 4096 sep 18 12:24 cmd_nvs
drwxr-xr-x 2 mario mario 4096 sep 18 12:24 cmd_system
drwxr-xr-x 2 mario mario 4096 sep 18 12:24 cmd_wifi
debian12:~/esp/esp-idf/examples/system/console/advanced/components$ ls cmd_nvs/
CMakeLists.txt  cmd_nvs.c  cmd_nvs.h
debian12:~/esp/esp-idf/examples/system/console/advanced/components$ ls cmd_system/
CMakeLists.txt  cmd_system.c  cmd_system_common.c  cmd_system.h  cmd_system_sleep.c
debian12:~/esp/esp-idf/examples/system/console/advanced/components$ ls cmd_wifi/
CMakeLists.txt  cmd_wifi.c  cmd_wifi.h
```

En el salida observamos como hay tres componentes denominados **cmd_nvs**, **cmd_system** y **cmd_wifi**, cada uno de los cuales contiene su propio fichero **CMakeLists.tst**. Además de esto, podemos ver como en el inicio de la aplciación se encuentran importados dichos componentes:

```C
#include "cmd_system.h"
#include "cmd_wifi.h"
#include "cmd_nvs.h"
```

De este modo podemos llegar a la conclusión de que el proyecto esta importando los tes componentes externos indicados.


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
