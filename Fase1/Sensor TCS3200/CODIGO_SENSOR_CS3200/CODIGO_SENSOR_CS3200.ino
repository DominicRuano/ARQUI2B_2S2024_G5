// Sensor de Color
//
//    Arduino   TCS230 SENSOR COLOR
//      10         OUT
//      11         S3
//      12         S2
//      9         S1
//      8         S0
//      5V        VCC
//      GND       GND
//

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
  
  //Iniciar la comunicación serie
  Serial.begin(9600);
}
void loop() {
  // Configura el filtor ROJO para tomar lectura
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  //delay(100);
  //Rojo_Frec= pulseIn(sensorSalida, LOW);
  frecuenciaRojo = pulseIn(sensorSalida,LOW);
  colorRojo = map(frecuenciaRojo,170,1115,255,0);
  Serial.print(" R= "); 
  Serial.print(frecuenciaRojo);
  //Serial.print(colorRojo);
  delay(100);
  // Configura el filtor VERDE para tomar lectura
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  delay(100);
  //Verde_Frec = pulseIn(sensorSalida, LOW);
  frecuenciaVerde = pulseIn(sensorSalida, LOW);
  colorVerde = map(frecuenciaVerde,100,199,255,0);
  Serial.print(" V= ");
  //Serial.print(frecuenciaVerde);
  Serial.print(colorVerde);
  delay(100);
  // Configura el filtor AZUL para tomar lectura
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  delay(100);
  //Azul_Frec = pulseIn(sensorSalida, LOW);
  frecuenciaAzul = pulseIn(sensorSalida, LOW);
  colorAzul = map(frecuenciaVerde,38,84,255,0);
  Serial.print(" A= "); 
  //Serial.print(frecuenciaAzul);
  Serial.print(colorAzul);
  delay(100);

                  //Valores obtenidos en la calibracion del sensor, son propios de cada usuario 
                  
 /*if (Rojo_Frec < 40 && Verde_Frec > 50 && Azul_Frec <40)  Serial.print(" . *** PELOTA ROJA **");
 if (Rojo_Frec > 55 && Verde_Frec <60 && Azul_Frec < 35)  Serial.print(" . *** PELOTA AZUL **");
 if (Rojo_Frec < 65 && Verde_Frec < 50 && Azul_Frec >40)  Serial.print(" . *** PELOTA VERDE **");
*/
if (colorRojo > colorVerde && colorRojo > colorAzul){
  Serial.println("- Detectado ROJO");
}
if (colorVerde > colorRojo && colorVerde > colorAzul){
  Serial.println("- Detectado VERDE");
}
if (colorAzul > colorRojo && colorAzul > colorVerde){
  Serial.println("- Detectado AZUL");
}

// Serial.println("*");
 delay(200);
}
