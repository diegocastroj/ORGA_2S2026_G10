# Organización Computacional

## Práctica 2 - LogicCalc

**Estudiante:** Diego José Castro Lux - 202300627

**Estudiante:** Ludsvin Anthony Marcelino Cuxum Cahuec - 202300497

**Estudiante:** Jonathan Eduardo Fuentes Garcia - 202408977

**Estudiante:** Andre Emanuel Negreros Pacaya - 202401461

## Introducción

La presente práctica consiste en el diseño e implementación de **LogicCalc**, una calculadora basada en lógica combinacional que permite realizar diferentes operaciones aritméticas y lógicas utilizando circuitos digitales.

El sistema trabaja con dos operandos de 4 bits, denominados **A** y **B**, y utiliza tres bits de control para seleccionar la operación que se desea realizar. Entre las operaciones disponibles se encuentran suma, resta, multiplicación, potencia, AND, OR, NAND y XNOR.

Además, el sistema incorpora un comparador que permite determinar cuál de los dos operandos es mayor o menor, así como diferentes mecanismos de visualización mediante displays de 7 segmentos y LEDs.

La implementación se realiza mediante circuitos integrados de lógica digital, sumadores, comparadores, multiplexores, demultiplexores, compuertas lógicas y elementos de visualización, utilizando **Proteus** para la simulación y posteriormente realizando el montaje físico del circuito.

## Simulación en Proteus

A continuación, se presenta la simulación completa del circuito de LogicCalc en Proteus.

<img src="assets/proteus.png" width="600">

## Funcionamiento de LogicCalc

LogicCalc recibe dos operandos de 4 bits:

* Operando A
* Operando B

La operación que se ejecutará se selecciona mediante tres bits de control:

| C | B | A | Operación      |
| - | - | - | -------------- |
| 0 | 0 | 0 | Suma           |
| 0 | 0 | 1 | Resta          |
| 0 | 1 | 0 | Multiplicación |
| 0 | 1 | 1 | Potencia       |
| 1 | 0 | 0 | AND            |
| 1 | 0 | 1 | OR             |
| 1 | 1 | 0 | NAND           |
| 1 | 1 | 1 | XNOR           |

De esta manera, el sistema permite seleccionar una de las ocho operaciones disponibles utilizando únicamente tres señales de control.

## Diagramas de Diseño

En esta sección se presentan los diagramas correspondientes a los diferentes módulos utilizados para construir LogicCalc.

### Entradas A y B

<img src="assets/entradas.png" width="600">

### Selector de Operaciones

<img src="assets/controlador.png" width="600">

## Operaciones Aritméticas

### Sumador

El módulo de suma permite realizar la operación:

**A + B**

Utilizando circuitos sumadores de 4 bits y considerando el acarreo generado durante la operación.

<img src="assets/suma1.png" width="600">
<img src="assets/suma2.png" width="600">
<img src="assets/suma3.png" width="600">
<img src="assets/suma4.png" width="600">
<img src="assets/suma5.png" width="600">
<img src="assets/suma6.png" width="600">


### Restador

La resta permite realizar:

**A - B**

La operación solamente es válida cuando A es mayor o igual que B. En caso contrario, el sistema debe mostrar el resultado correspondiente a un error.

<img src="assets/resta.png" width="600">



### Multiplicador

El módulo de multiplicación permite realizar la operación entre los dos operandos de 4 bits.

**A × B**

La multiplicación se implementa utilizando lógica combinacional y sumadores para obtener el resultado correspondiente.

<img src="assets/multiplicacion.png" width="600">

### Potencia

El módulo de potencia permite realizar las operaciones especificadas para el proyecto.

Cuando B = 2:

**A²**

Cuando B = 3:

**A³**

El resultado se obtiene mediante la combinación de los módulos de operación correspondientes.

<img src="assets/potencia1.png" width="600">
<img src="assets/potencia2.png" width="600">
<img src="assets/potencia3.png" width="600">
<img src="assets/potencia4.png" width="600">
<img src="assets/potencia5.png" width="600">
<img src="assets/potencia6.png" width="600">



## Operaciones Lógicas

Las operaciones lógicas trabajan bit a bit sobre los operandos A y B.

### AND

La operación AND se realiza entre cada uno de los bits correspondientes de los operandos.

<img src="assets/and.png" width="600">

### OR

La operación OR se realiza entre cada uno de los bits correspondientes de los operandos.

<img src="assets/or.png" width="600">

### NAND

La operación NAND corresponde a la negación de la operación AND.

<img src="assets/nand.png" width="600">

### XNOR

La operación XNOR permite determinar si los bits correspondientes de A y B son iguales.

<img src="assets/xor.png" width="600">

## Comparador

El módulo comparador permite determinar la relación entre los operandos A y B.

El sistema permite identificar:

* Si A es mayor que B.
* Si B es mayor que A.
* Si A y B son iguales.

El resultado de la comparación se muestra mediante los displays correspondientes.

<img src="assets/comparador.png" width="600">

## Indicadores de Operación

Para facilitar la identificación del módulo activo se utilizan indicadores luminosos.

* **Azul:** indica que se encuentra activa una operación aritmética.
* **Amarillo:** indica que se encuentra activa una operación lógica.

<img src="assets/indicadores.png" width="600">

## Visualización

Los resultados de las operaciones aritméticas y de comparación se muestran mediante displays de 7 segmentos.

Las operaciones lógicas utilizan cuatro LEDs, donde cada LED representa uno de los cuatro bits del resultado.

### Displays de 7 Segmentos

<img src="assets/resultados.png" width="600">

### LEDs de Operaciones Lógicas

<img src="assets/leds.png" width="600">

## Circuitos Integrados Utilizados

Para la implementación de LogicCalc se utilizan diferentes circuitos integrados y componentes de lógica digital.

Entre ellos se encuentran:

* 7408 - Compuertas AND.
* 7432 - Compuertas OR.
* 7404 - Compuertas NOT.
* 7486 - Compuertas XOR.
* 7483 / 74283 - Sumador de 4 bits.
* 7485 / 74285 - Comparador de magnitud.
* 74157 - Multiplexor.
* 74138 - Demultiplexor.
* 7447 / 7448 - Decodificador para display de 7 segmentos.
* 2N2222 - Transistor NPN.
* Displays de 7 segmentos.
* LEDs.

## Circuito Físico

A continuación, se presentan las fotografías del circuito físico implementado.

### Fotografía 1

<img src="assets/placa.jpeg" width="500">

### Fotografía 2

<img src="assets/armado2.jpeg" width="500">

### Fotografía 3

<img src="assets/armado3.jpeg" width="500">

### Fotografía 4

<img src="assets/armado4.jpeg" width="500">

### Fotografía 5

<img src="assets/armado5.jpeg" width="500">


## Conclusiones

La realización de la práctica permitió aplicar los conceptos de lógica combinacional y electrónica digital mediante el diseño de una calculadora basada en circuitos lógicos.

La implementación de los diferentes módulos permitió comprender la utilización de sumadores, restadores, multiplicadores, comparadores, compuertas lógicas, multiplexores y demultiplexores dentro de un sistema digital integrado.

La simulación en Proteus permitió verificar el funcionamiento de las operaciones antes de realizar el montaje físico, facilitando la identificación y corrección de posibles errores en las conexiones.

Finalmente, la práctica permitió integrar diferentes componentes de lógica digital en un solo sistema, utilizando señales de control para seleccionar las operaciones y diferentes dispositivos de salida para representar los resultados.

