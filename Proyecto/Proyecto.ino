// Permite utilizar la comunicacion I2C de la pantalla LCD.
#include <Wire.h>

// Permite controlar una pantalla LCD mediante un adaptador I2C.
#include <LiquidCrystal_I2C.h>

// Permite controlar los servomotores.
#include <Servo.h>

// Permite leer y escribir datos en la memoria EEPROM.
#include <EEPROM.h>

// Permite crear un puerto serial adicional mediante pines digitales.
#include <SoftwareSerial.h>


const int pinLDR1 = A0; // Sensor inferior
const int pinLDR2 = A1; // Sensor superior
const int pinMotor = 7;
const int pinBuzzer = 8;
const int pinLedAzul = 4;
const int pinLedVerde = 5;
const int pinLedRojo = 6;


Servo servoGrandes;
Servo servoPequenos;
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial bt(2, 3);
int pGrandes = 0;
int pPequenos = 0;
int pRechazos = 0;
long totalHistorico = 0;
String modoActual = "NORMAL";
bool bandaActiva = false;
int loteObjetivo = 0;
int loteActual = 0;


unsigned long tiempoServo = 0;
bool servoActivo = false;
unsigned long tiempoLed = 0;



void setup() {

  // Inicializa la comunicacion serial para depuracion por USB.
  Serial.begin(9600);

  // Inicializa la comunicacion con el modulo Bluetooth.
  bt.begin(9600);


  // Configura como salidas los dispositivos que reciben senales
  // de control desde el Arduino.
  pinMode(pinMotor, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinLedAzul, OUTPUT);
  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedRojo, OUTPUT);


  // Asocia cada objeto Servo con su pin de control.
  servoGrandes.attach(9);
  servoPequenos.attach(10);

  // Coloca ambos servos en su posicion neutral inicial.
  resetServos();


  // Inicializa la pantalla y activa su iluminacion.
  lcd.init();
  lcd.backlight();


  // Recupera los contadores previamente guardados.
  cargarEEPROM();


  // Enciende el LED azul para indicar que el sistema inicio.
  digitalWrite(pinLedAzul, HIGH);

  // Muestra un mensaje inicial en la pantalla.
  actualizarLCD("SmartSort GT", "Esperando cmd..");
}

void loop() {

  // Comprueba si se recibio una instruccion por Bluetooth.
  procesarBluetooth();
  if (bandaActiva && modoActual != "MANTENIM") {
    leerSensores();
  }
  // Revisa si corresponde finalizar una actuacion o apagar LED.
  gestionarTemporizadores();
}

void procesarBluetooth() {

  if (bt.available() > 0) {

    String cmd = bt.readStringUntil('\n');
    cmd.trim();


    if (cmd == "iniciar") {

      bandaActiva = true;
      digitalWrite(pinMotor, HIGH);

      modoActual = "NORMAL";

      actualizarLCD("Modo: NORMAL", "Banda: ON");


    } else if (cmd == "pausa") {

      bandaActiva = false;
      digitalWrite(pinMotor, LOW);

      actualizarLCD("PAUSADO", "Banda: OFF");

    } else if (cmd == "detener") {

      bandaActiva = false;
      digitalWrite(pinMotor, LOW);

      mostrarReporte();

    } else if (cmd == "reporte") {

      mostrarReporte();

    } else if (cmd == "Resetear") {

      pGrandes = 0;
      pPequenos = 0;
      pRechazos = 0;

      guardarEEPROM();

      actualizarLCD("Reset OK", "Nuevo turno listo");

    } else if (cmd.startsWith("modo_lote:")) {

      loteObjetivo = cmd.substring(10).toInt();

      loteActual = 0;
      modoActual = "LOTE";

      bandaActiva = true;
      digitalWrite(pinMotor, HIGH);

      actualizarLCD(
        "Lote: " + String(loteObjetivo) + " paq",
        "Banda: ON"
      );

    } else if (cmd == "modo_normal") {

      modoActual = "NORMAL";

      actualizarLCD("Modo: NORMAL", "Esperando...");

    } else if (cmd == "modo_mantenimiento") {

      modoActual = "MANTENIM";

      bandaActiva = true;
      digitalWrite(pinMotor, HIGH);

      actualizarLCD("Modo: MANTENIM.", "Banda: ON");

    } else if (cmd == "estado") {

      actualizarLCD(
        "Modo: " + modoActual,
        bandaActiva ? "Banda: ON" : "Banda: OFF"
      );

    } else {

      actualizarLCD("ERROR:", "Cmd invalido");
    }
  }
}


void leerSensores() {


  if (servoActivo) return;

  int luz1 = analogRead(pinLDR1);
  int luz2 = analogRead(pinLDR2);

  int umbral = 800;
  bool l1_corte = (luz1 < umbral);
  bool l2_corte = (luz2 < umbral);


  if (l1_corte || l2_corte) {

    if (l1_corte && !l2_corte) {

      clasificarPaquete(1); // Pequeno

    } else if (l1_corte && l2_corte) {

      clasificarPaquete(2); // Grande

    } else {

      clasificarPaquete(3); // Rechazo
    }
  }
}

void clasificarPaquete(int tipo) {

  // Marca el inicio de una secuencia de actuacion.
  servoActivo = true;

  // Guarda el instante inicial para medir el tiempo de actuacion.
  tiempoServo = millis();

  // Guarda el instante de referencia para los indicadores.
  tiempoLed = millis();

  if (tipo == 1) {

    // Incrementa el contador de paquetes pequenos.
    pPequenos++;

    // Ordena el movimiento del servo para pequenos.
    servoPequenos.write(45);

    // Enciende el LED de clasificacion correcta.
    digitalWrite(pinLedVerde, HIGH);

    // Emite una senal sonora breve.
    tone(pinBuzzer, 1500, 200);

    // Informa al operador sobre la clasificacion.
    actualizarLCD("Paq: PEQUENO", "Desviando S2");

  } else if (tipo == 2) {

    pGrandes++;
    servoGrandes.write(45);
    digitalWrite(pinLedVerde, HIGH);
    tone(pinBuzzer, 1000, 200);
    actualizarLCD("Paq: GRANDE", "Desviando S1");

  } else {

    pRechazos++;
    digitalWrite(pinLedRojo, HIGH);
    tone(pinBuzzer, 500, 100);
    actualizarLCD("Paq: RECHAZO", "Pasa libre");
  }

  totalHistorico++;
  guardarEEPROM();

  if (modoActual == "LOTE") {

    loteActual++;
    if (loteActual >= loteObjetivo) {

      bandaActiva = false;
      digitalWrite(pinMotor, LOW);
      actualizarLCD(
        "Lote completo",
        "Procesados: " + String(loteActual)
      );
      tone(pinBuzzer, 800, 1000);
      modoActual = "NORMAL";
    }
  }
}



void gestionarTemporizadores() {

  if (servoActivo && (millis() - tiempoServo >= 2000)) {

    resetServos();

    servoActivo = false;
  }

  if (millis() - tiempoLed > 500) {

    digitalWrite(pinLedVerde, LOW);
    digitalWrite(pinLedRojo, LOW);
  }
}


void resetServos() {

  servoGrandes.write(90);
  servoPequenos.write(90);
}


void actualizarLCD(String l1, String l2) {


  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(l1);

  lcd.setCursor(0, 1);
  lcd.print(l2);
}



void cargarEEPROM() {

  // Lee los paquetes grandes desde la direccion 0.
  EEPROM.get(0, pGrandes);

  // Lee los paquetes pequenos desde la direccion 2.
  EEPROM.get(2, pPequenos);

  // Lee los paquetes rechazados desde la direccion 4.
  EEPROM.get(4, pRechazos);

  // Lee el total historico desde la direccion 6.
  EEPROM.get(6, totalHistorico);

  if (pGrandes == -1 || pGrandes > 30000) {

    // Reinicia los contadores si se cumple la condicion.
    pGrandes = 0;
    pPequenos = 0;
    pRechazos = 0;
    totalHistorico = 0;

    // Guarda los valores iniciales.
    guardarEEPROM();
  }
}


void guardarEEPROM() {

  // Almacena los paquetes grandes en la direccion 0.
  EEPROM.put(0, pGrandes);

  // Almacena los paquetes pequenos en la direccion 2.
  EEPROM.put(2, pPequenos);

  // Almacena los paquetes rechazados en la direccion 4.
  EEPROM.put(4, pRechazos);

  // Almacena el total historico en la direccion 6.
  EEPROM.put(6, totalHistorico);
}

void mostrarReporte() {

  // Primera fila: cantidades por categoria.
  String r1 = "G:" + String(pGrandes) +
              " P:" + String(pPequenos) +
              " R:" + String(pRechazos);

  // Segunda fila: acumulado historico.
  String r2 = "Total:" + String(totalHistorico);

  // Muestra el reporte en la pantalla.
  actualizarLCD(r1, r2);
}
