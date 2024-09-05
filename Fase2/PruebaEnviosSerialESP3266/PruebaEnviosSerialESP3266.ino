#include <SoftwareSerial.h>

SoftwareSerial espSerial(D6, D5);  // D6 = RX, D5 = TX

void setup() {
  Serial.begin(9600);      // Para el Monitor Serial
  espSerial.begin(9600);   // Comunicación con el Arduino Mega
}

void loop() {
  if (espSerial.available()) {
    String mensaje = espSerial.readStringUntil('\n');  // Leer datos del Arduino
    Serial.println("Mensaje recibido: " + mensaje);   // Mostrar en Monitor Serial
  }
}