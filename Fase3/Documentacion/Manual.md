
<h1 align="center">Proyecto 01</h1>

<div align="center">
📕 Arquitectura De Computadoras y Ensambladores 2
</div>
<div align="center"> 🏛 Universidad San Carlos de Guatemala</div>
<div align="center"> 📆 Segundo Semestre 2024</div>
<div align="center">📎 Grupo 05</div>

#### 📍 CONTROL DE ACCESO VEHICULAR CON IOT
*Este proyecto consiste en el desarrollo de un sistema de control de acceso vehicular mejorado mediante la implementación de una estación meteorológica basada en IoT. Este sistema está diseñado para recopilar datos meteorológicos en tiempo real y utilizar estos datos para optimizar el control climático del entorno.*

##### OBJETIVOS
- **Diseñar un dispositivo IoT** para medir y registrar variables meteorológicas como temperatura, humedad, iluminación y concentración de CO2.
- **Desarrollar un algoritmo de análisis de datos** que interprete la información meteorológica y prediga patrones climáticos relevantes.
- **Implementar una plataforma centralizada de gestión** que integre datos meteorológicos y controle dispositivos en el sistema de acceso vehicular.
- **Desarrollar un sistema de visualización de datos** que facilite el reconocimiento y análisis de información relevante para el usuario final.

---
#### **CAPAS STACK IOT FRAMEWORK IMPLEMENTADAS**
##### **SENSORES**
Los sensores son la primera capa del Stack IoT Framework y son responsables de la recopilación de datos meteorológicos y de proximidad. Los sensores implementados en este proyecto incluyen:
- **Sensor de Temperatura y Humedad (DHT11)**: EstE sensores miden la temperatura y humedad del ambiente.
- **Sensor de CO2 (MQ2)**: Este sensor mide la concentración de CO2 en el aire para evaluar la calidad del aire.
- **Sensor de Iluminación (LDR)**: Este sensor mide la cantidad de luz en el ambiente utilizando una fotocelda.
- **Sensor de Movimiento (HC-SR04)**: Este sensor detecta la proximidad de objetos y personas.
- **Pantalla LCD**: Muestra la información recopilada en tiempo real al usuario.
- **LED Infrarrojo**: Indica el estado de un evento en el sistema, como la detección de movimiento.

##### **CONECTIVIDAD**
La capa de conectividad se encarga de la transmisión de datos desde los sensores al microcontrolador Arduino y su posterior envío a la plataforma centralizada. La conectividad se logra a través de cables que conectan el Arduino a una computadora, desde donde los datos son procesados y visualizados en tiempo real usando Processing.

##### **PRODUCT INFRASTRUCTURE**
Esta capa incluye la infraestructura física del sistema, principalmente el microcontrolador Arduino, la pantalla LCD para la presentación de datos y los botones para la interacción del usuario.

##### **ANALYTICS**
La capa de análisis de datos es crucial para interpretar la información meteorológica recopilada. En este proyecto, el análisis de datos se realiza utilizando Processing, lo que permite visualizar los datos en tiempo real y tomar decisiones basadas en ellos.

##### **SMART APP**
En fases futuras, se desarrollarán aplicaciones que permitirán a los usuarios interactuar con el sistema y tomar decisiones basadas en los datos meteorológicos. En la fase 1, se establecen las bases para esta capa.

##### **PLATAFORMA DE GESTIÓN**

Esta capa se encarga de la administración y monitoreo del sistema IoT. Integra la información de los sensores y permite gestionar las acciones necesarias para optimizar el control de acceso vehicular y el control climático del entorno. La aplicación está levantada en una instancia EC2 de AWS, lo cual permite el acceso remoto y escalabilidad del sistema, aprovechando la infraestructura en la nube para una mayor disponibilidad y seguridad.

#### **SEGURIDAD**

La capa de seguridad se enfoca en garantizar la integridad de los datos recopilados y la privacidad del sistema. Implementa medidas como cifrado de datos y autenticación de usuarios para proteger la infraestructura IoT y las comunicaciones. Además, se aprovechan las capacidades de seguridad de AWS, como las reglas de seguridad de EC2 y la gestión de credenciales para garantizar un entorno seguro en la nube.
---

#### Sensor LDR
Este código permite medir la cantidad de luz en el ambiente utilizando un sensor LDR , este dato es enviado a processing para generar una grafica.
```arduino

int ldrPin = A1;
void setup() {
  Serial.begin(9600);
}

void loop() {
  int ldrValue = analogRead(ldrPin);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);
  delay(1000);
}

```

#### Pantalla LCD
Este código controla una pantalla LCD para mostrar mensajes y datos en tiempo real. La pantalla LCD se inicializa y muestra un mensaje de bienvenida al arrancar.
```arduino

#include <LiquidCrystal.h>

void lcdInicio(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("***Bienvenido***");
  lcd.setCursor(0,1);
  lcd.print("Fase 1 - Grupo 5");  
}

```

#### Sensor DHT11
Este código se utiliza para leer la temperatura y la humedad del ambiente utilizando el sensor DHT11. Los valores de temperatura y humedad se imprimen en el monitor serial y son enviados al processing para generar una grafica.

```arduino

#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.print("%  Temperatura: ");
  Serial.print(temperature);
  Serial.println("C");
  delay(2000);
}

```

#### Sensor MQ2
Este código mide la concentración de CO2 en el aire usando el sensor MQ2. El valor analógico leído del pin A0 se imprime en el monitor serial.
```arduino

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

```

#### LED Infrarrojo
Este código enciende y apaga un LED infrarrojo conectado al pin 9 del Arduino. El LED parpadea cada segundo.
```arduino

int ledPin = 9;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}

```

#### Sensor RC522
Este código se utiliza para leer la tarjeta RFID utilizando el sensor RC522.
```arduino
#include <MFRC522.h>

#define SS_PIN 10
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
  SPI.begin();
  rfid.PCD_Init();
}

void leerTarjetaRFID() {
  // Revisar si hay una tarjeta presente
  if (!rfid.PICC_IsNewCardPresent()) {
    return false; // Si no hay tarjeta, salimos de la función
  }

  // Revisar si la tarjeta puede ser leída
  if (!rfid.PICC_ReadCardSerial()) {
    return false; // Si no puede ser leída, salimos de la función
  }

  // Comparar el NUID con la lista de NUIDs conocidos
  if (tarjetaEsConocida(rfid.uid.uidByte)) {
    rfid.PICC_HaltA();
    return true;
  } else {
    rfid.PICC_HaltA();
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
```

#### Servo Motor
Este codigo controla un servo motor conectado al arduino, dependiendo del sensor rfid se activa el servo motor.
```arduino
#include <Servo.h>

bool barreraAbierta = false;           // Estado inicial de la barrera: cerrada

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
```

#### Ultrasonico
Este código controla un sensor de proximidad ultrasónico HC-SR04 para detectar la distancia de un objeto. Dependiendo de la distancia, se encienden diferentes colores de LED.

```arduino

const int Trigger = 45;   
const int Echo = 47;      
const int LedRojo = 53;   
const int LedVerde = 49;  

unsigned long tiempoInicioRojo = 0;  // Variable para almacenar el tiempo cuando se enciende el rojo
bool rojoEncendido = false;          // Indicador de si el LED rojo está encendido
bool cambiarAAmarillo = false;       // Indicador para cambiar a amarillo después de los 5 segundos

void setup(){
  pinMode(Trigger, OUTPUT);  // Pin como salida para el Trigger
  pinMode(Echo, INPUT);      // Pin como entrada para el Echo
  pinMode(LedRojo, OUTPUT);  // Pin como salida para el color rojo
  pinMode(LedVerde, OUTPUT); // Pin como salida para el color verde
  digitalWrite(Trigger, LOW); // Inicializamos el Trigger en bajo
}

void loop(){
  valorDistancia = distancia();
  ultrasonico(valorDistancia);
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
```


#### DASHBOARD
El dashboard en React se utiliza para visualizar en un intervalo de tiempo determinado los datos recopilados por los sensores. Estos datos son almacenados en una base de datos (DB) y el dashboard los recupera para representarlos de manera gráfica e interactiva en una interfaz web. La interfaz permite al usuario monitorear y analizar fácilmente la información proveniente de los sensores, mejorando la interpretación y toma de decisiones basada en los datos.

![IMAGEN1](./img/1.png)

```javascript

  return (
    <Router>
      <div className='dashboard'>
        <SideBar />
        <div className="dashboard--content">
          <Routes>
            <Route path="/" element={<Dashboard />} />
            <Route path="/clima" element={<Clima />} />
            <Route path="/pronostico" element={<Pronostico />} />
            <Route path="/about" element={<About />} />
          </Routes>
        </div>
      </div>
    </Router>
  );
};


```
**PLATAFORMA DE GESTIÓN**

Esta capa se encarga de la administración y monitoreo del sistema IoT. Integra la información de los sensores y permite gestionar las acciones necesarias para optimizar el control de acceso vehicular y el control climático del entorno. La aplicación está levantada en una instancia EC2 de AWS, lo cual permite el acceso remoto y escalabilidad del sistema, aprovechando la infraestructura en la nube para una mayor disponibilidad y seguridad. Además, se utiliza MQTT para la comunicación entre dispositivos, usando Mosquitto como broker público, lo cual facilita la transmisión de datos entre los diferentes componentes del sistema IoT.

**MQTT y Mosquitto como Broker**

MQTT (Message Queuing Telemetry Transport) es un protocolo de mensajería ligero ideal para aplicaciones IoT debido a su eficiencia y bajo consumo de ancho de banda. En este proyecto, MQTT se utiliza para establecer la comunicación entre los sensores y la plataforma de gestión, permitiendo un intercambio rápido y confiable de datos.

Mosquitto se emplea como broker MQTT, que actúa como un intermediario para gestionar los mensajes entre los dispositivos conectados. Al usar Mosquitto como broker público, se logra una configuración sencilla y eficiente para manejar las publicaciones y suscripciones de los diferentes tópicos, facilitando la comunicación entre los componentes del sistema sin la necesidad de una infraestructura compleja.

**SEGURIDAD**

La capa de seguridad se enfoca en garantizar la integridad de los datos recopilados y la privacidad del sistema. Implementa medidas como cifrado de datos y autenticación de usuarios para proteger la infraestructura IoT y las comunicaciones. Además, se aprovechan las capacidades de seguridad de AWS, como las reglas de seguridad de EC2 y la gestión de credenciales para garantizar un entorno seguro en la nube.



### Prototipo

El prototipo desarrollado en esta fase incluye los siguientes componentes:

- **Microcontrolador Arduino**: El cerebro del sistema, encargado de procesar los datos de los sensores y controlar los actuadores como la pantalla LCD.
- **Sensores**: Incluye sensores de temperatura y humedad (DHT11), calidad del aire (MQ2), iluminación (LDR) y Modulo TCS3200 (detector de color), y proximidad (HC-SR04).
- **Actuadores**: Una pantalla LCD para mostrar la información.
- **Conectividad**: Comunicación entre el Arduino y una computadora a través de un cable USB, con visualización de datos en Processing.
- **Interfaz de Usuario**: Botones físicos en el prototipo permiten la interacción del usuario, como mostrar datos en tiempo real o acceder a información almacenada.


| ![IMAGEN1](./img/2.jpeg) | ![IMAGEN2](./img/4.jpeg) |
|------------------------------|------------------------------|
| ![IMAGEN3](./img/3.jpeg) | ![IMAGEN4](./img/5.jpeg) |
---

### Instancia de EC2 de AWS

![ER](./img/6.png)
