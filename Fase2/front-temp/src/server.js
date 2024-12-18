const express = require('express');
const bodyParser = require('body-parser');
const mysql = require('mysql2');
const cors = require('cors');

//crea la aplicación express
const app = express();
app.use(cors());

//configurar body-parser para procesar datos
app.use(bodyParser.json());

//Conf conexión local
/*const connection = mysql.createConnection({
    host:'localhost',
    port:3306,
    user:'root',
    password:'$$Ace123',
    database:'ace2_p1_g5'
});*/

//conexión a base de datos en cloud
const connection = mysql.createConnection({
    host:'accesovehicular.cneoqseiqzhs.us-east-1.rds.amazonaws.com',
    user:'admin',
    password:'KSh95:Nt2b(j4cxW',
    database:'accesovehicular'

});
//Abriendo conexión
connection.connect((err)=>{
    if(err){
        console.error('Error de conexion: '+err.stack);
        return;
    }
    console.log('Conectado');
});

//Crea ruta POST
/*app.post('/agregar',(req,res)=>{
    const{device_src, val} = req.body;//extrae los datos de la petición

    const sql = 'INSERT INTO device_data(device_src, val) values(?,?)';
    connection.query(sql,[device_src,val],(error,results)=>{
        if(error){
            console.error('Error al insertar datos: '+error.stack);
            return res.status(500).send('Error al insertar datos');
        }
        res.status(200).send('Datos insertados correctamente, ID: '+results.insertId);
    });

});*/

//Post al sp 
app.post('/agregar', (req, res) => {
    const data = req.body;  // Se espera que esto sea un arreglo de objetos [{device_src: "HUMEDAD", val: "100"}, ...]

    if (!Array.isArray(data)) {
        // Si no se recibe un array, responder con un error
        return res.status(400).send('El cuerpo de la solicitud debe ser un arreglo de objetos.');
    }

    let completedQueries = 0;  // Contador para saber cuándo se han completado todas las consultas
    let errors = [];  // Para almacenar posibles errores

    data.forEach(item => {
        const { device_src, val } = item;

        // Llamada al stored procedure
        const query = 'CALL insertarMedicion(?, ?, ?, NULL)';
        // - Fecha y Hora --> DATTIME (Se puede enviar NULL y se toma la fecha y hora de la base de datos)
        let date = new Date().toISOString().slice(0, 19).replace('T', ' ');  // Formato: 'YYYY-MM-DD HH:MM:SS'
        connection.query(query, [device_src, val, date], (err, result) => {
            completedQueries++;

            if (err) {
                console.error('Error ejecutando el procedimiento:', err);
                errors.push(err);
            }

            // Solo enviamos la respuesta cuando todas las consultas han finalizado
            if (completedQueries === data.length) {
                if (errors.length > 0) {
                    console.error('Error ejecutando algunas consultas:', errors);
                    res.status(500).send({ message: 'Error ejecutando algunas consultas', errors });
                } else {
                    console.log('Datos insertados correctamente');
                    res.status(200).send('Datos insertados correctamente');
                }
            }
        });
    });
});


//lectura
app.get('/datos',(req,res)=>{
    const { startDate, endDate } = req.query;

    // Llamada al stored procedure
    const query = 'CALL obtenerMediciones(?, ?)';
    connection.query(query, [startDate, endDate], (err, results) => {
        if (err) {
            console.error('Error executing procedure:', err);
            res.status(500).send('Error executing procedure');
            return;
        }
        console.log('Data retrieved successfully');
        res.status(200).json(results[0]); // Los resultados se devuelven en el primer elemento del array
    });
});

app.post('/crear_usuario', (req, res) => {
    const { nombre, pin } = req.body;  // Se espera recibir los parámetros desde el body

    // Validamos que se hayan enviado ambos parámetros
    if (!nombre || !pin) {
        return res.status(400).send('Faltan parámetros: nombre y pin son obligatorios.');
    }

    // Llamada al stored procedure sp_crear_usuario
    const query = 'CALL sp_crear_usuario(?, ?)';
    connection.query(query, [nombre, pin], (err, results) => {
        if (err) {
            console.error('Error ejecutando el procedimiento:', err);
            return res.status(500).send('Error al crear el usuario.');
        }

        // Asumiendo que el resultado del SP incluye el código de usuario creado
        const codigoUsuario = results; // Esto depende de cómo devuelve el SP la información
        console.log('Usuario creado correctamente, código:', codigoUsuario);
        res.status(200).send({ message: 'Usuario creado correctamente', codigoUsuario });
    });
});



//Iniciando el servidor
app.listen(3002,()=>{
    console.log('Servidor escuchando en el puerto 3002');
});

//Cerrando conexión
/*connection.end((err)=>{
    if(err) {
        console.error('Error al cerrar la conexión: '+err.stack);
        return;
    }
    console.log('Conexión cerrada.');
});
*/