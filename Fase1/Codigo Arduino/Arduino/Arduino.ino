/****************************************************************************
Universidad de San Carlos de Guatemala
Arquitectura de Computadores y Ensambladores 2 - Sección "B"
Segundo Semestre 2024
Autor: Grupo 5
Proyecto: Control de Acceso Vehicular
Codigo escrito para: ARDUINO UNO
*****************************************************************************/
#include <EEPROM.h>
#include <DHT.h>
#include <LiquidCrystal.h>

// Inicializando la libreria 
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Definiendo el inicio de las direcciones de memoria EEPROM para guardar los datos de los sensores
#define EEPROM_START_TEMPERATURA 0
#define EEPROM_START_HUMEDAD 10
#define EEPROM_START_LUZ 20
#define EEPROM_START_CO2 30
#define EEPROM_START_PROXIMIDAD 40

// Definiendo el pin digital donde se conecta el sensor 
#define DHT_PIN 9
// Definiendo el tipo de senso a utilizar
#define DHT_TYPE DHT11

// Configuramos los pines del boton
const byte interruptPin = 2;
volatile bool ledState = LOW;

//Configuramos los pines del sensor Trigger y Echo
const byte triggerPin = 13;
const byte echoPin = 12;

void setup() {
  // Inicialización del sensor DHT11
  DHT dht(DHT_PIN, DHT_TYPE);
  Serial.begin(9600);
  lcd.begin(16,2);
  dht.begin();
/*  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(interruptPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), isrDesplegarLCD, RISING); 
*/
  // El pin Trigger en modo salida
//  pinMode(triggerPin,OUTPUT);
  // El pin Echo en modo entrada
//  pinMode(echoPin,INPUT);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("***Bienvenido***");
  lcd.setCursor(0,1);
  lcd.print("Fase 1 - Grupo 5");  
  delay(2000);

  float temperatura = obtenerTemperatura();
  float humedad = obtenerHumedad();
  iniciarTrigger();

  if (isnan(temperatura)) {
    guardarDTH11(temperatura, EEPROM_START_TEMPERATURA);
  }
  if (isnan(humedad)) {
    guardarDTH11(humedad, EEPROM_START_HUMEDAD);
  }
//  leerDTH11();

}

float obtenerHumedad(){
  // Obteniendo el vaor de la humedad relativa
  float humedad = random(1,30);//dht.readHumidity();

  // Comprobando si ha habido algun error en la lectura
  if(isnan(humedad)){
    Serial.println("No se logro obtener datos de HUMEDAD del sensor DHT11");
  }
  else {
    Serial.print("Humedad: ");
    Serial.print(humedad);
    Serial.println("%");
    //Debe haber una condición para saber si se debe desplegar en la pantalla LCD
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Humedad:");
    lcd.setCursor(0,1);
    lcd.print(humedad);
    lcd.print("%");
    delay(1000);
  }
  return humedad;
}

float obtenerTemperatura(){
  // Obteniendo el valor de la temperatura en grados centigrados
  float temperatura = random(1,100);//dht.readTemperature();

  // Comprobando si ha habido algun error en la lectura
  if(isnan(temperatura)){
    Serial.println("No se logro obtener datos de TEMPERATURA del sensor DHT11");
    //Debe haber una condición para saber si se debe desplegar en la pantalla LCD
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ERROR al leer:");
    lcd.setCursor(0,1);
    lcd.print("TEMPERATURA:");
  }
  else {
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println("°C");
    //Debe haber una condición para saber si se debe desplegar en la pantalla LCD
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temperatura:");
    lcd.setCursor(0,1);
    lcd.print(temperatura);
    lcd.print("C");
    delay(1000);
  }
  return temperatura;
}


void guardarDTH11(float valor, byte posMemoria){
  EEPROM.put(posMemoria, valor);  //Escribir (direccion_inicial, valor)
}

void leerDTH11(){
  float tempMemoria;
  float humedadMemoria;

  EEPROM.get(EEPROM_START_TEMPERATURA, tempMemoria);  //Obtener (direccion_inicial, var_almacenar)
  Serial.println(tempMemoria);
  EEPROM.get(EEPROM_START_HUMEDAD, humedadMemoria);  //Obtener (direccion_inicial, var_almacenar)
  Serial.println(humedadMemoria);
  delay(100);
}

void isrDesplegarLCD(){
  ledState = !ledState;
  digitalWrite(LED_BUILTIN, ledState);
  Serial.println("Interrupcion Activada");
  delay(2000);
}

void iniciarTrigger(){
  int duracion;
  int distancia;
  // Establecer tiempo de envio y recepcion - Trigger
/*  
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(1);
  
  // Obtener respuesta - Eco
  duracion = pulseIn(echoPin, HIGH);
*/
  distancia = random(1,100);//duracion/58.2; // Conversion a centimetros
  if (distancia < 45){
    Serial.print("Ditancia: ");
    Serial.print(distancia);
    Serial.println("cm");
    //Debe haber una condición para saber si se debe desplegar en la pantalla LCD
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Distancia:");
    lcd.setCursor(0,1);
    lcd.print(distancia);
    lcd.print("cm");
    delay(1000);   
  }
}