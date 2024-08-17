#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <EEPROM.h>

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

float CO2[3] = {2.3, 0.53, -0.44};                                                    
float Ro = 10;    


//colores

#define S0 8
#define S1 9
#define S2 12
#define S3 11
#define sensorSalida 10
int frecuenciaRojo = 0;
int frecuenciaVerde = 0;
int frecuenciaAzul = 0;
int Rojo_Frec = 0;
int Verde_Frec = 0;
int Azul_Frec = 0;
int colorRojo;
int colorVerde;
int colorAzul;



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

  //colores
  //Defininiendo las salidas
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  //Definiendo salidaSensor como entrada
  pinMode(sensorSalida, INPUT);
  
  // Configura la escala de Frecuencia en 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

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

    leerDHT11();    

    ppmCO2 = ObtenerPorcentajeGas(LeerSensor(PIN_MQ)/Ro, GAS_CO2);
    valorLuz = SensorCantidadLuz();
    valorInfrarrojo = Infrarrojo();
    //Colores();

    Serial.print(humedad); // simulando el de humendad
    Serial.print(",");  
    Serial.print(temperatura); //simulando el de temperatura
    Serial.print(",");
    Serial.print(ppmCO2);  //salida del sensor de CO2
    Serial.print(",");
    Serial.print(valorLuz); 
    Serial.print(",");
    Serial.println(valorInfrarrojo);
    
    /*Serial.print(",");
    Serial.print(rojoGlobal);
    Serial.print(",");
    Serial.print(verdeGlobal);
    Serial.print(",");
    Serial.println(azulGlobal);*/

    //para agregar más sensores es necesario agregar más líneas como la anterior
    /*
        Serial.print(",");
        Serial.println(NUEVO SENSOR);
    */

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


void Colores(){

    // Configura el filtor ROJO para tomar lectura
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);

  //Rojo_Frec= pulseIn(sensorSalida, LOW);
  frecuenciaRojo = pulseIn(sensorSalida,LOW);
  colorRojo = map(frecuenciaRojo,170,1115,255,0);

  // Configura el filtor VERDE para tomar lectura
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  //Verde_Frec = pulseIn(sensorSalida, LOW);
  frecuenciaVerde = pulseIn(sensorSalida, LOW);
  colorVerde = map(frecuenciaVerde,100,199,255,0);
  // Configura el filtor AZUL para tomar lectura
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);

//Azul_Frec = pulseIn(sensorSalida, LOW);
  frecuenciaAzul = pulseIn(sensorSalida, LOW);
  colorAzul = map(frecuenciaVerde,38,84,255,0);

  //return colorRojo, colorVerde, colorAzul;
  rojoGlobal = colorRojo;
  verdeGlobal = colorVerde;
  azulGlobal = colorAzul;

}


