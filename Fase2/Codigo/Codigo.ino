#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <EEPROM.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

const int ledCalibracion = 13;                      
const int PIN_MQ = A0;                                
int VALOR_RL = 1;                                     
float FACTOR_AIRE_LIMPIO_RO = 9.75;   
int ldrPin = A1;                      // Pin LDR                  
const int pinInfrarrojo = 7;

int TIEMPOS_MUESTRA_CALIBRACION = 10;                  
int INTERVALO_MUESTRA_CALIBRACION = 100;               
int INTERVALO_MUESTRA_LECTURA = 50;                    
int TIEMPOS_MUESTRA_LECTURA = 5;                      

#define GAS_CO2 2    
#define DHTPIN 5       // Pin donde está conectado el sensor DHT11
#define DHTTYPE DHT11  // Tipo de sensor DHT

DHT dht(DHTPIN, DHTTYPE);
float humedad = 0;
float temperatura = 0;


// Asegúrate de usar la dirección correcta encontrada por el escáner I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); 

const int buttonPin1 = 3; // Pin al que está conectado el primer botón
const int buttonPin2 = 2; // Pin al que está conectado el segundo botón
const int buttonPin3 = 4; // Pin al que está conectado el segundo botón

int Opcion = 0;
int Opcion2 = 0;
int OpcionANT = 0;

long ppmCO2 = 0;
int valorLuz = 0;
int valorInfrarrojo = 0;
int rojoGlobal = 0;
int azulGlobal = 0;
int verdeGlobal = 0;
int valorDistancia = 0;

float CO2[3] = {2.3, 0.53, -0.44};                                                    
float Ro = 10;    


// ultrasónico y leds parqueo
// const int Trigger = 8;   
// const int Echo = 9;      
// const int LedRojo = 10;   
// const int LedVerde = 11;  

const int Trigger = 45;   
const int Echo = 47;      
const int LedRojo = 53;   
const int LedVerde = 49;  

unsigned long tiempoInicioRojo = 0;  // Variable para almacenar el tiempo cuando se enciende el rojo
bool rojoEncendido = false;          // Indicador de si el LED rojo está encendido
bool cambiarAAmarillo = false;       // Indicador para cambiar a amarillo después de los 5 segundos

// ===== Talanquera ======
Servo myServo;            
int irSensor = 6;                      // Pin infrarrojo (modificar existe uno)
int servoPin = 12;                     // Pin servomotor
int tarjetaPin = 22;                   // Pin para simular la tarjeta RFID con un interruptor :) borrar luego
bool barreraAbierta = false;           // Estado inicial de la barrera: cerrada

// ===== modulo rfid =====
#define SS_PIN 10  // pines temporales estan repetidos pero como vamos a cambiar de arduino se queda si de momento.
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

// Declarar una lista de NUIDs conocidos
byte NUIDsConocidos[][4] = {
  {0xA1, 0xB2, 0xC3, 0xD4},
  {0x01, 0x02, 0x03, 0x04},
  {0xFF, 0xEE, 0xDD, 0xCC}
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
  Serial.println("Calibrando...");

  Ro = CalibrarSensor(PIN_MQ);                         
  digitalWrite(ledCalibracion, LOW);              

  Serial.println(Ro);

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);

  //infrarrojo
  pinMode(pinInfrarrojo, INPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin1), boton1, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin2), boton2, FALLING);

  //ultrasónico y leds parqueo
  pinMode(Trigger, OUTPUT);  // Pin como salida para el Trigger
  pinMode(Echo, INPUT);      // Pin como entrada para el Echo
  pinMode(LedRojo, OUTPUT);  // Pin como salida para el color rojo
  pinMode(LedVerde, OUTPUT); // Pin como salida para el color verde
  digitalWrite(Trigger, LOW); // Inicializamos el Trigger en bajo

  // ===== Talanquera =====         
    myServo.attach(servoPin);     //  servomotor en el pin 12
    myServo.write(0);             // posición inicial de 0grados (cerrado)
    pinMode(irSensor, INPUT);     // pin del sensor infrarrojo como entrada
    pinMode(tarjetaPin, INPUT);   // pin para la simular de la tarjeta RFID ,borrar luego :)

  // ===== Lector rfid =====
  SPI.begin();
  rfid.PCD_Init();
  // pinMode(pinSalida, OUTPUT); // Configuramos el pin como salida TDODAS ESTAS LINEAS ESTAN PENDIENTES DE VER SI SON NECESARIAS.
  // digitalWrite(pinSalida, LOW); // Inicialmente apagado
  // Serial.println(F("Esperando tarjeta RFID..."));

  delay(1000);
}

void boton1(){
  Opcion = 1;
}

void boton2(){
  Opcion = 2;
}

void loop() { 
    ppmCO2 = 0;
    valorLuz = 0;
    valorInfrarrojo = 0;
    valorDistancia = 0;

    leerDHT11();   

    valorDistancia = distancia();

    ultrasonico(valorDistancia);

    // ===== Talanquera =====  
    controlarBarrera();  // funcion para controlar la barrera

    ppmCO2 = ObtenerPorcentajeGas(LeerSensor(PIN_MQ)/Ro, GAS_CO2);
    valorLuz = SensorCantidadLuz();
    valorInfrarrojo = Infrarrojo();

    /*  NO DESCOMENTAR
    Serial.print(humedad); // simulando el de humendad
    Serial.print(",");  
    Serial.print(temperatura); //simulando el de temperatura
    Serial.print(",");
    Serial.print(ppmCO2);  //salida del sensor de CO2
    Serial.print(",");
    Serial.print(valorLuz); 
    Serial.print(",");
    Serial.println(valorInfrarrojo);
    */

  //formateo de las salidas como json para enviar al esp8266, para que este lo envie al servidor
  /*
  String jsonData = "{\"Humedad\":" + String(humedad) + ",\"Temperatura\":" + String(temperatura) +
                    ",\"PPMCO2\":" + String(ppmCO2) + ",\"Luz\":" + String(valorLuz) +
                    ",\"Infrarrojo\":" + String(valorInfrarrojo)+"}";
  */

  //PRUEBAS CON DATOS QUEMADOS:
  String jsonData = "[{\"device_src\":\"HUMEDAD\", \"val\":\"" + String(50) + "\"}, " +
                  "{\"device_src\":\"TEMPERATURA\", \"val\":\"" + String(22) + "\"}, " +
                  "{\"device_src\":\"GAS\", \"val\":\"" + String(550) + "\"},"+
                  "{\"device_src\":\"LUZ\", \"val\":\"" + String(550) + "\"},"+
                  "{\"device_src\":\"INFRARROJO\", \"val\":\"" + String(550) + "\"},"+
                  "{\"device_src\":\"DISTANCIA\", \"val\":\"" + String(valorDistancia) + "\"},"+"]";

  Serial.println(jsonData);

  leerTarjetaRFID();  // Llamamos a la función que lee la tarjeta continuamente
  pantallaLCD();

  delay(1000);
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
int SensorCantidadLuz() {
  int valorLuz = analogRead(ldrPin);  // Lee el valor analógico del LDR

  return valorLuz;
}

//dht
void leerDHT11() {
  float humedad2 = humedad;
  float temperatura2 = temperatura;
  humedad = dht.readHumidity();
  temperatura = dht.readTemperature();

  // Verificar si hay errores al leer el sensor
  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println("Error al leer el sensor DHT11");
    humedad = humedad2;
    temperatura = temperatura2;
  }
}

// LCD
void pantallaLCD (){
    // Leer el estado de los botones
  int buttonState1 = digitalRead(buttonPin1);
  int buttonState2 = digitalRead(buttonPin2);
  int buttonState3 = digitalRead(buttonPin3);

  // Verificar si el primer botón está presionado
  if (buttonState3 == HIGH) {
    Opcion = 3;
  }

  switch (Opcion) {
    case 1:
      lcdB1();
      break;
    case 2:
      lcdB2();
      break;
    case 3:
      lcdB3();
      break;
    default:
      if (OpcionANT == 0){
        lcdInicio();  // Mensaje de bienvenida en pantalla lcd y espera 2s
      }
      OpcionANT = 1;
      break;
  }
}


void lcdInicio(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("***Bienvenido***");
  lcd.setCursor(0,1);
  lcd.print("Fase 1 - Grupo 5");  
}

void lcdB1(){
  lcd.clear();
  lcd.setCursor(0,0);

  if (Opcion2 == 0){
    lcd.print("Humed | Temp");
    lcd.setCursor(0,1);
    lcd.print(String(humedad) + " | " + String(temperatura));  
    Opcion2 = 1;
  }else if(Opcion2 == 1){
    lcd.print("Luz   | Gas");
    lcd.setCursor(0,1);
    String mensaje = String(valorLuz);

    // Agregar espacios hasta que la longitud sea 3
    while (mensaje.length() < 6) {
    mensaje += " ";
    }
    lcd.print( mensaje + "| " + String(ppmCO2));  
    Opcion2 =2;
  }else if(Opcion2 == 2){
    lcd.print("Infrarrojo");
    lcd.setCursor(0,1);
    if (valorInfrarrojo == 0){
      lcd.print("Obstaculo");
    }else{
      lcd.print("Libre");
    }
    Opcion2 = 0;
  }
}

void lcdB2(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("     Valores");
  lcd.setCursor(0,1);
  lcd.print("    Guardados ");

  byte* bytePointer;

  bytePointer = (byte*)(void*)&humedad; // Castea el float a un array de bytes
  for (int i = 0; i < sizeof(humedad); i++) {
    EEPROM.write(0 + i, bytePointer[i]);
  }

  bytePointer = (byte*)(void*)&temperatura; // Castea el float a un array de bytes
  for (int i = 0; i < sizeof(temperatura); i++) {
    EEPROM.write(100 + i, bytePointer[i]);
  }

  bytePointer = (byte*)(void*)&valorLuz; // Castea el int a un array de bytes
  for (int i = 0; i < sizeof(valorLuz); i++) {
    EEPROM.write(200 + i, bytePointer[i]);
  }

  bytePointer = (byte*)(void*)&ppmCO2; // Castea el long a un array de bytes
  for (int i = 0; i < sizeof(ppmCO2); i++) {
    EEPROM.write(300 + i, bytePointer[i]);
  }
  //infrarrojo en la eeprom
  bytePointer = (byte*)(void*)&valorInfrarrojo;
  for (int i=0; i<sizeof(valorInfrarrojo);i++){
    EEPROM.write(400+i, bytePointer[i]);
  }


  Opcion = 1;
}

void lcdB3(){
  float humedad2 ;
  float temperatura2;
  long ppmCO22;
  int valorLuz2;
  int valorInfrarrojo2;

  byte* bytePointer0 = (byte*)(void*)&humedad2;
  for (int i = 0; i < sizeof(humedad2); i++) {
    bytePointer0[i] = EEPROM.read(0 + i);
  }

  byte* bytePointer1 = (byte*)(void*)&temperatura2;
  for (int i = 0; i < sizeof(temperatura2); i++) {
    bytePointer1[i] = EEPROM.read(100 + i);
  }

  byte* bytePointer2 = (byte*)(void*)&valorLuz2;
  for (int i = 0; i < sizeof(valorLuz2); i++) {
    bytePointer2[i] = EEPROM.read(200 + i);
  }

  byte* bytePointer3 = (byte*)(void*)&ppmCO22;
  for (int i = 0; i < sizeof(ppmCO22); i++) {
    bytePointer3[i] = EEPROM.read(300 + i);
  }

  byte* bytePointer4 = (byte*)(void*)&valorInfrarrojo2;
  for (int i=0; i<sizeof(valorInfrarrojo2);i++){
    bytePointer4[i] = EEPROM.read(400+i);
  }



  lcd.clear();
  lcd.setCursor(0,0);

  if (Opcion2 == 0){
    lcd.print("Humed | Temp   G");
    lcd.setCursor(0,1);
    lcd.print(String(humedad2) + " | " + String(temperatura2));  
    Opcion2 = 1;
  }else if(Opcion2 == 1){
    lcd.print("Luz   | Gas    G");
    lcd.setCursor(0,1);
    String mensaje = String(valorLuz2);

    // Agregar espacios hasta que la longitud sea 3
    while (mensaje.length() < 6) {
    mensaje += " ";
    }
    lcd.print( mensaje + "| " + String(ppmCO22));  
    Opcion2 = 2;
  }else if(Opcion2 == 2){
    lcd.print("Infrarrojo   G");
    lcd.setCursor(0,1);
    if (valorInfrarrojo2 == 0){
      lcd.print("Obstaculo");
    }else{
      lcd.print("Libre");
    }
    Opcion2 = 0;
  }
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
    if (leerTarjetaRFID() && !barreraAbierta) {  // Abre la barrera si se detecta una tarjeta valida, barrera cerrada
        abrirBarrera();                                 // abre a 90 grados
        barreraAbierta = true;                          // Marca la barrera como abierta
    }

    // Lee el estado del sensor infrarrojo
    int irValue = digitalRead(irSensor);  

    // Cierre de la barrera tras verificar que no hay obstaculos
    if (barreraAbierta && irValue == HIGH) {  
        delay(5000);                                   // Espera 5 segundos para permitir el paso completo del auto
        cerrarBarrera(); 
        barreraAbierta = false;                        // Marca la barrera como cerrada
    } else if (barreraAbierta && irValue == LOW) {
                                                       // La barrera permanece abierta si se detecta un obstaculo en el infrarojo (0)
    }
}

// Función para abrir la barrera
void abrirBarrera() {
    myServo.write(90);                                 // Mueve el servomotor a 90 grados para abrir la barrera
}

void cerrarBarrera() {
    myServo.write(0);                                  // Mueve el servomotor a 0 grados para cerrar la barrera
}

bool leerTarjetaRFID() {
  // Revisar si hay una tarjeta presente
  if (!rfid.PICC_IsNewCardPresent()) {
    return false; // Si no hay tarjeta, retornamos false
  }

  // Revisar si la tarjeta puede ser leída
  if (!rfid.PICC_ReadCardSerial()) {
    return false; // Si no puede ser leída, retornamos false
  }

  /*/ Mostrar el NUID de la tarjeta
  Serial.print(F("NUID de la tarjeta: "));
  for (byte i = 0; i < 4; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  */

  // Comparar el NUID con la lista de NUIDs conocidos
  if (tarjetaEsConocida(rfid.uid.uidByte)) {
    rfid.PICC_HaltA(); // Detener la lectura de la tarjeta
    return true;
  } else {
    rfid.PICC_HaltA(); // Detener la lectura de la tarjeta
    return false;
  }
}


// Función para comparar la tarjeta leída con la lista de NUIDs conocidos
bool tarjetaEsConocida(byte* nuidLeido) {
  for (int i = 0; i < cantidadNUIDs; i++) {
    bool coinciden = true;
    for (int j = 0; j < 4; j++) {
      if (NUIDsConocidos[i][j] != nuidLeido[j]) {
        coinciden = false;
        break; // Salir si no coinciden
      }
    }
    if (coinciden) {
      return true; // Si los 4 bytes coinciden, la tarjeta es reconocida
    }
  }
  return false; // Si ninguna coincidió, la tarjeta no es reconocida
}