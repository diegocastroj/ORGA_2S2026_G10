// Pines de Entrada
const int pinHabilitar = 2; // Salida A=B del comparador 7485

// Pines de Salida hacia el Puente H y LEDs
const int pinMotorDir1 = 3; // Giro 1 + LED Verde
const int pinMotorDir2 = 4; // Giro 2 + LED Rojo

// Pines de Control para los Contadores Físicos
const int pinClockOut  = 9; // Pulsos de Reloj para los Flip-Flops
const int pinResetFF   = 8; // Reset para cargar/limpiar contadores

bool secuenciaEjecutada = false;

// Función para emitir un pulso de reloj cuadrado de 1 segundo (1 Hz)
void emitirPulsoReloj() {
  digitalWrite(pinClockOut, HIGH);
  delay(500); // 500 ms en ALTO
  digitalWrite(pinClockOut, LOW);
  delay(500); // 500 ms en BAJO
}

void setup() {
  pinMode(pinHabilitar, INPUT);
  
  pinMode(pinMotorDir1, OUTPUT);
  pinMode(pinMotorDir2, OUTPUT);
  pinMode(pinClockOut, OUTPUT);
  pinMode(pinResetFF, OUTPUT);

  // Estado inicial deshabilitado
  digitalWrite(pinMotorDir1, LOW);
  digitalWrite(pinMotorDir2, LOW);
  digitalWrite(pinClockOut, LOW);
  digitalWrite(pinResetFF, LOW);
}

void loop() {
  int claveValida = digitalRead(pinHabilitar);

  if (claveValida == HIGH && !secuenciaEjecutada) {
    
    // -----------------------------------------------------------
    // FASE 1: 15 Segundos - Giro 1 / LED Verde / Contador Ascendente
    // -----------------------------------------------------------
    digitalWrite(pinMotorDir2, LOW);
    digitalWrite(pinMotorDir1, HIGH); // Activa motor Dir 1 y LED Verde
    
    // Genera 15 pulsos de reloj hacia los Flip-Flops externos
    for (int i = 0; i < 15; i++) {
      emitirPulsoReloj();
    }

    // Pausa breve de seguridad entre giros
    digitalWrite(pinMotorDir1, LOW);
    delay(500);

    // -----------------------------------------------------------
    // FASE 2: 10 Segundos - Giro 2 / LED Rojo / Contador Descendente
    // -----------------------------------------------------------
    digitalWrite(pinMotorDir2, HIGH); // Activa motor Dir 2 y LED Rojo
    
    // Genera 10 pulsos de reloj hacia los Flip-Flops externos
    for (int i = 0; i < 10; i++) {
      emitirPulsoReloj();
    }

    // -----------------------------------------------------------
    // FASE 3: Fin del ciclo
    // -----------------------------------------------------------
    digitalWrite(pinMotorDir2, LOW); // Apaga motor y LED Rojo
    secuenciaEjecutada = true;
  }

  // Si la contraseña se resetea o borra, permite volver a ejecutar
  if (claveValida == LOW) {
    secuenciaEjecutada = false;
  }
}