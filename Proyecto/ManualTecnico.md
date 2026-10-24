 <div align="center">

<p align="left" style="font-size: 18px; font-weight: bold; margin: 0;">UNIVERSIDAD DE SAN CARLOS DE GUATEMALA</p>
<p align="left" style="font-size: 18px; font-weight: bold; margin: 5px 0;">FACULTAD DE INGENIERÍA</p>
<p align="left" style="font-size: 18px; font-weight: bold; margin: 5px 0;">ESCUELA DE CIENCIAS Y SISTEMAS</p>
<p align="left" style="font-size: 18px; font-weight: bold; margin: 5px 0;">ORGANIZACIÓN COMPUTACIONAL</p>

<br><br>

<p align="center" style="font-size: 32px; font-weight: bold; margin: 0;">Manual Técnico</p>

<br>

<p align="center" style="font-size: 28px; font-weight: bold; margin: 0;">SmartSort GT</p>

<br><br>

<img src="imagenes/usac.png" width="250" alt="Logo de la Universidad de San Carlos de Guatemala">

<br><br><br>

<p align="center" style="font-size: 20px; font-weight: bold; margin: 0;">Integrantes</p>

<br>

<table align="center">
  <thead>
    <tr>
      <th align="center">Carné</th>
      <th align="center">Nombre completo</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td align="center">202300497</td>
      <td align="center">Ludsvin Anthony Marcelino Cuxum Cahuec</td>
    </tr>
    <tr>
      <td align="center">202300627</td>
      <td align="center">Diego Jose Castro Lux</td>
    </tr>
    <tr>
      <td align="center">202408977</td>
      <td align="center">Jonathan Eduardo Fuentes Garcia</td>
    </tr>
    <tr>
      <td align="center">202401461</td>
      <td align="center">Andre Emanuel Negreros Pacaya</td>
    </tr>
  </tbody>
</table>

<br><br>

<p align="center" style="font-size: 16px; margin: 0;">Guatemala, octubre de 2026</p>

</div>

## 1. Introducción

SmartSort GT es un proyecto del curso de Organización Computacional de la Facultad de Ingeniería de la USAC, planteado como un reto de Cargo Rapido GT, empresa guatemalteca de mensajería. Su centro de distribución clasifica los paquetes de forma manual, lo que genera errores de desvío, no deja registro de lo procesado por turno y depende del criterio del operario.

Como solución se desarrolla un prototipo de clasificadora automática de paquetes, construido sobre una maqueta de banda transportadora y controlado por un Arduino. Dos sensores láser (diodo láser y LDR) colocados a distintas alturas determinan si cada paquete es pequeño, grande o de rechazo, y dos servomotores lo desvían al contenedor correspondiente.

El sistema guarda las estadísticas de producción en la EEPROM, de modo que no se pierden ante reinicios o cortes de energía. El operario lo controla desde su smartphone por Bluetooth (HC-06) y sigue el estado en una pantalla LCD, con apoyo de LEDs y un buzzer. Con esto se busca demostrar que microcontroladores de bajo costo pueden automatizar un proceso de clasificación con registros confiables.

## 2. Objetivos

### 2.1. Objetivo general

Implementar un prototipo funcional de clasificadora automática de paquetes basado en Arduino, que detecte el tamaño mediante sensores láser, desvíe los paquetes con servomotores y registre estadísticas de producción en la EEPROM, con control remoto vía Bluetooth.

### 2.2. Objetivos específicos

1. Construir una maqueta con banda transportadora, portal de detección con dos sensores láser, dos servomotores desviadores y tres contenedores (pequeño, grande y rechazo).
2. Clasificar correctamente al menos 10 paquetes consecutivos, sin errores de desvío, sincronizando la detección con la activación de los servomotores.
3. Almacenar en la EEPROM los contadores de paquetes grandes, pequeños, rechazados y el total histórico, con validación de rango y persistencia tras reiniciar el Arduino.
4. Controlar el sistema desde un smartphone mediante el módulo HC-06, con todos los comandos definidos (`iniciar`, `pausa`, `detener`, `reporte`, `Resetear`, `modo_lote:N`, `modo_normal`, `modo_mantenimiento` y `estado`).
5. Informar al operario en tiempo real mediante la pantalla LCD, los LEDs de estado y el buzzer.
6. Gestionar todos los tiempos con `millis()`, sin usar `delay()` en el ciclo principal.

## 3. Descripción general del sistema

SmartSort GT es un sistema embebido que combina componentes electrónicos, programación y una estructura mecánica para automatizar la clasificación de paquetes.

El sistema está compuesto por una banda transportadora que desplaza los objetos, un portal de detección con dos sensores ópticos, dos servomotores desviadores, tres contenedores y un conjunto de dispositivos de control y visualización.

El Arduino recibe las lecturas de los sensores y determina la categoría del paquete de acuerdo con la lógica programada. Posteriormente, activa el servomotor correspondiente cuando se identifica un paquete pequeño o grande. Si el objeto se identifica como rechazo, los servomotores permanecen en posición neutral para permitir su paso hacia el contenedor central.

Además de la clasificación física, el sistema registra las cantidades procesadas en la EEPROM y permite consultar información mediante la pantalla LCD. La comunicación Bluetooth proporciona una interfaz para enviar instrucciones desde un teléfono inteligente.

### 3.1. Módulos principales

| Módulo | Componentes principales | Función |
|---|---|---|
| Unidad de control | Arduino  Mega | Procesa las señales y coordina los módulos del sistema. |
| Detección óptica | Dos diodos láser y dos LDR | Detecta la interrupción de los haces de luz. |
| Transporte | Motor DC y banda transportadora | Desplaza los paquetes por el sistema. |
| Clasificación | Dos servomotores | Desvía los paquetes hacia sus respectivos contenedores. |
| Comunicación | Módulo Bluetooth HC-05 | Recibe comandos enviados desde una aplicación móvil. |
| Visualización | Pantalla LCD I2C de 16x2 | Presenta mensajes de estado y estadísticas. |
| Indicadores | LED azul, verde y rojo | Comunican visualmente el estado y los resultados de clasificación. |
| Alerta sonora | Buzzer | Emite sonidos asociados a determinados eventos. |
| Almacenamiento | EEPROM interna del Arduino | Conserva las estadísticas de producción. |

### 3.2. Arquitectura de funcionamiento

El sistema sigue una secuencia general de operación:

1. El Arduino inicia los periféricos y recupera las estadísticas almacenadas en la EEPROM.
2. El sistema activa el indicador azul para señalar que está encendido.
3. El operador envía el comando correspondiente mediante Bluetooth.
4. Cuando la banda está activa, el Arduino consulta las lecturas de los sensores LDR.
5. El programa compara las lecturas con un umbral de iluminación configurado.
6. De acuerdo con las señales obtenidas, se determina la categoría del paquete.
7. Si corresponde, se activa el servomotor encargado del desvío.
8. Se actualizan los contadores, la pantalla LCD y los indicadores correspondientes.
9. Las estadísticas se escriben en la EEPROM.
10. El sistema continúa procesando paquetes hasta que se recibe una instrucción que modifica o detiene su funcionamiento.


## 4. Requisitos de hardware y software

### 4.1. Hardware utilizado

| Componente | Cantidad | Función |
|---|---:|---|
| Arduino Mega | 1 | Control principal del sistema. |
| Módulo Bluetooth HC-05 | 1 | Comunicación inalámbrica serial. |
| Pantalla LCD de 16x2 con I2C | 1 | Visualización de mensajes. |
| Servomotor | 2 | Desvío de paquetes grandes y pequeños. |
| Motor DC | 1 | Movimiento de la banda transportadora. |
| Diodo láser | 2 | Emisión de los haces de detección. |
| Fotorresistencia LDR | 2 | Detección de los cambios de iluminación. |
| LED azul | 1 | Indicador de sistema activo y listo. |
| LED verde | 1 | Indicador de clasificación correcta. |
| LED rojo | 1 | Indicador de rechazo o fallo, según la implementación. |
| Buzzer | 1 | Señalización sonora. |
| Resistencias y cables | Según necesidad | Conexiones y acondicionamiento eléctrico. |
| Materiales para maqueta | Según necesidad | Construcción de la estructura y los contenedores. |

Las cantidades y los modelos deben verificarse con los componentes instalados realmente en el prototipo.

### 4.2. Software y herramientas

| Herramienta | Uso |
|---|---|
| Arduino IDE | Edición, compilación y carga del programa al microcontrolador. |
| Serial Bluetooth Terminal | Envío de comandos desde un dispositivo móvil. |
| Bibliotecas de Arduino | Control de la pantalla, servomotores, memoria EEPROM y comunicación serial. |
| GitHub | Almacenamiento y control de versiones del código y la documentación. |

## 5. Conexiones

### 5.1. Distribución de pines

De acuerdo con el código fuente proporcionado, la distribución de pines es la siguiente:

| Componente | Pin del Arduino | Tipo de señal | Descripción |
|---|---|---|---|
| LDR inferior | A0 | Entrada analógica | Lectura del sensor inferior. |
| LDR superior | A1 | Entrada analógica | Lectura del sensor superior. |
| Motor de la banda | 7 | Salida digital | Señal de activación del motor. |
| Buzzer | 8 | Salida | Generación de señales sonoras. |
| LED azul | 4 | Salida digital | Indicador de sistema encendido. |
| LED verde | 5 | Salida digital | Indicador de clasificación correcta. |
| LED rojo | 6 | Salida digital | Indicador de rechazo. |
| Servomotor para grandes | 9 | Control de servo | Desvío de paquetes grandes. |
| Servomotor para pequeños | 10 | Control de servo | Desvío de paquetes pequeños. |
| Bluetooth HC-05 TX | 18, como RX del Arduino | Comunicación serial | Recepción de datos desde el módulo. |
| Bluetooth HC-05 RX | 19, como TX del Arduino | Comunicación serial | Transmisión de datos hacia el módulo. |
| LCD SDA | SDA del Arduino | Comunicación I2C | Línea de datos de la pantalla. |
| LCD SCL | SCL del Arduino | Comunicación I2C | Línea de reloj de la pantalla. |


La pantalla LCD utiliza la dirección I2C `0x27` en el código proporcionado. Si la pantalla no muestra información, se debe verificar su dirección real y las conexiones SDA, SCL, alimentación y tierra.

La conexión del motor DC debe realizarse mediante un circuito de potencia o controlador apropiado. No se debe alimentar directamente el motor desde un pin digital del Arduino.

## 6. Sistema de detección láser y calibración

### 6.1. Descripción del sistema de detección

El sistema de detección está formado por dos emisores láser y dos fotorresistencias LDR colocados a diferentes alturas. Los láseres generan haces de luz dirigidos hacia las respectivas fotorresistencias. Cuando un paquete interrumpe uno de estos haces, cambia la cantidad de luz que recibe el sensor.

La fotorresistencia modifica su resistencia eléctrica de acuerdo con la iluminación que recibe. Mediante un circuito de lectura adecuado, este cambio se convierte en una señal que el Arduino puede interpretar utilizando sus entradas analógicas.

El programa consulta las entradas A0 y A1 mediante la función `analogRead()`. Posteriormente, compara cada lectura con un umbral definido en el código. Si la lectura cae por debajo del umbral, el programa interpreta que el haz de luz correspondiente ha sido interrumpido.

### 6.2. Función de los sensores

| Sensor | Ubicación | Función |
|---|---|---|
| LDR inferior | A la altura inferior del portal | Detecta la interrupción del haz inferior. |
| LDR superior | A una altura superior del portal | Detecta la interrupción del haz superior. |

La clasificación depende de la disposición física de los sensores y de la lógica utilizada en el programa. Por ello, las alturas deben ajustarse para que las diferentes dimensiones de los paquetes produzcan señales distinguibles.

### 6.3. Lógica de clasificación implementada en el código

El programa define dos variables booleanas:

- `l1_corte`: indica si la lectura del sensor inferior está por debajo del umbral.
- `l2_corte`: indica si la lectura del sensor superior está por debajo del umbral.

La condición programada para interpretar las lecturas es la siguiente:

| Sensor inferior (`l1_corte`) | Sensor superior (`l2_corte`) | Resultado según el código |
|---:|---:|---|
| 0 | 0 | No se procesa un paquete. |
| 0 | 1 | Se clasifica como rechazo. |
| 1 | 0 | Se clasifica como pequeño. |
| 1 | 1 | Se clasifica como grande. |

La tabla describe el comportamiento de la lógica actual del código. Para que la clasificación sea correcta, es necesario comprobar que la disposición física de los sensores produce estos estados para cada tipo de paquete.

### 6.4. Calibración de los sensores

El código establece inicialmente un umbral de 800 para comparar las lecturas analógicas. Este valor debe verificarse con las lecturas reales de los LDR, ya que puede variar de acuerdo con la iluminación ambiental, la distancia del láser, las características de la fotorresistencia y las condiciones de montaje.

Se recomienda realizar el siguiente procedimiento:

1. Encender los láseres y comprobar que cada haz llegue correctamente a su LDR.
2. Leer los valores analógicos con los sensores iluminados.
3. Interrumpir cada haz por separado y registrar los valores obtenidos.
4. Comparar las lecturas con el umbral utilizado en el programa.
5. Ajustar el umbral para distinguir con suficiente margen entre los estados iluminado e interrumpido.
6. Probar con paquetes pequeños, grandes y objetos que deban rechazarse.
7. Repetir las pruebas con la iluminación y la velocidad de la banda que se utilizarán durante la demostración.


## 7. Control del motor de la banda transportadora

El motor DC es el encargado de desplazar los paquetes a través de la estructura. En el código proporcionado, el control del motor se realiza mediante el pin digital 7.

Cuando se recibe un comando que activa la banda, el programa coloca el pin en estado `HIGH`. Cuando se pausa o detiene el sistema, el pin pasa a estado `LOW`.

Es importante aclarar que el pin del Arduino proporciona una señal de control, no la potencia necesaria para alimentar directamente el motor. El circuito físico debe incorporar un transistor, controlador de motor o módulo de potencia apropiado, según las características eléctricas del motor utilizado.

### 7.1. Estados de funcionamiento

| Estado | Señal de control | Resultado esperado |
|---|---|---|
| Banda activa | `HIGH` | El circuito de potencia activa el motor. |
| Banda pausada | `LOW` | Se desactiva la señal de control del motor. |
| Banda detenida | `LOW` | El motor deja de recibir la orden de funcionamiento. |
| Modo mantenimiento | `HIGH` | La banda funciona sin ejecutar la clasificación normal según el diseño previsto. |

El comportamiento físico del motor depende del circuito de potencia utilizado y debe verificarse durante la instalación.

## 8. Comunicación Bluetooth

### 8.1. Descripción general

El módulo HC-05 permite enviar instrucciones al Arduino mediante comunicación serial inalámbrica. Para operar el sistema se utiliza una aplicación de terminal Bluetooth, como Serial Bluetooth Terminal.

El Arduino inicializa el puerto serial del módulo a 9600 baudios y revisa continuamente si hay datos disponibles. Cuando recibe un comando, lee el texto hasta encontrar un salto de línea y elimina los espacios exteriores mediante `trim()`.

Después de recibir el texto, el programa compara su contenido con los comandos definidos. Si encuentra una coincidencia, ejecuta la acción correspondiente; de lo contrario, muestra un mensaje de error en la pantalla LCD.

### 8.2. Comandos disponibles

| Comando | Función prevista |
|---|---|
| `iniciar` | Activa la banda y establece el modo normal. |
| `pausa` | Detiene temporalmente la banda. |
| `detener` | Detiene la banda y muestra el reporte. |
| `reporte` | Muestra las estadísticas actuales. |
| `Resetear` | Reinicia los contadores del turno y conserva el total histórico. |
| `modo_lote:N` | Configura el procesamiento de N paquetes y activa la banda. |
| `modo_normal` | Establece el modo normal de funcionamiento. |
| `modo_mantenimiento` | Activa la banda y evita la clasificación normal. |
| `estado` | Muestra el modo actual y el estado de la banda. |

Los comandos deben enviarse respetando la escritura utilizada en el programa. Por ejemplo, el código compara `Resetear` con la primera letra en mayúscula, mientras que los demás comandos se comparan en minúsculas.

### 8.3. Procedimiento de conexión

1. Alimentar el Arduino y el módulo Bluetooth.
2. Activar Bluetooth en el teléfono.
3. Emparejar el teléfono con el módulo HC-05 siguiendo las instrucciones del dispositivo.
4. Abrir la aplicación Serial Bluetooth Terminal.
5. Seleccionar el módulo emparejado e iniciar la conexión.
6. Configurar la aplicación para enviar los comandos como líneas de texto, terminadas en un salto de línea.
7. Enviar un comando de consulta, como `estado`, y verificar la respuesta del sistema mediante la pantalla LCD.
8. Probar los comandos de operación de forma individual.

## 9. Pantalla LCD e indicadores de estado

### 19.1. Pantalla LCD

La pantalla LCD I2C de 16 columnas por 2 filas permite mostrar información relevante para el operador. El programa utiliza la biblioteca `LiquidCrystal_I2C` para inicializar la pantalla, encender la iluminación y escribir mensajes en sus dos filas.

La función `actualizarLCD()` centraliza la actualización de los mensajes. Primero limpia la pantalla y después escribe el contenido recibido en cada fila.

Entre los mensajes definidos se encuentran:

| Situación | Primera fila | Segunda fila |
|---|---|---|
| Inicio | SmartSort GT | Esperando cmd.. |
| Banda activa | Modo: NORMAL | Banda: ON |
| Pausa | PAUSADO | Banda: OFF |
| Paquete pequeño | Paq: PEQUENO | Desviando S2 |
| Paquete grande | Paq: GRANDE | Desviando S1 |
| Rechazo | Paq: RECHAZO | Pasa libre |
| Lote terminado | Lote completo | Procesados: N |
| Comando desconocido | ERROR: | Cmd invalido |

Los mensajes mostrados dependen del comando o evento procesado. La tabla presenta ejemplos de las cadenas definidas en el código y no implica que todos los estados posibles tengan una pantalla exclusiva.

### 9.2. Indicadores LED

El sistema utiliza tres LED para proporcionar información visual.

| LED | Función prevista |
|---|---|
| Azul | Indica que el sistema está encendido y listo. |
| Verde | Indica una clasificación correcta. |
| Rojo | Indica que un paquete fue enviado a rechazo. |

En el código, el LED azul se activa durante la inicialización. El LED verde se enciende al clasificar paquetes pequeños o grandes, mientras que el LED rojo se enciende al registrar un rechazo.

La función `gestionarTemporizadores()` apaga los LED verde y rojo cuando transcurre el tiempo configurado. El LED azul no se apaga mediante esa función.

### 9.3. Buzzer

El buzzer genera señales sonoras para indicar determinados eventos. En el código se utiliza la función `tone()` para establecer la frecuencia y duración de cada señal.

| Evento | Frecuencia configurada | Duración configurada |
|---|---:|---:|
| Paquete pequeño | 1500 Hz | 200 ms |
| Paquete grande | 1000 Hz | 200 ms |
| Rechazo | 500 Hz | 100 ms |
| Finalización de lote | 800 Hz | 1000 ms |

Las frecuencias y duraciones corresponden a los valores presentes en el código. La señal que se escucha realmente depende del tipo de buzzer y de las conexiones utilizadas.

## 10. Almacenamiento de estadísticas en EEPROM

### 10.1. Concepto y propósito

La EEPROM es una memoria no volátil que permite conservar datos aunque el Arduino pierda la alimentación eléctrica. En SmartSort GT se utiliza para mantener las estadísticas de producción, de modo que los contadores puedan recuperarse al iniciar nuevamente el sistema.

El código utiliza las funciones `EEPROM.get()` y `EEPROM.put()` para leer y escribir los valores almacenados en direcciones específicas.

### 10.2. Distribución de direcciones

La distribución definida en el código es la siguiente:

| Dato almacenado | Dirección inicial | Tipo de dato | Tamaño utilizado |
|---|---:|---|---:|
| Paquetes grandes | 0 | `int` | 2 bytes en la plataforma prevista |
| Paquetes pequeños | 2 | `int` | 2 bytes en la plataforma prevista |
| Paquetes rechazados | 4 | `int` | 2 bytes en la plataforma prevista |
| Total histórico | 6 | `long` | 4 bytes en la plataforma prevista |

La distribución se basa en la utilización de un `int` de dos bytes y un `long` de cuatro bytes, como se espera en las plataformas Arduino Uno y Mega basadas en AVR. Si se utiliza otra arquitectura, deben verificarse los tamaños de los tipos de datos.

### 10.3. Lectura de datos

La función `cargarEEPROM()` recupera las estadísticas al iniciar el sistema. Cada valor se lee desde la dirección definida para su tipo de dato.

El propósito de esta función es recuperar los contadores previamente almacenados para que el sistema pueda continuar utilizando las estadísticas disponibles después de un reinicio.

El código contiene una validación inicial para detectar determinados valores fuera del rango esperado. Esta validación es limitada y no comprueba por sí sola que todos los campos de la EEPROM contengan datos válidos.

### 10.4. Escritura de datos

La función `guardarEEPROM()` escribe cada contador en su dirección correspondiente. Se ejecuta después de registrar una clasificación y cuando se reinician los contadores del turno.

Esta organización permite centralizar el almacenamiento y facilita la revisión de las direcciones utilizadas para cada dato.

### 10.5. Reinicio de contadores

Cuando se recibe el comando `Resetear`, el programa asigna cero a los contadores de paquetes grandes, pequeños y rechazados. Después, guarda los valores actualizados en la EEPROM.

El contador `totalHistorico` no se pone en cero durante esta operación. De esta forma, el comando reinicia las estadísticas del turno sin eliminar el total histórico acumulado.

### 10.6. Consideraciones técnicas

Para mejorar la confiabilidad del almacenamiento se recomienda:

- Validar todos los contadores al iniciar el sistema.
- Definir límites máximos razonables para los valores almacenados.
- Verificar los datos antes de utilizarlos.
- Comprobar que cada campo ocupe el espacio de memoria previsto.
- Evitar escrituras innecesarias para reducir el desgaste de la EEPROM.
- Probar que los datos se mantengan después de apagar y encender el Arduino.
- Considerar un valor de identificación o versión para distinguir datos inicializados de datos válidos.

Estas medidas son recomendaciones de mantenimiento y no deben interpretarse como características que ya están implementadas en el código actual.

## 11. Temporización y gestión del programa

### 11.1. Uso de `millis()`

La función `millis()` devuelve el tiempo transcurrido desde que el Arduino comenzó a ejecutar el programa. Se utiliza para medir intervalos sin depender de pausas explícitas como `delay()` en las funciones principales de temporización.

En SmartSort GT se emplea para controlar cuánto tiempo permanece activo un movimiento de los servomotores y cuánto tiempo se mantienen encendidos los indicadores luminosos.

### 11.2. Variables de temporización

| Variable | Función |
|---|---|
| `tiempoServo` | Guarda el instante en que se inicia la secuencia del servomotor. |
| `servoActivo` | Indica que hay una secuencia de actuación en curso. |
| `tiempoLed` | Guarda el instante de referencia para controlar los LED. |

La función `gestionarTemporizadores()` comprueba si han transcurrido dos segundos desde la activación del servo. Si se cumple esa condición, devuelve ambos servomotores a su posición neutral y libera la bandera `servoActivo`.

También comprueba si han transcurrido más de 500 milisegundos desde la referencia guardada en `tiempoLed`, para apagar los LED verde y rojo.

### 11.3. Consideración sobre la recepción serial

Aunque las funciones de temporización utilizan `millis()`, la instrucción `readStringUntil('\n')` puede esperar la llegada del salto de línea o el vencimiento del tiempo de espera de lectura serial. Por ello, el comportamiento del programa no es completamente no bloqueante.

Para mejorar la respuesta ante eventos simultáneos, se puede sustituir la lectura completa de la línea por una recepción carácter por carácter con un búfer y un tiempo de espera controlado.

Esta mejora debe implementarse y probarse antes de documentarla como parte del funcionamiento definitivo.

## 12. Organización del código fuente

El programa está dividido en funciones que permiten separar las tareas principales del sistema.

| Función | Responsabilidad |
|---|---|
| `setup()` | Inicializa los puertos seriales, los pines, los servomotores, la pantalla LCD y la EEPROM. |
| `loop()` | Ejecuta continuamente la recepción de comandos, la lectura de sensores y la gestión de temporizadores. |
| `procesarBluetooth()` | Lee y procesa las instrucciones recibidas mediante Bluetooth. |
| `leerSensores()` | Obtiene las lecturas de los LDR y determina la categoría según las condiciones programadas. |
| `clasificarPaquete()` | Actualiza los contadores, activa los actuadores y registra el evento. |
| `gestionarTemporizadores()` | Controla el retorno de los servos y el apagado de los indicadores. |
| `resetServos()` | Coloca ambos servomotores en la posición neutral. |
| `actualizarLCD()` | Muestra los mensajes en la pantalla LCD. |
| `cargarEEPROM()` | Recupera los contadores almacenados. |
| `guardarEEPROM()` | Escribe las estadísticas en la memoria no volátil. |
| `mostrarReporte()` | Construye y muestra un resumen de los contadores. |

La separación en funciones facilita la lectura del programa y permite localizar con mayor facilidad los puntos que deben revisarse cuando se presenta un problema.

## 13. Código fuente comentado (.ino)

El siguiente código conserva la estructura y la lógica del código proporcionado, agregando comentarios para explicar las funciones, las variables y las decisiones principales. Los comentarios documentan el comportamiento actual; no significan que todas las condiciones de funcionamiento hayan sido verificadas físicamente.

```cpp
// ============================================================
// PROYECTO: SMARTSORT GT (Version Actualizada)
// PLATAFORMA: Arduino Mega 2560
//
// Descripcion:
// Sistema de clasificacion de paquetes no bloqueante.
// Utiliza maquina de estados para los sensores y control de
// tiempo con millis() para permitir recepcion Bluetooth continua
// y manejo simultaneo de actuadores. Incorpora control de rele.
// ============================================================

// ------------------------------------------------------------
// BIBLIOTECAS
// ------------------------------------------------------------

// Permite controlar los servomotores.
#include <Servo.h>

// Permite utilizar la comunicacion I2C de la pantalla LCD.
#include <Wire.h>

// Permite controlar una pantalla LCD mediante un adaptador I2C.
#include <LiquidCrystal_I2C.h>

// Permite leer y escribir datos en la memoria EEPROM.
#include <EEPROM.h>


// ------------------------------------------------------------
// CONFIGURACION DE PINES (Arduino Mega 2560)
// ------------------------------------------------------------

// Pin para el control del rele que enciende/apaga la banda.
const int pinRelayMotor = 7;

// Entradas analogicas para los sensores de luz (LDR).
const int pinLDRBajo = A0;  // L1 (Sensor Inferior)
const int pinLDRAlto = A1;  // L2 (Sensor Superior)

// Pines PWM para el control de los servomotores.
const int pinServo1 = 8;
const int pinServo2 = 9;

// Indicadores visuales y sonoro del estado del sistema.
const int pinLedAzul = 2;
const int pinLedVerde = 3;
const int pinLedRojo = 4;
const int pinBuzzer = 5;

// Valor umbral para detectar si el haz de luz de los laseres fue interrumpido.
const int UMBRAL_LDR = 700;


// ------------------------------------------------------------
// DIRECCIONES DE MEMORIA EEPROM
// ------------------------------------------------------------
// Espacios asignados para guardar de forma permanente los contadores.
const int DIR_GRANDE = 0;           // Ocupa 2 bytes (int)
const int DIR_PEQUENO = 2;          // Ocupa 2 bytes (int)
const int DIR_RECHAZO = 4;          // Ocupa 2 bytes (int)
const int DIR_TOTAL_HISTORICO = 8;  // Ocupa 4 bytes (unsigned long)


// ------------------------------------------------------------
// OBJETOS PARA LOS PERIFERICOS
// ------------------------------------------------------------

Servo servo1;
Servo servo2;
LiquidCrystal_I2C lcd(0x27, 16, 2);


// ------------------------------------------------------------
// VARIABLES PARA TEMPORIZACION DE SERVOS
// ------------------------------------------------------------

unsigned long tiempoActivacionS1 = 0;
unsigned long tiempoActivacionS2 = 0;
bool s1Activo = false;
bool s2Activo = false;

// Tiempos que los servos se mantienen en posicion de desvio.
const unsigned long TIEMPO_RETORNO_S1 = 4500; 
const unsigned long TIEMPO_RETORNO_S2 = 7500; 


// ------------------------------------------------------------
// VARIABLES DE SENSORES Y MAQUINA DE ESTADOS
// ------------------------------------------------------------

// Estados posibles para la deteccion de un paquete.
enum EstadoDeteccion { ESPERANDO, CONFIRMANDO, ESPERANDO_LIBERACION };
EstadoDeteccion estadoSensor = ESPERANDO;

unsigned long tiempoInicioDeteccion = 0;
const unsigned long TIEMPO_CONFIRMACION = 150; // Ventana para evitar falsos positivos

bool l1Detectado = false;
bool l2Detectado = false;
unsigned long tiempoRechazo = 0;
bool rechazoActivo = false;


// ------------------------------------------------------------
// VARIABLES PARA BUZZER (MODO NO BLOQUEANTE)
// ------------------------------------------------------------

bool beepRechazoActivo = false;
int beepsRealizados = 0;
unsigned long tiempoUltimoBeep = 0;


// ------------------------------------------------------------
// VARIABLES DE BLUETOOTH Y ESTADO DEL SISTEMA
// ------------------------------------------------------------

// Almacena los caracteres recibidos por Bluetooth hasta armar el comando.
String comandoBT = "";

// Estados generales de operacion de la maquina.
enum ModoOperacion { DETENIDO, NORMAL, PAUSADO, MANTENIMIENTO, LOTE, FIN_LOTE };
ModoOperacion modoActual = DETENIDO;

// Estadisticas de produccion.
int contGrande = 0;
int contPequeno = 0;
int contRechazo = 0;
unsigned long totalHistorico = 0; 
int objetivoLote = 0;
int procesadosLote = 0;


// ============================================================
// FUNCION setup()
// ============================================================
// Se ejecuta al encender el sistema. Inicializa hardware,
// configura la EEPROM y establece estados iniciales.
// ============================================================

void setup() {

  // Inicializa la consola serie para diagnostico por USB.
  Serial.begin(9600);   

  // Inicializa la comunicacion con el modulo HC-06 usando el 
  // puerto serial por hardware del Mega (Pines 18 TX1 y 19 RX1).
  Serial1.begin(9600);  
  
  // Configuracion de pines de salida.
  pinMode(pinRelayMotor, OUTPUT);
  pinMode(pinLedAzul, OUTPUT);
  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedRojo, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  // Vincula los servos a sus respectivos pines.
  servo1.attach(pinServo1);
  servo2.attach(pinServo2);

  // Posicion neutral inicial.
  servo1.write(90); 
  servo2.write(90);

  // Inicializa la pantalla LCD y enciende la luz de fondo.
  lcd.init();
  lcd.backlight();
  
  // Recupera los valores estadisticos guardados en la EEPROM.
  EEPROM.get(DIR_GRANDE, contGrande);
  EEPROM.get(DIR_PEQUENO, contPequeno);
  EEPROM.get(DIR_RECHAZO, contRechazo);
  EEPROM.get(DIR_TOTAL_HISTORICO, totalHistorico);

  // Validacion de seguridad para evitar datos corruptos
  // o basura si la EEPROM es nueva.
  if (contGrande < 0 || contGrande > 32000) contGrande = 0;
  if (contPequeno < 0 || contPequeno > 32000) contPequeno = 0;
  if (contRechazo < 0 || contRechazo > 32000) contRechazo = 0;
  if (totalHistorico == 0xFFFFFFFF) totalHistorico = 0; 
  
  // Mensaje de bienvenida en pantalla.
  actualizarLCD("SmartSort GT", "Esperando cmd...");
  
  // Enciende LED azul de forma constante indicando sistema listo.
  digitalWrite(pinLedAzul, HIGH);

  // Apaga el rele. (Logica negativa: HIGH significa apagado).
  digitalWrite(pinRelayMotor, HIGH); 
}


// ============================================================
// FUNCION loop()
// ============================================================
// Bucle principal. Escanea Bluetooth, lee sensores, evalua 
// la maquina de estados y maneja tiempos de forma no bloqueante.
// ============================================================

void loop() {

  // Captura el tiempo actual en milisegundos para usar como
  // referencia en todos los temporizadores.
  unsigned long tiempoActual = millis();


  // ----------------------------------------------------------
  // 1. PROCESAMIENTO BLUETOOTH NO BLOQUEANTE
  // ----------------------------------------------------------
  // Lee el bufer caracter por caracter sin usar delay ni readString.
  while (Serial1.available() > 0) {
    char c = Serial1.read();

    // Si se detecta un salto de linea, finalizo el comando.
    if (c == '\n' || c == '\r') {
      if (comandoBT.length() > 0) {
        ejecutarComando(comandoBT);
        comandoBT = ""; // Limpia el buffer para el siguiente comando
      }
    } else {
      // Concatena caracteres al comando.
      comandoBT += c;
    }
  }


  // ----------------------------------------------------------
  // 2. LECTURA DE SENSORES Y CLASIFICACION (Maquina de Estados)
  // ----------------------------------------------------------
  if (modoActual == NORMAL || modoActual == LOTE) {

    // Lee los sensores analogicos LDR.
    int valorL1 = analogRead(pinLDRBajo);
    int valorL2 = analogRead(pinLDRAlto);

    // Evalua si el laser se interrumpio (lectura baja).
    bool actualL1 = (valorL1 < UMBRAL_LDR);
    bool actualL2 = (valorL2 < UMBRAL_LDR);

    // ESTADO 1: Esperando la llegada de un objeto.
    if (estadoSensor == ESPERANDO) {
      if (actualL1 || actualL2) {
        estadoSensor = CONFIRMANDO;
        tiempoInicioDeteccion = tiempoActual;
        l1Detectado = actualL1;
        l2Detectado = actualL2;
      }
    } 
    // ESTADO 2: Confirmando deteccion (evita lecturas falsas).
    else if (estadoSensor == CONFIRMANDO) {

      // Acumula la deteccion si algun sensor se corta en esta ventana de tiempo.
      if (actualL1) l1Detectado = true;
      if (actualL2) l2Detectado = true;

      // Si termino la ventana de confirmacion, decide el tipo.
      if (tiempoActual - tiempoInicioDeteccion >= TIEMPO_CONFIRMACION) {
        int tipo = 0;
        
        // Logica de tamaños
        if (l1Detectado && l2Detectado) tipo = 1;      // Ambos sensores: GRANDE
        else if (l1Detectado && !l2Detectado) tipo = 2; // Solo abajo: PEQUENO
        else if (!l1Detectado && l2Detectado) tipo = 3; // Solo arriba: RECHAZO (Anomalo)


        // ACCION: PAQUETE GRANDE
        if (tipo == 1 && !s1Activo) { 
          servo1.write(45);
          digitalWrite(pinLedVerde, HIGH);
          tone(pinBuzzer, 1000, 200); // 1 beep corto
          actualizarLCD("Paq: GRANDE", "Desviando S1");
          
          s1Activo = true;
          tiempoActivacionS1 = tiempoActual;
          
          if (contGrande < 32000) contGrande++;
          if (totalHistorico < 4000000000) totalHistorico++; 
          
          EEPROM.put(DIR_GRANDE, contGrande);
          EEPROM.put(DIR_TOTAL_HISTORICO, totalHistorico);
          procesadosLote++;
        } 
        
        // ACCION: PAQUETE PEQUENO
        else if (tipo == 2 && !s2Activo) { 
          servo2.write(140);
          digitalWrite(pinLedVerde, HIGH);
          tone(pinBuzzer, 1500, 200); // 1 beep corto
          actualizarLCD("Paq: PEQUENO", "Desviando S2");
          
          s2Activo = true;
          tiempoActivacionS2 = tiempoActual;
          
          if (contPequeno < 32000) contPequeno++;
          if (totalHistorico < 4000000000) totalHistorico++;
          
          EEPROM.put(DIR_PEQUENO, contPequeno);
          EEPROM.put(DIR_TOTAL_HISTORICO, totalHistorico);
          procesadosLote++;
        }
        
        // ACCION: PAQUETE RECHAZADO
        else if (tipo == 3) {
          digitalWrite(pinLedRojo, HIGH);
          actualizarLCD("Paq: RECHAZO", "Sin desvio");
          
          rechazoActivo = true;
          tiempoRechazo = tiempoActual;
          
          if (contRechazo < 32000) contRechazo++;
          if (totalHistorico < 4000000000) totalHistorico++;
          
          EEPROM.put(DIR_RECHAZO, contRechazo);
          EEPROM.put(DIR_TOTAL_HISTORICO, totalHistorico);
          procesadosLote++;
          
          // Iniciar secuencia de 3 beeps rapidos de forma no bloqueante.
          beepRechazoActivo = true;
          beepsRealizados = 1;
          tiempoUltimoBeep = tiempoActual;
          tone(pinBuzzer, 500, 100);
        }

        // Evalua si se completo la meta en modo por Lotes.
        if (modoActual == LOTE && procesadosLote >= objetivoLote) {
           modoActual = FIN_LOTE; // Cambio de estado a espera de servos
           actualizarLCD("Meta alcanzada", "Terminando...");
        }

        // Pasa al estado de esperar que el objeto se vaya del sensor.
        estadoSensor = ESPERANDO_LIBERACION;
      }
    } 
    // ESTADO 3: Espera a que los laseres vuelvan a verse (objeto salio).
    else if (estadoSensor == ESPERANDO_LIBERACION) {
      if (!actualL1 && !actualL2) {
        estadoSensor = ESPERANDO; // Reinicia la maquina de estados.
      }
    }
  }


  // ----------------------------------------------------------
  // 3. SECUENCIADOR DE BEEPS PARA RECHAZO (No bloqueante)
  // ----------------------------------------------------------
  // Genera multiples tonos seguidos usando la diferencia de tiempo.
  if (beepRechazoActivo) {
    if (tiempoActual - tiempoUltimoBeep >= 200) { 
      if (beepsRealizados < 3) { 
        tone(pinBuzzer, 500, 100);
        tiempoUltimoBeep = tiempoActual;
        beepsRealizados++;
      } else {
        beepRechazoActivo = false; // Finaliza la secuencia.
      }
    }
  }


  // ----------------------------------------------------------
  // 4. CIERRE DE LOTE DEMORADO
  // ----------------------------------------------------------
  // Apaga la banda solo cuando ningun actuador sigue moviendose.
  if (modoActual == FIN_LOTE) {
    if (!s1Activo && !s2Activo && !rechazoActivo) {
      modoActual = DETENIDO;
      
      digitalWrite(pinRelayMotor, HIGH); // Apagar banda (Logica negativa)
      
      actualizarLCD("Lote completo", "Banda: OFF");
      tone(pinBuzzer, 2000, 1500); // Tono largo de aviso final
    }
  }


  // ----------------------------------------------------------
  // 5. GESTION DE RETORNO DE SERVOS Y APAGADO DE LEDS
  // ----------------------------------------------------------
  
  // Revisa si el tiempo del Servo 1 ya caduco.
  if (s1Activo && (tiempoActual - tiempoActivacionS1 >= TIEMPO_RETORNO_S1)) {
    servo1.write(90);
    digitalWrite(pinLedVerde, LOW);
    s1Activo = false;
    refrescarPantallaBase();
  }

  // Revisa si el tiempo del Servo 2 ya caduco.
  if (s2Activo && (tiempoActual - tiempoActivacionS2 >= TIEMPO_RETORNO_S2)) {
    servo2.write(90);
    digitalWrite(pinLedVerde, LOW);
    s2Activo = false;
    refrescarPantallaBase();
  }

  // Revisa si el indicador de rechazo ya debe apagarse.
  if (rechazoActivo && (tiempoActual - tiempoRechazo >= 1500)) {
    digitalWrite(pinLedRojo, LOW);
    rechazoActivo = false;
    refrescarPantallaBase();
  }
}


// ============================================================
// FUNCION ejecutarComando()
// ============================================================
// Identifica el texto recibido y ejecuta la accion correspondiente.
// ============================================================

void ejecutarComando(String cmd) {
  
  // Elimina espacios en blanco y saltos de linea accidentales.
  cmd.trim(); 
  
  if (cmd.equalsIgnoreCase("iniciar") || cmd.equalsIgnoreCase("modo_normal")) {
    modoActual = NORMAL;
    digitalWrite(pinRelayMotor, LOW); // Encender banda (logica negativa)
    actualizarLCD("Modo: NORMAL", "Banda: ON");
  } 
  else if (cmd.equalsIgnoreCase("pausa")) {
    modoActual = PAUSADO;
    digitalWrite(pinRelayMotor, HIGH); // Apagar banda
    actualizarLCD("PAUSADO", "Banda: OFF");
  } 
  else if (cmd.equalsIgnoreCase("detener")) {
    modoActual = DETENIDO;
    digitalWrite(pinRelayMotor, HIGH); // Apagar banda
    
    // Muestra estadisticas del turno
    String rep = "G:" + String(contGrande) + " P:" + String(contPequeno) + " R:" + String(contRechazo)  + " T:" + String(contGrande+contPequeno+contRechazo);
    actualizarLCD("REPORTE FINAL", rep);
  } 
  else if (cmd.equalsIgnoreCase("modo_mantenimiento")) {
    modoActual = MANTENIMIENTO;
    digitalWrite(pinRelayMotor, LOW); // Banda encendida pero no lee sensores
    actualizarLCD("Modo: MANTENIM.", "Sensores OFF");
  }
  else if (cmd.equalsIgnoreCase("estado")) {
    refrescarPantallaBase();
  }
  else if (cmd.equalsIgnoreCase("reporte")) {
    // Formatea los contadores en ambas lineas
    String linea1 = "G:" + String(contGrande) + " P:" + String(contPequeno);
    String linea2 = "R:" + String(contRechazo) + " T:" + String(contGrande+contPequeno+contRechazo) + "   H:" + String(totalHistorico); 
    actualizarLCD(linea1, linea2);
  }
  else if (cmd.startsWith("modo_lote:")) {
    // Extrae el numero deseado del comando de lote
    String strNum = cmd.substring(10);
    objetivoLote = strNum.toInt();
    
    if (objetivoLote > 0) {
      modoActual = LOTE;
      procesadosLote = 0; 
      digitalWrite(pinRelayMotor, LOW); // Encender banda
      actualizarLCD("Lote: " + String(objetivoLote) + " paq", "Banda: ON");
    } else {
      actualizarLCD("ERROR:", "Param. invalido");
    }
  }
  else if (cmd.equalsIgnoreCase("resetear")) {
    // Pone en cero los contadores de turno (Historico se mantiene intacto)
    contGrande = 0;
    contPequeno = 0;
    contRechazo = 0;
    
    // Aplica el reseteo a la memoria permanente
    EEPROM.put(DIR_GRANDE, contGrande);
    EEPROM.put(DIR_PEQUENO, contPequeno);
    EEPROM.put(DIR_RECHAZO, contRechazo);
    
    actualizarLCD("Reset OK", "Nuevo turno listo");
  }
  else {
    // Manejo de comandos desconocidos
    actualizarLCD("ERROR:", "Cmd invalido");
  }
}


// ============================================================
// FUNCION refrescarPantallaBase()
// ============================================================
// Reestablece el texto de la pantalla LCD segun el modo actual
// despues de mostrar mensajes temporales (como "Desviando...").
// ============================================================

void refrescarPantallaBase() {
  if (modoActual == NORMAL) actualizarLCD("Modo: NORMAL", "Banda: ON");
  else if (modoActual == PAUSADO) actualizarLCD("PAUSADO", "Banda: OFF");
  else if (modoActual == DETENIDO) actualizarLCD("SmartSort GT", "Esperando cmd...");
  else if (modoActual == MANTENIMIENTO) actualizarLCD("Modo: MANTENIM.", "Sensores OFF");
  else if (modoActual == LOTE) actualizarLCD("Lote: " + String(objetivoLote) + " paq", "Procesando...");
}


// ============================================================
// FUNCION actualizarLCD()
// ============================================================
// Simplifica la escritura en la pantalla LCD. Limpia los datos
// antiguos y escribe dos lineas nuevas de forma simultanea.
// ============================================================

void actualizarLCD(String linea1, String linea2) {
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print(linea1);
  
  lcd.setCursor(0, 1);
  lcd.print(linea2);
}
```

### 13.1. Observaciones sobre el código

Antes de considerar el programa como versión definitiva, se recomienda verificar los siguientes aspectos:

1. **Clasificación de paquetes:** la lógica actual considera pequeño al objeto que interrumpe únicamente el sensor inferior y grande al que interrumpe ambos sensores. La disposición física debe producir exactamente esos estados.

2. **Modo por lote:** el programa convierte el valor recibido mediante `toInt()`, pero no valida que sea mayor que cero ni que el comando contenga un número válido.

3. **Modo normal:** el comando `modo_normal` cambia la variable `modoActual`, pero no activa por sí solo la banda. La activación se realiza mediante el comando `iniciar` en el código actual.

4. **Persistencia EEPROM:** la validación de inicio revisa únicamente el contador de paquetes grandes. Se recomienda validar también los demás valores y establecer un mecanismo confiable de inicialización.

5. **Buzzer:** el código genera una señal de 100 ms para el rechazo. Si se requieren tres sonidos separados, es necesario implementar esa secuencia.

6. **Temporización de LED:** `tiempoLed` se actualiza al comenzar una clasificación, pero su funcionamiento debe revisarse si se esperan tiempos independientes para cada indicador.

7. **Control del motor:** el programa utiliza una salida digital. El circuito real debe incorporar el controlador de potencia necesario y comprobar que el motor se detenga de manera segura.

8. **Recepción Bluetooth:** `readStringUntil()` puede bloquear temporalmente la ejecución. Si se requiere una respuesta más rápida, conviene implementar una recepción serial no bloqueante.

Estas observaciones permiten distinguir las funciones que aparecen en el código de las mejoras que podrían incorporarse posteriormente.

## 14. Presupuesto

**Semana del 19 al 23 de octubre = Q20.00**

<p align="center">
  <img src="imagenes/presupuesto.png" alt="Presupuesto 1" width="400">
</p>

**Semana del 19 al 23 de octubre = Q195.50**

<p align="center">
  <img src="imagenes/presupuesto2.png" alt="Presupuesto 2" width="400">
</p>

**Semana del 19 al 23 de octubre = Q23.50**

<p align="center">
  <img src="imagenes/presupuesto3.png" alt="Presupuesto 3" width="400">
</p>

## 15. Evidencias fotográficas

Las siguientes secciones deben completarse con fotografías reales del prototipo y del proceso de desarrollo.

##  Evidencia fotográfica 1

<p align="center">
  <img src="imagenes/evidencia.png" alt="Presupuesto 1" width="400">
</p>

##  Evidencia fotográfica 2

<p align="center">
  <img src="imagenes/evidencia2.png" alt="Presupuesto 1" width="400">
</p>

##  Evidencia fotográfica 3

<p align="center">
  <img src="imagenes/evidencia3.png" alt="Presupuesto 1" width="400">
</p>

##  Evidencia fotográfica 4

<p align="center">
  <img src="imagenes/evidencia4.png" alt="Presupuesto 1" width="400">
</p>



## 16. Conclusiones técnicas

1. SmartSort GT permite aplicar conceptos de organización computacional mediante la integración de un microcontrolador, sensores, actuadores, comunicación serial y memoria no volátil en un sistema de clasificación automatizada, como la que requiere Cargo Rapido GT.

2. El par diodo láser y LDR, colocado a dos alturas, permite diferenciar los paquetes pequeños, grandes y de rechazo según qué haces se interrumpen. La confiabilidad depende de calibrar el umbral de lectura y de alinear correctamente los láseres con sus fotorresistencias.

3. Los servomotores desviadores permiten dirigir cada paquete a su contenedor mediante software. Para que no haya errores de desvío, los ángulos de las posiciones neutral y activa deben calibrarse, y la activación debe sincronizarse con la velocidad de la banda.

4. El módulo HC-06 permite controlar la banda y los modos de operación desde un smartphone con comandos de texto, sin depender de controles físicos.

5. La EEPROM conserva los contadores de producción y el total histórico ante reinicios o cortes de energía. Esto resuelve la falta de registro por turno que motivó el proyecto, siempre que los datos se validen antes de escribirse y de usarse.

6. Gestionar los tiempos con `millis()` en lugar de `delay()` permite que el sistema siga leyendo sensores y comandos mientras los servomotores y los indicadores están activos.

7. Un sistema de bajo costo como este permite automatizar un proceso de clasificación con registros confiables. Esto sienta una base para soluciones similares en el contexto guatemalteco.

## 17. Recomendaciones

1. Calibrar los sensores antes de cada demostración y verificar que los haces láser estén alineados con las fotorresistencias, considerando la iluminación del lugar.

2. Probar con cajas de ambos tamaños, y con objetos fuera de especificación, para comprobar que cada categoría se identifica correctamente y que los rechazos no activan ningún servomotor.

3. Ajustar los ángulos de los servomotores y el tiempo de activación según la velocidad de la banda, y comprobar que regresen a la posición neutral sin atascar los paquetes.

4. Alimentar el motor DC con un circuito de potencia apropiado (transistor o controlador), nunca directamente desde un pin del Arduino, y verificar que la alimentación alcance para el motor y los servomotores.

5. Validar que los valores estén dentro de rango antes de escribirlos en la EEPROM y al leerlos al iniciar, e incluir todos los contadores en esa validación.

6. Validar el valor de N en `modo_lote:N` para rechazar valores vacíos, negativos o iguales a cero.

7. Reemplazar la lectura con `readStringUntil()` por una recepción serial no bloqueante, para que el sistema responda de forma consistente mientras clasifica.

8. Verificar que las estadísticas se conserven al apagar y volver a encender el Arduino, y que `Resetear` ponga en cero los contadores del turno sin borrar el total histórico.

9. Alinear la lógica de clasificación del código con la del enunciado: solo el láser inferior interrumpido es paquete grande, ambos interrumpidos es pequeño y ninguno es rechazo.

## 18. Anexos

### Diagrama general de conexiones


<img src="imagenes/Diagrama.png" width="800" alt="Diagrama general del circuito">



