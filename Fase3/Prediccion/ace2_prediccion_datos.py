import mysql.connector
import pandas as pd
print('aqui inica el archivo')
# Conexión a la base de datos MySQL
connection = mysql.connector.connect(
    host='accesovehicular.cneoqseiqzhs.us-east-1.rds.amazonaws.com',
    user='admin',
    password='KSh95:Nt2b(j4cxW',
    database='accesovehicular'
)
print("creando cursor")
# Crear cursor y ejecutar el procedimiento almacenado con multi=True
cursor = connection.cursor()
print("ejecutando procedimiento")
# Ejecutar procedimiento almacenado con multi=True
cursor.execute("CALL obtenerMediciones('2024-01-01', '2024-01-31')", multi=True)

# Inicializar la variable data para almacenar los resultados
data = []

# Recolectar los resultados del cursor
for result in cursor:
    data.extend(result.fetchall())  # Guardar los resultados en 'data'
    print('leyendo datos')
# Verificar si se obtuvieron resultados
if data:
    # Convertir los datos a un DataFrame de pandas
    df = pd.DataFrame(data, columns=['nombre', 'valor', 'fecha', 'error'])

    # Mostrar los primeros registros para validar
    print(df.head())
else:
    print("No se obtuvieron datos.")

# Cerrar la conexión
cursor.close()
connection.close()
