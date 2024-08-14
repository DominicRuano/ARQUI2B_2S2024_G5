#include "DHT.h"
const int ledCalibracion = 13;                      
const int PIN_MQ = A0;                                
int VALOR_RL = 1;                                     
float FACTOR_AIRE_LIMPIO_RO = 9.75;   
int ldrPin = A1;                      // Pin LDR                  

int TIEMPOS_MUESTRA_CALIBRACION = 10;                  
int INTERVALO_MUESTRA_CALIBRACION = 100;               
int INTERVALO_MUESTRA_LECTURA = 50;                    
int TIEMPOS_MUESTRA_LECTURA = 5;                      

#define GAS_CO2 2    
#define DHTPIN 3       // Pin donde está conectado el sensor DHT11
#define DHTTYPE DHT11  // Tipo de sensor DHT

DHT dht(DHTPIN, DHTTYPE);
float humedad = 0;
float temperatura = 0;




float CO2[3] = {2.3, 0.53, -0.44};                                                    
float Ro = 10;                 

void setup() { 
    Serial.begin(9600);
    dht.begin();
    pinMode(ledCalibracion, OUTPUT);
    digitalWrite(ledCalibracion, HIGH);                     
    Serial.println("Calibrando...");

    Ro = CalibrarSensor(PIN_MQ);                         
    digitalWrite(ledCalibracion, LOW);              

    Serial.println(Ro);
    delay(1000);
}

void loop() { 
    long ppmCO2 = 0;
    int valorLuz = 0;

    leerDHT11();    

    ppmCO2 = ObtenerPorcentajeGas(LeerSensor(PIN_MQ)/Ro, GAS_CO2);
    valorLuz = SensorCantidadLuz();

    Serial.print(humedad); // simulando el de humendad
    Serial.print(",");  
    Serial.print(temperatura); //simulando el de temperatura
    Serial.print(",");
    Serial.print(ppmCO2);  //salida del sensor de CO2
    Serial.print(",");
    Serial.println(valorLuz); 


    //para agregar más sensores es necesario agregar más líneas como la anterior
    /*
        Serial.print(ppmCO2);
        Serial.print(",");
        Serial.println(NUEVO SENSOR);
    */

    //SensorCantidadLuz(); //LDR

    delay(2000);
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
  humedad = dht.readHumidity();
  temperatura = dht.readTemperature();

  // Verificar si hay errores al leer el sensor
  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println("Error al leer el sensor DHT11");
    humedad = 0.0;
    temperatura = 0.0;
  }
}
