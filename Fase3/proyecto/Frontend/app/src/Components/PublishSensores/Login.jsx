import React, { useState } from 'react';
import mqtt from 'mqtt';
import axios from 'axios';

const client = mqtt.connect('wss://test.mosquitto.org:8081/mqtt', { keepalive: 60 });

const Login = () => {
    const [usuario, setUsuario] = useState('');
    const [pin, setPin] = useState('');
    const [mensaje, setMensaje] = useState('');

    const handleLogin = async () => {
        try {
            const response = await axios.post('http://100.24.10.27:5000/validateUser', { usuario, pin });
            
            if (response.data.accesoConcedido) {
                const nuidMessage = JSON.stringify({ NUID: "0x19,0x7D,0xD1,0xB1" }); // NUID para acceso concedido
                client.publish('F3G5/RFID', nuidMessage, { qos: 0 }, (error) => {
                    if (!error) {
                        setMensaje('Acceso concedido...');
                    } else {
                        console.error('Error al publicar en MQTT:', error);
                    }
                });
            } else {
                const nuidIncorrecto = JSON.stringify({ NUID: "0x39,0x09,0xFB,0xB3" }); // NUID para acceso denegado
                client.publish('F3G5/RFID', nuidIncorrecto, { qos: 0 }, (error) => {
                    if (!error) {
                        setMensaje('Usuario o PIN incorrecto.');
                    } else {
                        console.error('Error al publicar en MQTT:', error);
                    }
                });
            }
        } catch (error) {
            console.error('Error en la validacion:', error);
            setMensaje('Error de conexion.');
        }
    };

    return (
        <div>
            <h2>Enter your credentials to proceed</h2>
            <input
                type="text"
                placeholder="Usuario"
                value={usuario}
                onChange={(e) => setUsuario(e.target.value)}
            />
            <input
                type="password"
                placeholder="pin"
                value={pin}
                onChange={(e) => setPin(e.target.value)}
            />
            <button onClick={handleLogin}>Ingresar</button>
            <p>{mensaje}</p>
        </div>
    );
};

export default Login;
