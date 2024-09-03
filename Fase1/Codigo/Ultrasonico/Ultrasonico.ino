const int Trigger = 45;   
const int Echo = 47;      
const int LedRojo = 53;   
const int LedVerde = 51;  

unsigned long tiempoInicioRojo = 0;  // Variable para almacenar el tiempo cuando se enciende el rojo
bool rojoEncendido = false;          // Indicador de si el LED rojo está encendido
bool cambiarAAmarillo = false;       // Indicador para cambiar a amarillo después de los 5 segundos

void setup() {
  Serial.begin(9600); 
  pinMode(Trigger, OUTPUT);  // Pin como salida para el Trigger
  pinMode(Echo, INPUT);      // Pin como entrada para el Echo
  pinMode(LedRojo, OUTPUT);  // Pin como salida para el color rojo
  pinMode(LedVerde, OUTPUT); // Pin como salida para el color verde
  digitalWrite(Trigger, LOW); // Inicializamos el Trigger en bajo
}

void loop() {
  ultrasonico();
}

void ultrasonico() {
  long t; // Tiempo que demora en llegar el eco
  long d; // Distancia en centímetros

  // Enviar pulso de Trigger
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10); // Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH); // Obtenemos el ancho del pulso
  d = t / 59;              // Escalamos el tiempo a una distancia en cm

  // Comportamiento cuando la distancia es <= 5 cm
  if (d <= 5 && !rojoEncendido && !cambiarAAmarillo) {
    // Si el LED rojo no está encendido y aún no ha cambiado a amarillo
    digitalWrite(LedRojo, HIGH);
    digitalWrite(LedVerde, LOW); // Apagar verde
    tiempoInicioRojo = millis(); // Guardamos el tiempo de inicio
    rojoEncendido = true;        // Indicamos que el rojo está encendido
  }

  // Verificar si han pasado 5 segundos desde que el LED rojo se encendió
  if (rojoEncendido && millis() - tiempoInicioRojo >= 5000) {
    // Después de 5 segundos, cambiar a amarillo (rojo + verde)
    digitalWrite(LedRojo, HIGH);
    digitalWrite(LedVerde, HIGH); // Encender verde para obtener amarillo
    cambiarAAmarillo = true;      // Indicamos que ya se cambió a amarillo
    rojoEncendido = false;        // Apagamos el estado de espera de rojo
  }

  // Si la distancia está entre 5 y 10 cm o si ya hemos cambiado a amarillo después de <= 5 cm
  if ((d > 5 && d <= 10) || cambiarAAmarillo) {
    digitalWrite(LedRojo, HIGH);
    digitalWrite(LedVerde, HIGH); // Mantener el amarillo encendido
  }

  // Si la distancia es mayor a 10 cm, apagar todos los colores y reiniciar estado
  if (d > 10) {
    digitalWrite(LedRojo, LOW);
    digitalWrite(LedVerde, LOW);
    cambiarAAmarillo = false;   // Reiniciar estado de cambio a amarillo
    rojoEncendido = false;      // Reiniciar estado del rojo
  }

  delay(100); // Pausa de 100 ms para evitar lecturas muy frecuentes
}


