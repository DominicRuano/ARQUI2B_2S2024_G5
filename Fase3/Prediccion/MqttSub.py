import paho.mqtt.client as mqtt
import ace2_prediccion_datos as prediccion
import json
def on_connect(client, userdata, flags, rc):
    print("Se conecto con mqtt "+str(rc))
    client.subscribe("F3G5/pronostico")

def on_message(client, userdata, msg):
    if msg.topic == "F3G5/pronostico":
        print(f"Predicción es {str(msg.payload)}")
    print(msg.topic+" "+str(msg.payload))

    client = mqtt.Client(transport='websockets')
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("test.mosquitto.org",1883,60)
    client.loop_forever()
# def llamada():
#     parametro = {"dia":"1","sensor":"mq"}
#     json_data = json.dumps(parametro)
#     #print(json_data)
#     prediccion.obtener_mediciones(json_data)
    