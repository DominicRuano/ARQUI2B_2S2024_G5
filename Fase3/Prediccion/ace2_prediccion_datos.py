import mysql.connector
import json

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
        # Asumimos que el valor es una tupla de (temperatura, humedad)
        return {"sensor": sensor, "valor": {"temperatura": valor[0], "humedad": valor[1]}}
    elif sensor == 'ldr':
        return {"sensor": sensor, "valor": {"luminosidad": valor}}
    else:
        return {"sensor": sensor, "valor": "desconocido"}

# Método para obtener las mediciones
def obtener_mediciones(datos):
    # Obtener los parámetros del JSON
    dia = datos.get('dia')
    sensor_nombre = datos.get('sensor')

    # Validar que los parámetros existan
    if not dia or not sensor_nombre:
        return {"error": "Faltan parámetros"}

    # Conectar a la base de datos
    conexion = obtener_conexion()
    cursor = conexion.cursor()

    # Ajustar las fechas para llamar al procedimiento almacenado
    fecha_inicio = '2024-01-01'
    fecha_fin = '2024-12-31'

    # Llamar al procedimiento almacenado
    query = "CALL obtenerMediciones(%s, %s)"
    
    try:
        data = []
        for result in cursor.execute(query, (fecha_inicio, fecha_fin), multi=True):
            if result.with_rows:
                data.extend(result.fetchall())  # Guardar los resultados en 'data'

        # Verificar si se obtuvieron resultados
        if not data:
            return {"error": "No se encontraron datos para el sensor y día solicitados"}
        
        # Obtener el valor de la medición más reciente
        valor_reciente = data[-1][1]  # Asumimos que 'valor' es la segunda columna

        # Procesar la respuesta según el sensor
        respuesta = procesar_respuesta(sensor_nombre, valor_reciente)

    except mysql.connector.Error as err:
        return {"error": str(err)}

    finally:
        cursor.close()
        conexion.close()

    # Devolver la respuesta
    return respuesta
