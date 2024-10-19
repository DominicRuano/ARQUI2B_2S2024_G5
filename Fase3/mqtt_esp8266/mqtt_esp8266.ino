#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

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

// Topics a suscribirse
const char *topicControl = "F3G5/Control";
const char *topicLedControl = "F3G5/LedControl";
const char *topicRFID = "F3G5/RFID";

// Cliente WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);

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

      // Suscribirse a los topics
      client.subscribe(topicControl);
      client.subscribe(topicLedControl);
      client.subscribe(topicRFID);
    } else {
      Serial.print("Fallo al conectar al broker MQTT. Estado: ");
      Serial.println(client.state());
      delay(2000);  // Espera antes de reintentar
    }
  }
}

void enviarDatosQuemados() {
  // Crear JSON y enviar datos a varios topics
  enviarJSON(topicHumedad, 45, "humedad");
  enviarJSON(topicTemperatura, 25, "temperatura");
  enviarJSON(topicProximidad, 10, "proximidad");
  enviarJSON(topicCalidadAire, 400, "calidadAire");
  enviarJSON(topicLuminosidad, 300, "luminosidad");
  enviarJSON(topicAccesos, 5, "Accesos");
}

void enviarJSON(const char *topic, int valor, String name) {
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en ");
  Serial.print(topic);
  Serial.print(": ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void loop() {
  if (!client.connected()) {
    conectarMQTT();  // Intentar reconectar si se pierde la conexión
  }

  client.loop();  // Mantener la conexión activa

  // Enviar datos cada 5 segundos
  enviarDatosQuemados();
  delay(5000);  // Esperar 5 segundos antes de enviar nuevamente
}


