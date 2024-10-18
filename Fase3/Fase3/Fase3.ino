#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <EEPROM.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#include <ArduinoJson.h>

const int ledCalibracion = 13;                 // led de calibracion del MQ, led no conectado (revisar)             
const int PIN_MQ = A0;                                
int VALOR_RL = 1;                                     
float FACTOR_AIRE_LIMPIO_RO = 9.75;   
int ldrPin = A1;                                // Pin LDR   (Sensor de luz)
const int pinLED = 8;                           // Pin para el LED de iluminación general             
const int pinInfrarrojo = 7;                    // Infrarrojo para la barrera

int contadorAccesosCorrectos = 0;               // +++++ Contador de accesos correctos +++++

int TIEMPOS_MUESTRA_CALIBRACION = 10;                  
int INTERVALO_MUESTRA_CALIBRACION = 100;               
int INTERVALO_MUESTRA_LECTURA = 50;                    
int TIEMPOS_MUESTRA_LECTURA = 5;                      

#define GAS_CO2 2    
#define DHTPIN 4       // Pin donde esta conectado el sensor DHT11
#define DHTTYPE DHT11  // Tipo de sensor DHT

DHT dht(DHTPIN, DHTTYPE);
float humedad = 0;
float temperatura = 0;

// ===== Modulo RFID =====
#define SS_PIN 10                                        // Pin para SS (SDA) del RFID
#define RST_PIN 9                                        // Pin para RST del RFID
MFRC522 rfid(SS_PIN, RST_PIN);                           // Instancia del lector RFID

// Configuración de la pantalla LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); 

long ppmCO2 = 0;
int valorLuz = 0;
int valorInfrarrojo = 0;
int rojoGlobal = 0;
int azulGlobal = 0;
int verdeGlobal = 0;
int valorDistancia = 0;

float CO2[3] = {2.3, 0.53, -0.44};                                                    
float Ro = 10;    

const int Trigger = 6;   
const int Echo = 5;      
const int LedRojo = 22;   
const int LedVerde = 23;  

unsigned long tiempoInicioRojo = 0;  // Variable para almacenar el tiempo cuando se enciende el rojo
bool rojoEncendido = false;          // Indicador de si el LED rojo está encendido
bool cambiarAAmarillo = false;       // Indicador para cambiar a amarillo después de los 5 segundos

// ===== Talanquera ======
Servo myServo;            
const int servoPin = 12;               // Pin del servo motor de la barrera
bool barreraAbierta = false;           // Estado inicial de la barrera: cerrada

int umbralLuz = 200;                  // Umbral para encender/apagar el LED de iluminación (calibrar)


// ===== Modulo RFID =====
byte NUIDsConocidos[][4] = { 
  {0x19, 0x7D, 0xD1, 0xB1}   // nuid (tarjeta azul)     // {0x39, 0x09, 0xFB, 0xB3},  // nuid (tarjeta blanca)
};

// Número de NUIDs que se han almacenado
int cantidadNUIDs = sizeof(NUIDsConocidos) / sizeof(NUIDsConocidos[0]);


void setup() { 
  Serial.begin(9600);

  Wire.begin();
  lcd.begin(16, 2);       // Inicializa la pantalla con 16 columnas y 2 filas
  lcd.backlight();        // Enciende la retroiluminación
  lcd.setCursor(0, 0);    // Coloca el cursor en la primera columna, primera fila

  dht.begin();
  pinMode(ledCalibracion, OUTPUT);
  digitalWrite(ledCalibracion, HIGH);                     
  //Serial.println("Calibrando...");

  Ro = CalibrarSensor(PIN_MQ);                         
  digitalWrite(ledCalibracion, LOW);              

  //Serial.println(Ro);


  // Inicialización del sensor infrarrojo 
  pinMode(pinInfrarrojo, INPUT);

  // inicializacion led iluminacion (general)
  pinMode(pinLED, OUTPUT);
  //digitalWrite(pinLED, HIGH);

  //ultrasónico y leds parqueo
  pinMode(Trigger, OUTPUT);  // Pin como salida para el Trigger
  pinMode(Echo, INPUT);      // Pin como entrada para el Echo
  pinMode(LedRojo, OUTPUT);  // Pin como salida para el color rojo
  pinMode(LedVerde, OUTPUT); // Pin como salida para el color verde
  digitalWrite(Trigger, LOW); // Inicializamos el Trigger en bajo

  // ===== Talanquera =====           Inicialización del servo      
  myServo.attach(servoPin);     //  servomotor en el pin 12
  myServo.write(180);             // posición inicial de 180grados (cerrado)

  // ===== Lector rfid =====
  SPI.begin();
  rfid.PCD_Init();
  //Serial.println(F("Esperando tarjeta RFID..."));

  //lcdInicio();
  delay(1000);
}

void loop() {
    ppmCO2 = 0;
    valorLuz = 0;
    valorInfrarrojo = 0;
    valorDistancia = 0;

    // Verificar si hay datos desde el ESP8266
    if (Serial.available()) {
      String message = Serial.readStringUntil('\n');  // Leer mensaje completo
      StaticJsonDocument<256> doc;

      // Intentar parsear el mensaje JSON
      DeserializationError error = deserializeJson(doc, message);
      if (error) {
          Serial.print("Error al parsear JSON: ");
          Serial.println(error.c_str());
          return;
      }

      // Ejecutar acciones según el topic recibido
      if (doc.containsKey("ControlServo")) {
          int servoPos = doc["ControlServo"];
          myServo.write(servoPos);
          //Serial.println("Servo movido a: " + String(servoPos));
      }

      if (doc.containsKey("LedState")) {
          int ledState = doc["LedState"];
          digitalWrite(pinLED, ledState);
          //Serial.println("LED " + String(ledState ? "Encendido" : "Apagado"));
      }

      if (doc.containsKey("NUID")) {
          const char* nuid = doc["NUID"];
          //Serial.println("NUID recibido: " + String(nuid));

          // abrir y cerrar la barrera automáticamente al recibir un NUID válido
          abrirBarrera();
          delay(1000);  // Mantener la barrera abierta por 1 segundos
          cerrarBarrera();
      }
    }

    leerDHT11();  

    leerTarjetaRFID();  // Llamamos a la función que lee la tarjeta continuamente 

    valorDistancia = distancia();
    ultrasonico(valorDistancia);

    // ===== Talanquera =====  
    controlarBarrera();  // funcion para controlar la barrera

    ppmCO2 = ObtenerPorcentajeGas(LeerSensor(PIN_MQ)/Ro, GAS_CO2);
    valorLuz = SensorCantidadLuz();
    valorInfrarrojo = Infrarrojo();

    // ===== Controlar el LED de iluminación =====
    controlarLED(valorLuz, umbralLuz);

  //formateo de las salidas como json para enviar al esp8266, para que este lo envie al servidor
    String jsonData = "{\"Humedad\":" + String(humedad) + ",\"Temperatura\":" + String(temperatura) +
                      ",\"PPMCO2\":" + String(ppmCO2) + ",\"Luz\":" + String(valorLuz) +
                      ",\"Infrarrojo\":" + String(valorInfrarrojo) + ",\"Distancia\":" + String(valorDistancia) +
                      ",\"AccesosCorrectos\":" + String(contadorAccesosCorrectos) + "}";
  
  //PRUEBAS CON DATOS QUEMADOS:
  /*String jsonData = "{\"Humedad\":" + String(100) + ",\"Temperatura\":" + String(110) +
                    ",\"PPMCO2\":" + String(120) + ",\"Luz\":" + String(130) +
                    ",\"Infrarrojo\":" + String(140)+"}";*/

  Serial.println(jsonData);
  delay(500);
  lcdInicio();
}

float CalcularResistenciaSensor(int valor_adc) {
    return (((float)VALOR_RL * (1023 - valor_adc) / valor_adc));
}

float CalibrarSensor(int pin_mq) {
    int i;
    float valor = 0;

    for (i = 0; i < TIEMPOS_MUESTRA_CALIBRACION; i++) {     
        valor += CalcularResistenciaSensor(analogRead(pin_mq));
        delay(INTERVALO_MUESTRA_CALIBRACION);
    }
    valor = valor / TIEMPOS_MUESTRA_CALIBRACION;           
    valor = valor / FACTOR_AIRE_LIMPIO_RO;                  
    return valor;                                            
}

float LeerSensor(int pin_mq) {
    int i;
    float rs = 0;

    for (i = 0; i < TIEMPOS_MUESTRA_LECTURA; i++) {
        rs += CalcularResistenciaSensor(analogRead(pin_mq));
        delay(INTERVALO_MUESTRA_LECTURA);
    }

    rs = rs / TIEMPOS_MUESTRA_LECTURA;
    return rs; 
}

long ObtenerPorcentajeGas(float rs_ro_ratio, int id_gas) {
    return CalcularPorcentaje(rs_ro_ratio, CO2);
}

long CalcularPorcentaje(float rs_ro_ratio, float *curva) {
    return (pow(10, ((log(rs_ro_ratio) - curva[1]) / curva[2]) + curva[0]));
}


//LDR
int  SensorCantidadLuz() {
  int valorLuz = analogRead(ldrPin);  // Lee el valor analógico del LDR
  return valorLuz;
}

// ====== iluminacion general ===== 
void controlarLED(int valorLuz, int umbral) {
  if (valorLuz < umbral) {
    digitalWrite(pinLED, HIGH);                         // Si no hay suficiente luz, encender el LED
  } else {
    digitalWrite(pinLED, LOW);                          // Si hay suficiente luz, apagar el LED
  }
}

//dht
void leerDHT11() {
  float humedad2 = humedad;
  float temperatura2 = temperatura;
  humedad = dht.readHumidity();
  temperatura = dht.readTemperature();

  // Verificar si hay errores al leer el sensor
  if (isnan(humedad) || isnan(temperatura)) {
    humedad = humedad2;
    temperatura = temperatura2;
  }
}

// LCD
void lcdInicio(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("***Bienvenido***");
  lcd.setCursor(0,1);
  lcd.print("Fase 3 - Grupo 5");  
}

void lcdExito() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lectura exitosa");
  lcd.setCursor(0, 1);
  lcd.print("Bienvenido");
}

void lcdFallida() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lectura fallida");
  lcd.setCursor(0, 1);
  lcd.print("Acceso denegado");
}  

int Infrarrojo(){
  int valor = digitalRead(pinInfrarrojo);
  return valor;

}


int distancia(){
  long t; // Tiempo que demora en llegar el eco
  long d; // Distancia en centímetros

  // Enviar pulso de Trigger
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10); // Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH); // Obtenemos el ancho del pulso
  d = t / 59;              // Escalamos el tiempo a una distancia en cm

  return d;
}

void ultrasonico(int d) {
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


// ===== Talanquera =====
void controlarBarrera() {
    if (barreraAbierta) {  // Abre la barrera si se detecta una tarjeta valida, barrera cerrada
        abrirBarrera(); 
        barreraAbierta = true;
    }

    // Lee el estado del sensor infrarrojo
    int irValue = digitalRead(pinInfrarrojo);  

    // Cierre de la barrera tras verificar que no hay obstaculos
    if (barreraAbierta && irValue == HIGH) {  
        delay(1000);                                  
        cerrarBarrera(); 
        delay(1000);
        barreraAbierta = false;                        // Marca la barrera como cerrada
    } else if (barreraAbierta && irValue == LOW) {
      barreraAbierta = true;                         // La barrera permanece abierta si se detecta un obstaculo en el infrarojo (0)
    }
}

// Función para abrir la barrera
void abrirBarrera() {
    myServo.write(90);                                 // Mueve el servomotor a 90 grados para abrir la barrera
}

void cerrarBarrera() {
    myServo.write(180);                                  // Mueve el servomotor a 0 grados para cerrar la barrera
}

// Función para leer tarjetas RFID
void leerTarjetaRFID() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return; // Error al leer la tarjeta
  }

  // Verificar si la tarjeta es conocida
  if (tarjetaEsConocida(rfid.uid.uidByte)) {
    lcdExito();
    abrirBarrera();
    barreraAbierta = true;
  // C:
    contadorAccesosCorrectos++;
  } else {
    lcdFallida();
  }

  rfid.PICC_HaltA();  // Detener la lectura de la tarjeta
}

// Función para verificar si el NUID es conocido
bool tarjetaEsConocida(byte* nuidLeido) {
  for (int i = 0; i < cantidadNUIDs; i++) {
    bool coinciden = true;
    for (int j = 0; j < 4; j++) {
      if (NUIDsConocidos[i][j] != nuidLeido[j]) {
        coinciden = false;
        break;
      }
    }
    if (coinciden) {
      return true;
    }
  }
  return false;
}

