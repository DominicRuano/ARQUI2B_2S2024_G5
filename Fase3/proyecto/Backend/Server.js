const express = require('express');
const mysql = require('mysql');
const cors = require('cors');  
const app = express();

app.use(cors()); 
app.use(express.json());

// Conexión a la base de datos MySQL en AWS RDS
const connection = mysql.createConnection({
    host: 'accesovehicular.cneoqseiqzhs.us-east-1.rds.amazonaws.com',
    user: 'admin',
    password: 'KSh95:Nt2b(j4cxW',
    database: 'accesovehicular',
    port: 3306 // Puerto de MySQL
});


connection.connect((err) => {
    if (err) {
        console.error('Error de conexion: ' + err.stack);
        return;
    }
    console.log('Conectado a la base de datos MySQL en AWS RDS');
});

// Ruta para validar usuarios
app.post('/validateUser', (req, res) => {
    const { usuario, pin } = req.body;
    const query = 'SELECT * FROM usuario WHERE nombre = ? AND pin = ?';

    connection.query(query, [usuario, pin], (error, results) => {
        if (error) return res.status(500).json({ error: 'Error en la base de datos' });

        if (results.length > 0) {
            res.json({ accesoConcedido: true, nuid: results[0].pin });
        } else {
            res.json({ accesoConcedido: false });
        }
    });
});

// Iniciar servidor en el puerto 5000
app.listen(5000, () => {
    console.log('Servidor corriendo en puerto 5000');
});
