import paho.mqtt.client as mqtt

# Función de callback cuando se conecta
def on_connect(client, userdata, flags, rc):
    print("Conectado con resultado de código: " + str(rc))
    # Suscribirse al tema
    client.subscribe("F3G5/pronostico")

# Función de callback para recibir mensajes
def on_message(client, userdata, msg):
    print(f"Mensaje recibido en {msg.topic}: {msg.payload.decode()}")

# Crear un cliente MQTT con el transporte WebSocket seguro
client = mqtt.Client()

# Asignar las funciones de callback
client.on_connect = on_connect
client.on_message = on_message

# Conectar al broker MQTT usando el puerto 8081 para WebSocket seguro
try:
    client.connect("test.mosquitto.org", 8081,60)
except Exception as e:
    print(f"Error al conectar: {e}")

# Iniciar el bucle para manejar las conexiones y recibir mensajes
client.loop_start()

# # Publicar un mensaje de prueba
client.publish("F3G5/PronosticoResultado", "Este es un mensaje de prueba")

# Mantener el script en ejecución
try:
    while True:
        pass
except KeyboardInterrupt:
    print("Desconectando...")
    client.loop_stop()
    client.disconnect()
