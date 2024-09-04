const mysql = require('mysql2');

const connection = mysql.createConnection({
    host:'localhost',
    port:3306,
    user:'root',
    password:'$$Ace123',
    database:'ace2_p1_g5'
});

connection.connect((err)=>{
    if(err){
        console.error('Error de conexion: '+err.stack);
        return;
    }
    console.log('Conectado');
});

connection.query('SELECT id, device_src, val, date_post FROM device_data',(error,results,fields) =>{
    if(error){
        console.error('Error al ejecutar la consulta: '+error.stack);
        return;
    }
    console.log('Resultados: ',JSON.stringify(results,null,2));
    
});


connection.end((err)=>{
    if(err) {
        console.error('Error al cerrar la conexión: '+err.stack);
        return;
    }
    console.log('Conexión cerrada.');
});