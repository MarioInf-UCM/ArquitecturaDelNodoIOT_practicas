# PRÁCTICA 7 - Over The Air Updates


<br />

>Cuestión
>
>¿Qué entradas tiene la tabla de particiones usada?


Para poder responder a la presente pregunta deberemos examinar la configuración establecida en el proyecto desde el Menú de configuración, donde deberemos fijarnos en la opción **CONFIG_PARTITION_TABLE_TYPE**, la cual especifica la tabla de particiones que utilizará el programa (más información [aquí](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html)).Aunque se pueden especificar diferentes configuraciones para la tabla de particiones mediante un archivo CSV, la opción indicada únicamente nos permite seleccionar una de entre dos opciones:

- Tabla básicas con una partición Factory pero sin particiones OTA.
- Tabla básica con una partición Factory y dos particiones OTA.

En el caso del ejemplo tratado, si observamos la opción indicada en el menú de configuración podremos ver que se ha seleccionado la segunda opción, tal y como nos indica la siguiente imagen:

<img src="images/menuconfig_PARTITION_TABLE.png" alt="drawing" style="width:60%; 
    display: block;
    margin-left: auto;
    margin-right: auto;
    margin-top: 1%;
    margin-botton: 1%;
"/>

Teniendo esto en cuenta y si recurrimos ala documentación referente a las **Tablas de particiones** (la cual podemos ver [aquí](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html)), observaremos que los diferentes campos que componen la tabla de particiones los que dispone cualquier programa con dicha opción de inicio son los siguientes:

<img src="images/defaultPartitionTable.png" alt="drawing" style="width:60%; 
    display: block;
    margin-left: auto;
    margin-right: auto;
    margin-top: 1%;
    margin-botton: 1%;
"/>

Además de esto, para aumentar la información mostrada por la aplicación sobre las particiones y sus códigos SHA, se ha complementado la función ``get_sha256_of_partitions()`` con la impresión de los diferentes campos que componen dichas particiones y el número de particiones OTA. A continuación podemos ver un ejemplo más detallado de la salida obtenida para dicha información:

```BASH
I (662) simple_ota_example: SHA-256 for bootloader:  71b74262f8090e15c20845bb690b6c2f175f040ebb918d746beb18ec1b3045e7
I (672) simple_ota_example: Bootloader firmware fields..:
I (672) simple_ota_example: Type: 0
I (682) simple_ota_example: Address: 4096
I (682) simple_ota_example: Size: 32768

I (912) simple_ota_example: SHA-256 for current firmware:  e099cfd06efe3ce5bf7d7b96323671e40e067226cedf2570329323e82b698079
I (912) simple_ota_example: Current firmware fields..:
I (922) simple_ota_example: Label: factory
I (932) simple_ota_example: Type: 0
I (932) simple_ota_example: Subtype: 0
I (932) simple_ota_example: Address: 65536
I (942) simple_ota_example: Size: 1048576
I (942) simple_ota_example: Encrypted?: False

I (952) simple_ota_example: Número de particiones OTA..: 2
```



<br />

>Cuestión
>
>¿Cómo se llama el fichero de certificado que se incluirá en el binario?

La aplicación utiliza un fichero de certificación a la hora de realizar la conexión HTTPS con el objetivo de que esta pueda llevarse a cabo de forma segura y obtener así la nueva versión del firware a instalar en el SoC. Sin embargo, para poder utilizarse dicho certificado, este debe ser incluido dentro de los fichero de texto que componen el componente de la aplicación que lo utiliza. De esta manera, para poder ver que certificado estas utilizado podemos examinar el fichero **CMakeList.txt** del componente main, cuyo interior contiene:

```txt
# Embed the server root certificate into the final binary
idf_build_get_property(project_dir PROJECT_DIR)
idf_component_register(SRCS "simple_ota_example.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)

```

El argumento utilizado para hacer referencia al fichero del certificado es **EMBED_TXTFILES**, el cual sirve para indicar aquellos ficheros que contienen información en cadenas de caracteres, que una vez incrustados dentro de nuestro programa, no queremos que su contenido sea modificado al realizar la compilación. Por otra parte, tal y como podemos ver en el cuadro anterior, el fichero de certificado empleado para realizar la conexión mediante HTTPS se encuentra en la dirección **server_certs/ca_cert.pem**, cuyo contenido es el siguiente:

```TXT
-----BEGIN CERTIFICATE-----
MIICrDCCAZQCCQCH33Z6fQORzTANBgkqhkiG9w0BAQsFADAYMRYwFAYDVQQDDA0x
OTIuMTY4LjguMTIwMB4XDTIyMTExOTExMTMxMloXDTIzMTExOTExMTMxMlowGDEW
MBQGA1UEAwwNMTkyLjE2OC44LjEyMDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC
AQoCggEBANnQpa7Ae64ZnHYP+fH8MSNjxWVxXgPnz2YTOUrrCIlf+6Z7cDN8lccG
VLdB4f6rFKTOzHOeeB4awt2JNt3GmThVUUQnUMv9OiiYgjLY9mqCECV6ehem09hs
H2df4m+OIDWNt697UtDibRdzNCZB+YhcbAK2L8QPLi8LW00jeBndtgRUP72ml+Aj
//edWLkSsh743DHq1h8kvoB+/a4a0vCq37cg48LaV5a0TxYPq5rn/SnIhWq/hjBF
Zbkx3sYFp5+LY4j9mH2jE8m2frvGhbOrztmZA0A0cqZtIbTXMyqjXAb3pIvA/LPB
rj/KYAK7SupJV8z6uYGQUf6qaH+l5D0CAwEAATANBgkqhkiG9w0BAQsFAAOCAQEA
ff+8fk45nUIoRglxsLftkUEwvuLo2FouOIqCZtqLqVe3VKJnf1FhLNWl/WJF4hl2
VL9JQgDyWIUXaCyQB7ZjVzcbwh8iEo9yS9VdntHZSV0/AaYqAZX9MXFyZ4zk/4bS
I5d6IbJy2mQhaH/GRAg3flEqXHa+kQFYe7tcTmTy1QQ7iYhX2GhO5gba/X0p7qFx
yTY/owggA+FvUIBKpMRErgDKt2tYb2fCWae0bWaKY+qzWBpUTyRLCNzehNp/uHxn
irxDALfzRIYrtDhTrrcMP6nXMO8ywnogk6jJIe0S7piy1iLqlEjNiNw3QF9o0KdX
47COkPGwZLGjiGPLZbQArw==
-----END CERTIFICATE-----
```



<br />

>Cuestión
>
>¿Cómo y dónde se indica que se debe incluir el certificado?

Para responder a la pregunta vamos a examinar la estructura de configuración utilizada a la hora de llevar a cabo la conexión HTTPS con el servidor que nos proveerá de la nueva imagen del firware. En el siguiente cuadro podemos ver la formación de dicha estructura y como se especifica el uso de un certificado mediante la línea `.cert_pem = (char *)server_cert_pem_start, `, lo cual hace obligatorio el uso de uno a la hora de realizar la conexión con el servidor.

```C
esp_http_client_config_t config = {
    .url = CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL,
#ifdef CONFIG_EXAMPLE_USE_CERT_BUNDLE
    .crt_bundle_attach = esp_crt_bundle_attach,
#else
    .cert_pem = (char *)server_cert_pem_start,
#endif
    .event_handler = _http_event_handler,
    .keep_alive_enable = true,
#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_BIND_IF
    .if_name = &ifr,
#endif
};
```

Posteriormente, en la última parte de la ejecución, podemos ver como se lleva a cabo la conexión con el servidor utilizando la estructura de configuración antes mencionada, para la cual será necesario dicho certificado.

```C
esp_https_ota_config_t ota_config = {
    .http_config = &config,
};
ESP_LOGI(TAG, "Attempting to download update from %s", config.url);
esp_err_t ret = esp_https_ota(&ota_config);
if (ret == ESP_OK) {
    ESP_LOGI(TAG, "OTA Succeed, Rebooting...");
    esp_restart();
} else {
    ESP_LOGE(TAG, "Firmware upgrade failed");
}
```



<br />

>Cuestión
>
>¿Qué es el símbolo server_cert_pem_start?

La variable `server_cert_pem_start` es utilizado en la estructura de configuración para la conexión mediante HTTPS para especificar el archivo donde se encuentra el certificado a emplear en la comunicación. Además de esto, dicha variable también se utiliza anteriormente en el código, concretamente en la línea donde se declara y a la cual se le asigna un valor a priori extraño. En el siguiente cuadro podemos ver dicha asignación e investigaremos para averiguar que se esta haciendo realmente.

```C
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");
```

Para responder a esta pregunta debemos saber que los ficheros externos que queremos que sean incrustados en la aplicación pero que no deseamos que sean modificados a la hora de realizar la compilación deben ser especificados mediante las variables **EMBED_FILES** o **EMBED_TXTFILES** mediante el fichero **CMakeList.xt** (de manera similar a lo visto en la segunda cuestión).

El contenido de los ficheros especificados de la forma indicada será incluido en la sección **.roadata** de la memoria flash, siendo accesibles a través de unos símbolos concretos, los cuales son formados a través de los nombres de dichos archivos, sustituyendo los caracteres especiales por como "\", "." o "," por "_" e incluyendo siempre el prefijo "_binary". Además de esto, mediante los sufijos "_start" y "_end" acceder al comienzo y al final de dicha variable.

De esta manera, si queremos poder utilizar nuestro certificado, deberemos utilizar la referencia que apunta inicio de su contenido una vez ha sido incrustado en la aplicación. En nuestro caso, si el fichero es **ca_cert.pem**, el símbolo que necesitaremos utilizar será **_binary_ca_cert_pem_start**, el cual es el mismo que asignamos a la variable **server_cert_pem_start**, utilizada para especificar el certificado empleado en la conexión HTTPS.

El único elemento que aún no hemos explicado es el operador **asm**, cuya función se basa en ensamblar dentro del primer argumento el contenido referenciado en memoria flash por el símbolo indicado como segundo argumento. Para ver más información sobre la incrustación de archivos binarios en la aplicación, se puede acceder al siguiente [enlace](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html#embedding-binary-data).

En conclusión, una vez visto lo anterior podemos decir que la variable **server_cert_pem_start** contiene el contenido del certificado utilizado en la conexión HTTPS sin que este haya sido o sea modificado durante la compilación.



<br />

>Tareas
>
>- Hacer funcionar el ejemplo conectando a un servidor que estará ejecutando en el equipo del profesor. Se usará este certificado para la conexión segura por HTTP y la red WiFi creada en el laboratorio. Se proporcionarán los credenciales de la WiFi y la IP del servidor durante el laboratorio.
>- Alterar un byte del fichero del certificado y probar nuevamente.
>- [Seguir los pasos del ejemplo](https://github.com/espressif/esp-idf/tree/master/examples/system/ota) para crear vuestro propio servidor HTTPS y certificado y probad de nuevo