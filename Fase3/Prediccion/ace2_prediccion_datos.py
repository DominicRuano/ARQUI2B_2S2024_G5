from flask import Flask, request, jsonify
import mysql.connector
from sklearn.linear_model import LinearRegression
import numpy as np

app = Flask(__name__)

# Conectar a la base de datos MySQL
def obtener_conexion():
    return mysql.connector.connect(
        host='accesovehicular.cneoqseiqzhs.us-east-1.rds.amazonaws.com',
        user='admin',
        password='KSh95:Nt2b(j4cxW',
        database='accesovehicular',
        port='3306'
    )

# Definir las respuestas del servicio para cada sensor
def procesar_respuesta(sensor, valor):
    if sensor == 'mq':
        return {"sensor": sensor, "valor": {"gas": valor}}
    elif sensor == 'dht':
        # Aquí asumimos que el valor es una tupla de (temperatura, humedad)
        return {"sensor": sensor, "valor": {"temperatura": valor[0], "humedad": valor[1]}}
    elif sensor == 'ldr':
        return {"sensor": sensor, "valor": {"luminosidad": valor}}
    else:
        return {"sensor": sensor, "valor": "desconocido"}

# Modelo de predicción
def predecir(datos_historicos, nuevo_valor):
    # Separar los datos históricos en variables X (independiente) e Y (dependiente)
    X = np.array([d[0] for d in datos_historicos]).reshape(-1, 1)  # Aquí asumimos que la primera columna es el tiempo/día
    y = np.array([d[1] for d in datos_historicos])  # Aquí asumimos que la segunda columna es el valor a predecir

    # Crear un modelo de regresión lineal
    modelo = LinearRegression()
    modelo.fit(X, y)

    # Realizar la predicción para el nuevo valor
    prediccion = modelo.predict(np.array([[nuevo_valor]]))

    return prediccion[0]

# Ruta para recibir la solicitud JSON
@app.route('/sensor', methods=['POST'])
def obtener_mediciones():
    # Obtener los datos del JSON recibido
    datos = request.get_json()
    dia = datos.get('dia')
    sensor_nombre = datos.get('sensor')
    
    # Validar que los parámetros existan
    if not dia or not sensor_nombre:
        return jsonify({"error": "Faltan parámetros"}), 400
    
    # Conectar a la base de datos
    conexion = obtener_conexion()
    cursor = conexion.cursor()

    # Ajustar las fechas para llamar al procedimiento almacenado
    fecha_inicio = '2024-01-01'
    fecha_fin = '2024-12-31'

    # Llamar al procedimiento almacenado con las fechas definidas
    query = "CALL obtenerMediciones(%s, %s)"
    
    try:
        data = []
        for result in cursor.execute(query, (fecha_inicio, fecha_fin), multi=True):
            if result.with_rows:
                data.extend(result.fetchall())  # Guardar los resultados en 'data'

        # Verificar si se obtuvieron resultados
        if not data:
            return jsonify({"error": "No se encontraron datos para el sensor y día solicitados"}), 404
        
        # Obtener el valor de la medición más reciente
        valor_reciente = data[-1][1]  # Aquí asumimos que 'valor' es la segunda columna en los resultados
        
        # Usar scikit-learn para predecir un nuevo valor
        nuevo_valor = int(dia)  # Supongamos que 'dia' es un número que representa el día
        valor_predicho = predecir(data, nuevo_valor)

        # Procesar la respuesta con el valor predicho
        respuesta = procesar_respuesta(sensor_nombre, valor_predicho)

    except mysql.connector.Error as err:
        return jsonify({"error": str(err)}), 500

    finally:
        cursor.close()
        conexion.close()

    # Devolver la respuesta en formato JSON
    return jsonify(respuesta)

# Ejecutar la aplicación
if __name__ == '__main__':
    app.run(debug=True)
