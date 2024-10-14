#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

SoftwareSerial espSerial(D6, D5);  // D6 = RX, D5 = TX

// WiFi
const char *ssid = "Galaxy A21s03A5"; // Enter your Wi-Fi name
const char *password = "Doju1975@$1234";  // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "192.168.56.14";
const char *topic = "Fase3G5/Test";
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
    // Publish and subscribe
    client.publish(topic, "Hi, I'm ESP32 ^^");
    //client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
  if (espSerial.available()) { 
    String mensaje = espSerial.readStringUntil('\n'); // Lee el mensaje completo hasta el salto de línea
    //Serial.println("Mensaje recibido del serial:");
    //Serial.println(mensaje);

    // Convertir el String a const char*
    client.publish(topic, mensaje.c_str()); 

    delay(500);
  }
  client.loop();
}
