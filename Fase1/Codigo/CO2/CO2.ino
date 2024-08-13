const int ledCalibracion = 13;                      
const int PIN_MQ = A0;                                
int VALOR_RL = 1;                                     
float FACTOR_AIRE_LIMPIO_RO = 9.75;                     

int TIEMPOS_MUESTRA_CALIBRACION = 10;                  
int INTERVALO_MUESTRA_CALIBRACION = 100;               
int INTERVALO_MUESTRA_LECTURA = 50;                    
int TIEMPOS_MUESTRA_LECTURA = 5;                      

#define GAS_CO2 2    

float CO2[3] = {2.3, 0.53, -0.44};                                                    
float Ro = 10;                 

void setup() { 
    Serial.begin(9600);
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

    ppmCO2 = ObtenerPorcentajeGas(LeerSensor(PIN_MQ)/Ro, GAS_CO2);

    Serial.print("22.5");
    Serial.print(",");
    Serial.print("16");
    Serial.print(",");
    Serial.println(ppmCO2); 

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
