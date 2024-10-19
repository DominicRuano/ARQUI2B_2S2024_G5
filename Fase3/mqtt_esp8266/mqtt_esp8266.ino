#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial espSerial(D6, D5);  // D6 = RX, D5 = TX

// WiFi
const char *ssid = "Galaxy A21s03A5"; // Wi-Fi name
const char *password = "Doju1975@$1234";  // Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "192.168.56.14";
const char *topic = "Fase3G5/Test";

//diferentes topics a publicar

/*
* F3G5/humedad
* F3G5/temperatura
* F3G5/proximidad
* F3G5/calidadAire
* F3G5/luminosidad
* F3G5/Accesos
*/

const char *topicHumedad = "F3G5/humedad";
const char *topicTemperatura = "F3G5/temperatura";
const char *topicProximidad = "F3G5/proximidad";
const char *topicCalidadAire = "F3G5/calidadAire";
const char *topicLuminosidad = "F3G5/luminosidad";
const char *topicAccesos = "F3G5/Accesos";


//topics a suscribirse 
/*
* F3G5/Control
* F3G5/LedControl
* F3G5/RFID
*/

const char *topicControl = "F3G5/Control";
const char *topicLedControl = "F3G5/LedControl";
const char *topicRfid = "F3G5/RFID";


const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

    Serial.begin(9600); // Inicializa la comunicación serial para depuración
    espSerial.begin(9600);   // Comunicación con el Arduino Mega
    delay(1000);
    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    //subs a los topics
    client.subscribe(topicControl);
    client.subscribe(topicLedControl);
    client.subscribe(topicRfid);

}

void callback(char *topic, byte *payload, unsigned int length) {

    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    // Enviar el mensaje al Arduino Mega por SoftwareSerial
    espSerial.println(String(topic) + ":" + message);
    //Serial.println("Mensaje enviado al Arduino: " + message);

}

void loop() {

  client.loop(); //para mantener la conexion mqtt activa


  if (espSerial.available()) { 
    String mensaje = espSerial.readStringUntil('\n'); // Lee el mensaje completo hasta el salto de línea

    //json que regresa (es algo así):

    /*
    String jsonData = "{\"Humedad\":" + String(humedad) + ",\"Temperatura\":" + String(temperatura) +
                      ",\"PPMCO2\":" + String(ppmCO2) + ",\"Luz\":" + String(valorLuz) +
                      ",\"Infrarrojo\":" + String(valorInfrarrojo) + ",\"Distancia\":" + String(valorDistancia) +
                      ",\"AccesosCorrectos\":" + String(contadorAccesosCorrectos) + "}";
    */

    //buffer estático para parsear el json
    StaticJsonDocument<256> doc;

    //parseear el json
    DeserializationError error = deserializeJson(doc, mensaje);
    if(error){
      Serial.print("JSON parse failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Publicar en los topics MQTT según los datos recibidos
    if (doc.containsKey("Humedad")) {
        client.publish(topicHumedad, String(doc["Humedad"]).c_str());
    }
    if (doc.containsKey("Temperatura")) {
        client.publish(topicTemperatura, String(doc["Temperatura"]).c_str());
    }
    if (doc.containsKey("PPMCO2")) {
        client.publish(topicCalidadAire, String(doc["PPMCO2"]).c_str());
    }
    if (doc.containsKey("Luminosidad")) {
        client.publish(topicLuminosidad, String(doc["Luminosidad"]).c_str());
    }
    if (doc.containsKey("Distancia")) {
        client.publish(topicProximidad, String(doc["Distancia"]).c_str());
    }
    if (doc.containsKey("AccesosCorrectos")) {
        client.publish(topicAccesos, String(doc["AccesosCorrectos"]).c_str());
    }

    delay(1000);  //para que no se sature
  }
}