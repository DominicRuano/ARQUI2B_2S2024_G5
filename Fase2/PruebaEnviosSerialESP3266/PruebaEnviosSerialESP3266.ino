/*#include <SoftwareSerial.h>

SoftwareSerial espSerial(D6, D5);  // D6 = RX, D5 = TX

void setup() {
  Serial.begin(9600);      // Para el Monitor Serial
  espSerial.begin(9600);   // Comunicación con el Arduino Mega
}

void loop() {
  if (espSerial.available()) {
    String mensaje = espSerial.readStringUntil('\n');  // Leer datos del Arduino
    Serial.println("Mensaje recibido: " + mensaje);   // Mostrar en Monitor Serial
  }
}*/


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

SoftwareSerial espSerial(D6, D5);  // D6 = RX, D5 = TX

const char* ssid = "Galaxy A21s03A5";         // Reemplaza con el nombre de tu red WiFi
const char* password = "Doju1975@$1234"; // Reemplaza con la contraseña de tu red WiFi
const char* serverName = "http://192.168.226.196:3002/agregar"; // Reemplaza "192.168.x.x" con la IP de tu servidor

WiFiClient client;  // Crea el objeto WiFiClient

void setup() {
  Serial.begin(9600); // Inicializa la comunicación serial para depuración
  espSerial.begin(9600);   // Comunicación con el Arduino Mega
  delay(1000);

  // Conexión a la red WiFi
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);

  // Espera hasta que el ESP8266 se conecte al WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado a la red WiFi");
}

void loop() {
  // Lee el mensaje desde el puerto serial
  if (espSerial.available()) { 
    String mensaje = espSerial.readStringUntil('\n'); // Lee el mensaje completo hasta el salto de línea
    Serial.println("Mensaje recibido del serial:");
    Serial.println(mensaje); // Verifica el mensaje que estás enviando
    
    if (WiFi.status() == WL_CONNECTED) { // Verifica que esté conectado al WiFi
      HTTPClient http; // Crea un objeto HTTPClient

      // Inicia la conexión HTTP con el WiFiClient y la URL
      http.begin(client, serverName);

      // Especifica el tipo de contenido que se va a enviar (application/json en lugar de x-www-form-urlencoded)
      http.addHeader("Content-Type", "application/json");

      // Envía la solicitud POST con el string recibido
      int httpResponseCode = http.POST(mensaje);

      // Verifica la respuesta del servidor
      if (httpResponseCode > 0) {
        String response = http.getString(); // Lee la respuesta del servidor
        Serial.println("Código de respuesta HTTP: " + String(httpResponseCode));
        Serial.println("Respuesta del servidor: " + response);
      } else {
        Serial.println("Error en la solicitud POST");
        Serial.println("Código de error: " + String(httpResponseCode));
      }

      // Finaliza la conexión
      http.end();
    } else {
      Serial.println("Error: No conectado al WiFi");
    }
  }

  delay(3000); // Espera 3 segundos antes de la siguiente iteración
}