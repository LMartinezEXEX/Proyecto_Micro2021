# Proyecto Microcontroladores 2021

El proyecto propuesto consiste en una medición periodica de la humedad relativa (HR) actual del entorno para tomar acciones mediante comandos infrarrojos al momento de pasar un determinado umbral.

Para el proyecto se utilizó el siguiente hardware:
* [STM32F411E](https://www.st.com/en/evaluation-tools/32f411ediscovery.html)
* DHT11
* KY 005
* Ky 022

El codigo base sobre el cual se avanzó fue generado por la herramienta de STM, [CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html). Sobre el cual, ademas, cargamos el S.O. [FreeRTOS](https://www.freertos.org/index.html).

Esta aplicacion se comunica mediante el Virtual Com Port (VCP) para ver su estado con la siguiente configuracion:
* Baud = 9600
* Data size = 8
* Parity = None

Ademas, es posible suplirle comandos para modificar la estrucutra interna donde se guardan los códigos infrarrojos que se ejecutaran:
* CP indice: donde indice es un número entre 0 y MAX_CODES. <br>
De esta forma apuntaremos al código que queremos modificar.

* HR+ indice: donde indice es un número entre 0 y MAX_CODES. <br>
De esta forma apuntaremos al código que queremos ejecutar por infrarrojo al subir la HR

* HR- indice: donde indice es un número entre 0 y MAX_CODES. <br>
De esta forma apuntaremos al código que queremos ejecutar por infrarrojo al bajar la HR

* codes<br>
Permite visualizar la estructura actual de los códigos infrarrojos.

* reset<br>
Resetear la estructura de los códigos a infrarrojos a 0, y moviendo todos los punteros al código 0.

La estructura de códigos infrarrojos junto a los punteros son guardados en memoria FLASH, por lo cual no se perderan al desenergizar el microcontrolador.

---

Para suplir un nuevo código a la estructura, oprimir el boton de usuario en el microcontrolador. Notar que el nuevo código solo sera guardado si se envia un "Y" (Yes) para asegurar la correcta recepcion del mismo.

---

#### IMPORTANTE
Si al iniciar el programa se enciende el led rojo del microcontrolador:
* Si titila, se produjo un error al momento de inicializar/ crear las distintas estructuras de FreeRTOS requeridas para el funcionamiento correcto.
* Si no titila, al igual que lo anterior, pero ademas no fue posible crear/inicializar el timer de FreeRTOS.