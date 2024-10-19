#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// Configuración de pines para SoftwareSerial
SoftwareSerial espSerial(D6, D5);  // D6 = RX, D5 = TX

// WiFi Credentials
const char *ssid = "Galaxy A21s03A5";
const char *password = "Doju1975@$1234";

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org";
const int mqtt_port = 1883;  // Puerto estándar MQTT

// Topics a publicar
const char *topicHumedad = "F3G5/humedad";
const char *topicTemperatura = "F3G5/temperatura";
const char *topicProximidad = "F3G5/proximidad";
const char *topicCalidadAire = "F3G5/calidadAire";
const char *topicLuminosidad = "F3G5/luminosidad";
const char *topicAccesos = "F3G5/Accesos";

// Cliente WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

//variable que controla el acceso de los carros
int accesos = 0;

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);  // Comunicación con el Arduino Mega

  // Conectar al Wi-Fi
  Serial.println("Conectando a Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado");

  // Configurar el cliente MQTT
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);  // Asignar función de callback

  conectarMQTT();
}

void conectarMQTT() {
  Serial.print("Conectando al broker MQTT...");

  while (!client.connected()) {
    String client_id = "ESP8266Client-" + String(WiFi.macAddress());

    if (client.connect(client_id.c_str())) {
      Serial.println("Conectado al broker MQTT");

      // Suscribirse a los topics de control
      client.subscribe("F3G5/Control");
      client.subscribe("F3G5/LedControl");
      client.subscribe("F3G5/RFID");
    } else {
      Serial.print("Fallo al conectar al broker MQTT. Estado: ");
      Serial.println(client.state());
      delay(2000);  // Espera antes de reintentar
    }
  }
}

void enviarJSON(const char *topic, String valor, String name) {
  StaticJsonDocument<128> doc;
  doc[name] = valor;

  char buffer[128];
  serializeJson(doc, buffer);

  if (client.publish(topic, buffer)) {
    Serial.print("JSON enviado correctamente en ");
    Serial.println(topic);
  } else {
    Serial.print("Error al enviar JSON en ");
    Serial.println(topic);
  }
}

void procesarMensajeArduino(String mensaje) {
  // Crear un JSONDocument para parsear el mensaje
  StaticJsonDocument<512> doc;

  // Parsear el JSON recibido
  DeserializationError error = deserializeJson(doc, mensaje);

  if (error) {
    Serial.print("Error al parsear JSON ESP: ");
    Serial.println(error.c_str());
    return;
  }

  // Extraer y enviar datos a los topics correspondientes
  if (doc.containsKey("Humedad")) {
    enviarJSON(topicHumedad, String(doc["Humedad"].as<float>()), "humedad");
  }
  if (doc.containsKey("Temperatura")) {
    enviarJSON(topicTemperatura, String(doc["Temperatura"].as<float>()), "temperatura");
  }
  if (doc.containsKey("PPMCO2")) {
    enviarJSON(topicCalidadAire, String(doc["PPMCO2"].as<int>()), "calidadAire");
  }
  if (doc.containsKey("Luz")) {
    enviarJSON(topicLuminosidad, String(doc["Luz"].as<int>()), "luminosidad");
  }
  if (doc.containsKey("Distancia")) {
    enviarJSON(topicProximidad, String(doc["Distancia"].as<float>()), "proximidad");
  }
    
  if (doc.containsKey("AccesosCorrectos")) {
      if((doc["AccesosCorrectos"].as<int>()) != 0){
        accesos += (doc["AccesosCorrectos"].as<int>());
        enviarJSON(topicAccesos, String(accesos), "Accesos");
      }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  
  /*Serial.print("Mensaje recibido en ");
  Serial.print(topic);
  Serial.print(": ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();*/

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

    // Enviar el mensaje al Arduino Mega por SoftwareSerial
  espSerial.println(String(message));

}

void loop() {
  if (!client.connected()) {
    conectarMQTT();  // Intentar reconectar si se pierde la conexión
  }

  client.loop();  // Mantener la conexión activa

  // Leer datos desde el Arduino Mega
  if (espSerial.available()) {
    String mensaje = espSerial.readStringUntil('\n');
    Serial.println("Mensaje recibido del Arduino: " + mensaje);
    procesarMensajeArduino(mensaje);  // Procesar y enviar datos
  }

  
  //delay(900);  // Pequeña espera para evitar saturación
}