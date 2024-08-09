const int pinInfrarrojo = 2;

void setup(){
    pinMode(pinInfrarrojo, INPUT);
    Serial.begin(9600);
}

void loop(){
    int valor = digitalRead(pinInfrarrojo);
    if (valor == 0){ //si detecta un obstáculo, el valor del sensor es cero
        Serial.println("obstáculo");
    }else{
        Serial.println("libre");
    }
    delay(200);
}