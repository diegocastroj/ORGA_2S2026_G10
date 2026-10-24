#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Pines del Arduino Mega 2560
const int pinRelayMotor = 7;
const int pinLDRBajo = A0;
const int pinLDRAlto = A1;
const int pinServo1 = 8;
const int pinServo2 = 9;
const int pinLedAzul = 2;
const int pinLedVerde = 3;
const int pinLedRojo = 4;
const int pinBuzzer = 5;

const int UMBRAL_LDR = 700;

// Direcciones de almacenamiento EEPROM
const int DIR_GRANDE = 0;
const int DIR_PEQUENO = 2;
const int DIR_RECHAZO = 4;
const int DIR_TOTAL_HISTORICO = 8;

// Perifericos
Servo servo1;
Servo servo2;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Temporizadores de los servos
unsigned long tiempoActivacionS1 = 0;
unsigned long tiempoActivacionS2 = 0;
bool s1Activo = false;
bool s2Activo = false;

const unsigned long TIEMPO_RETORNO_S1 = 4500;
const unsigned long TIEMPO_RETORNO_S2 = 7500;

// Estados de deteccion
enum EstadoDeteccion {
  ESPERANDO,
  CONFIRMANDO,
  ESPERANDO_LIBERACION
};

EstadoDeteccion estadoSensor = ESPERANDO;
unsigned long tiempoInicioDeteccion = 0;
const unsigned long TIEMPO_CONFIRMACION = 150;

bool l1Detectado = false;
bool l2Detectado = false;
unsigned long tiempoRechazo = 0;
bool rechazoActivo = false;

// Control de beeps
bool beepRechazoActivo = false;
int beepsRealizados = 0;
unsigned long tiempoUltimoBeep = 0;

// Bluetooth y modos
String comandoBT = "";

enum ModoOperacion {
  DETENIDO,
  NORMAL,
  PAUSADO,
  MANTENIMIENTO,
  LOTE,
  FIN_LOTE
};

ModoOperacion modoActual = DETENIDO;

// Estadisticas
int contGrande = 0;
int contPequeno = 0;
int contRechazo = 0;
unsigned long totalHistorico = 0;
int objetivoLote = 0;
int procesadosLote = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(pinRelayMotor, OUTPUT);
  pinMode(pinLedAzul, OUTPUT);
  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedRojo, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  servo1.attach(pinServo1);
  servo2.attach(pinServo2);

  servo1.write(90);
  servo2.write(90);

  lcd.init();
  lcd.backlight();

  // Recuperar datos de EEPROM
  EEPROM.get(DIR_GRANDE, contGrande);
  EEPROM.get(DIR_PEQUENO, contPequeno);
  EEPROM.get(DIR_RECHAZO, contRechazo);
  EEPROM.get(DIR_TOTAL_HISTORICO, totalHistorico);

  if (contGrande < 0 || contGrande > 32000)
    contGrande = 0;

  if (contPequeno < 0 || contPequeno > 32000)
    contPequeno = 0;

  if (contRechazo < 0 || contRechazo > 32000)
    contRechazo = 0;

  if (totalHistorico == 0xFFFFFFFFUL)
    totalHistorico = 0;

  actualizarLCD("SmartSort GT", "Esperando cmd...");
  digitalWrite(pinLedAzul, HIGH);
  digitalWrite(pinRelayMotor, HIGH);
}

void loop() {
  unsigned long tiempoActual = millis();

  // Recibir comandos Bluetooth
  while (Serial1.available() > 0) {
    char c = Serial1.read();

    if (c == '\n' || c == '\r') {
      if (comandoBT.length() > 0) {
        ejecutarComando(comandoBT);
        comandoBT = "";
      }
    } else {
      comandoBT += c;
    }
  }

  // Leer sensores en modo normal o por lote
  if (modoActual == NORMAL || modoActual == LOTE) {
    int valorL1 = analogRead(pinLDRBajo);
    int valorL2 = analogRead(pinLDRAlto);

    bool actualL1 = (valorL1 < UMBRAL_LDR);
    bool actualL2 = (valorL2 < UMBRAL_LDR);

    if (estadoSensor == ESPERANDO) {
      if (actualL1 || actualL2) {
        estadoSensor = CONFIRMANDO;
        tiempoInicioDeteccion = tiempoActual;
        l1Detectado = actualL1;
        l2Detectado = actualL2;
      }
    }
    else if (estadoSensor == CONFIRMANDO) {
      if (actualL1) l1Detectado = true;
      if (actualL2) l2Detectado = true;

      if (tiempoActual - tiempoInicioDeteccion >= TIEMPO_CONFIRMACION) {
        int tipo = 0;

        if (l1Detectado && l2Detectado) tipo = 1;
        else if (l1Detectado && !l2Detectado) tipo = 2;
        else if (!l1Detectado && l2Detectado) tipo = 3;

        // Clasificar paquete grande
        if (tipo == 1 && !s1Activo) {
          servo1.write(45);
          digitalWrite(pinLedVerde, HIGH);
          tone(pinBuzzer, 1000, 200);
          actualizarLCD("Paq: GRANDE", "Desviando S1");

          s1Activo = true;
          tiempoActivacionS1 = tiempoActual;

          if (contGrande < 32000) contGrande++;
          if (totalHistorico < 4000000000UL) totalHistorico++;

          EEPROM.put(DIR_GRANDE, contGrande);
          EEPROM.put(DIR_TOTAL_HISTORICO, totalHistorico);
          procesadosLote++;
        }

        // Clasificar paquete pequeno
        else if (tipo == 2 && !s2Activo) {
          servo2.write(140);
          digitalWrite(pinLedVerde, HIGH);
          tone(pinBuzzer, 1500, 200);
          actualizarLCD("Paq: PEQUENO", "Desviando S2");

          s2Activo = true;
          tiempoActivacionS2 = tiempoActual;

          if (contPequeno < 32000) contPequeno++;
          if (totalHistorico < 4000000000UL) totalHistorico++;

          EEPROM.put(DIR_PEQUENO, contPequeno);
          EEPROM.put(DIR_TOTAL_HISTORICO, totalHistorico);
          procesadosLote++;
        }

        // Registrar paquete rechazado
        else if (tipo == 3) {
          digitalWrite(pinLedRojo, HIGH);
          actualizarLCD("Paq: RECHAZO", "Sin desvio");

          rechazoActivo = true;
          tiempoRechazo = tiempoActual;

          if (contRechazo < 32000) contRechazo++;
          if (totalHistorico < 4000000000UL) totalHistorico++;

          EEPROM.put(DIR_RECHAZO, contRechazo);
          EEPROM.put(DIR_TOTAL_HISTORICO, totalHistorico);
          procesadosLote++;

          beepRechazoActivo = true;
          beepsRealizados = 1;
          tiempoUltimoBeep = tiempoActual;
          tone(pinBuzzer, 500, 100);
        }

        // Comprobar si se completo el lote
        if (modoActual == LOTE &&
            procesadosLote >= objetivoLote) {
          modoActual = FIN_LOTE;
          actualizarLCD("Meta alcanzada", "Terminando...");
        }

        estadoSensor = ESPERANDO_LIBERACION;
      }
    }
    else if (estadoSensor == ESPERANDO_LIBERACION) {
      if (!actualL1 && !actualL2) {
        estadoSensor = ESPERANDO;
      }
    }
  }

  // Generar tres beeps para rechazo
  if (beepRechazoActivo) {
    if (tiempoActual - tiempoUltimoBeep >= 200) {
      if (beepsRealizados < 3) {
        tone(pinBuzzer, 500, 100);
        tiempoUltimoBeep = tiempoActual;
        beepsRealizados++;
      } else {
        beepRechazoActivo = false;
      }
    }
  }

  // Detener la banda al terminar el lote
  if (modoActual == FIN_LOTE) {
    if (!s1Activo && !s2Activo && !rechazoActivo) {
      modoActual = DETENIDO;
      digitalWrite(pinRelayMotor, HIGH);
      actualizarLCD("Lote completo", "Banda: OFF");
      tone(pinBuzzer, 2000, 1500);
    }
  }

  // Retornar servo de paquetes grandes
  if (s1Activo &&
      tiempoActual - tiempoActivacionS1 >= TIEMPO_RETORNO_S1) {
    servo1.write(90);
    digitalWrite(pinLedVerde, LOW);
    s1Activo = false;
    refrescarPantallaBase();
  }

  // Retornar servo de paquetes pequenos
  if (s2Activo &&
      tiempoActual - tiempoActivacionS2 >= TIEMPO_RETORNO_S2) {
    servo2.write(90);
    digitalWrite(pinLedVerde, LOW);
    s2Activo = false;
    refrescarPantallaBase();
  }

  // Apagar indicador de rechazo
  if (rechazoActivo &&
      tiempoActual - tiempoRechazo >= 1500) {
    digitalWrite(pinLedRojo, LOW);
    rechazoActivo = false;
    refrescarPantallaBase();
  }
}

void ejecutarComando(String cmd) {
  cmd.trim();

  if (cmd.equalsIgnoreCase("iniciar") ||
      cmd.equalsIgnoreCase("modo_normal")) {
    modoActual = NORMAL;
    digitalWrite(pinRelayMotor, LOW);
    actualizarLCD("Modo: NORMAL", "Banda: ON");
  }
  else if (cmd.equalsIgnoreCase("pausa")) {
    modoActual = PAUSADO;
    digitalWrite(pinRelayMotor, HIGH);
    actualizarLCD("PAUSADO", "Banda: OFF");
  }
  else if (cmd.equalsIgnoreCase("detener")) {
    modoActual = DETENIDO;
    digitalWrite(pinRelayMotor, HIGH);

    String rep = "G:" + String(contGrande) +
                 " P:" + String(contPequeno) +
                 " R:" + String(contRechazo) +
                 " T:" + String(contGrande +
                                 contPequeno +
                                 contRechazo);

    actualizarLCD("REPORTE FINAL", rep);
  }
  else if (cmd.equalsIgnoreCase("modo_mantenimiento")) {
    modoActual = MANTENIMIENTO;
    digitalWrite(pinRelayMotor, LOW);
    actualizarLCD("Modo: MANTENIM.", "Sensores OFF");
  }
  else if (cmd.equalsIgnoreCase("estado")) {
    refrescarPantallaBase();
  }
  else if (cmd.equalsIgnoreCase("reporte")) {
    String linea1 = "G:" + String(contGrande) +
                    " P:" + String(contPequeno);

    String linea2 = "R:" + String(contRechazo) +
                    " T:" + String(contGrande +
                                    contPequeno +
                                    contRechazo) +
                    " H:" + String(totalHistorico);

    actualizarLCD(linea1, linea2);
  }
  else if (cmd.startsWith("modo_lote:")) {
    String strNum = cmd.substring(10);
    objetivoLote = strNum.toInt();

    if (objetivoLote > 0) {
      modoActual = LOTE;
      procesadosLote = 0;
      digitalWrite(pinRelayMotor, LOW);

      actualizarLCD(
        "Lote: " + String(objetivoLote) + " paq",
        "Banda: ON"
      );
    } else {
      actualizarLCD("ERROR:", "Param. invalido");
    }
  }
  else if (cmd.equalsIgnoreCase("resetear")) {
    contGrande = 0;
    contPequeno = 0;
    contRechazo = 0;

    EEPROM.put(DIR_GRANDE, contGrande);
    EEPROM.put(DIR_PEQUENO, contPequeno);
    EEPROM.put(DIR_RECHAZO, contRechazo);

    actualizarLCD("Reset OK", "Nuevo turno listo");
  }
  else {
    actualizarLCD("ERROR:", "Cmd invalido");
  }
}

void refrescarPantallaBase() {
  if (modoActual == NORMAL)
    actualizarLCD("Modo: NORMAL", "Banda: ON");
  else if (modoActual == PAUSADO)
    actualizarLCD("PAUSADO", "Banda: OFF");
  else if (modoActual == DETENIDO)
    actualizarLCD("SmartSort GT", "Esperando cmd...");
  else if (modoActual == MANTENIMIENTO)
    actualizarLCD("Modo: MANTENIM.", "Sensores OFF");
  else if (modoActual == LOTE)
    actualizarLCD(
      "Lote: " + String(objetivoLote) + " paq",
      "Procesando..."
    );
}

void actualizarLCD(String linea1, String linea2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linea1);
  lcd.setCursor(0, 1);
  lcd.print(linea2);
}