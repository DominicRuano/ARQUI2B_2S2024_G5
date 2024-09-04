const express = require('express');
const bodyParser = require('body-parser');
const mysql = require('mysql2');

//crea la aplicación express
const app = express();

//configurar body-parser para procesar datos
app.use(bodyParser.json());

//Conf conexión
const connection = mysql.createConnection({
    host:'localhost',
    port:3306,
    user:'root',
    password:'$$Ace123',
    database:'ace2_p1_g5'
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
app.post('/agregar',(req,res)=>{
    const{device_src, val} = req.body;//extrae los datos de la petición

    const sql = 'INSERT INTO device_data(device_src, val) values(?,?)';
    connection.query(sql,[device_src,val],(error,results)=>{
        if(error){
            console.error('Error al insertar datos: '+error.stack);
            return res.status(500).send('Error al insertar datos');
        }
        res.status(200).send('Datos insertados correctamente, ID: '+results.insertId);
    });

});

//Iniciando el servidor
app.listen(3000,()=>{
    console.log('Servidor escuchando en el puerto 3000');
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