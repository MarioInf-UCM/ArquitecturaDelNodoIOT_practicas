# PRÁCTICA 4 - Uso del bus I2C y del sensor de temperatura

>Cuestión
>
>La dirección del sensor es 1000000 (es decir, 0x40 expresado en hexadecimal). Si queremos hacer una operación de lectura (bit R/W a 1), ¿cómo construiremos el segundo argumento de la llamada a i2c_master_write_byte() que haremos tras i2c_master_start() ?

La función `i2c_master_write_byte()` únicamente puede utilizarse cuando el dispositivo se encuentra configurado en modo **master** y su función es introducir en la cola de mensajes I2C un único byte de escritura (más información [aquí](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html)). La función recibe tres parámetros, los cuales son relevantes de analizar:
 - 1) La lista de comandos I2c donde se introducirán los datos.
 - 2) Información que se introducirá en la lista especifica en la lista de comandos I2C.
 - 3) Flag para activar la señal ACK.

Una vez visto esto, sabemos que el tercer parámetro es constante y el primero depende de la cola I2C que estemos utilizando, por lo que es más relevante es el segundo. Teniendo en cuenta que se trata de la primera escritura, necesitaremos establecer el modo de operación, de modo que el contenido de este será diferente dependiendo de si lo que queremos hacer es leer desde o escribir hacia el esclavo, pero en ambos casos, dicho byte de información estará compuesto de la siguiente manera:
 - Los primero 7 bits harán referencia a la dirección del esclavo al cual se dirige la escritura/lectura.
 - El último bit dependerá del tipo de operación a realizar.

En el caso de Espressif, ya se encuentran definidas un par de macros para determinar si se produce una lectura o escritura, siendo estas **I2C_MASTER_READ** e **I2C_MASTER_WRITE** respectivamente. De esta manera, teniendo en cuenta que la dirección del dispositivo es de 7 bits, únicamente necesitaremos realizar un desplazamiento lateral de 1 bit sobre la variable que contiene dirección y operar mediante una puerta OR entre esta y la macro deseada.

En el siguiente cuadro tenemos un ejemplo sobre como sería la ejecución de dicha función para establecer una operación de lectura sobre el esclavo.

```C
i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | I2C_MASTER_READ, ACK_EN);
```



## Ejercicio obligatorio - Uso de I2Ctools

>Ejercicio obligatorio
>
>Compila y prueba el ejemplo i2c_tools de la carpeta de ejemplos (examples/peripherals/i2c/i2c_tools). Conecta el sensor a los pines indicados por defecto (también a Vcc y a tierra) y ejecuta al comando i2cdetect. Prueba a los distintos comandos disponibles para tratar de leer información del sensor.

Para realizar el presente ejercicio, primero necesitaremos conectar el sensor **si7021** a los pines configurados por defecto dependiendo del SoC que estemos utilizando. Para esto podemos visualizar el fichero **README.md** del ejemplo, donde se nos indica cuales son las conexiones que deberemso llevar a cabo. En el siguiente cuadro podremos ver la configuración indicada:

```md
#### Pin Assignment:

**Note:** The following pin assignments are used by default, you can change them with `i2cconfig` command at any time.

|                     | SDA    | SCL    | GND  | Other | VCC  |
| ------------------- | ------ | ------ | ---- | ----- | ---- |
| ESP32 I2C Master    | GPIO18 | GPIO19 | GND  | GND   | 3.3V |
| ESP32-S2 I2C Master | GPIO18 | GPIO19 | GND  | GND   | 3.3V |
| ESP32-S3 I2C Master | GPIO1  | GPIO2  | GND  | GND   | 3.3V |
| ESP32-C3 I2C Master | GPIO5  | GPIO6  | GND  | GND   | 3.3V |
| ESP32-C2 I2C Master | GPIO5  | GPIO6  | GND  | GND   | 3.3V |
| ESP32-H2 I2C Master | GPIO1  | GPIO2  | GND  | GND   | 3.3V |
| Sensor              | SDA    | SCL    | GND  | WAK   | VCC  |

**Note:** It is recommended to add external pull-up resistors for SDA/SCL pins to make the communication more stable, though the driver will enable internal pull-up resistors.
```

Una vez ejecutado el ejemplo, podremos ver un menú de opciones que nos muestras los diferentes comandos a poder utilizar. En nuestro caso utilizaremos la instrucción `i2cdetect`, cuyo propósito es escanear el bus I2C en busca de posibles dispositivos conectados al mismo. 

Una vez hecho esto, obtenemos la siguiente salida en forma de cuadro, la cual nos indica que detecta un total de dos dispositivos, uno situado en la dirección 0x00 y otro en la dirección 0x40. Este último se trata de nuestro sensor de temperatura y humedad.

```BASH
i2c-tools> i2cdetect
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: 00 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: 40 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
```