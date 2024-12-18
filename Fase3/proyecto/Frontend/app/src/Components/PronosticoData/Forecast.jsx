import React, { useState } from 'react';
import mqtt from 'mqtt';
import PronosticoCard from './PronosticoCard';

const client = mqtt.connect('wss://test.mosquitto.org:8081/mqtt', { keepalive: 60 });

const Pronostico = () => {
    const [sensorSeleccionado, setSensorSeleccionado] = useState('');
    const [diaSeleccionado, setDiaSeleccionado] = useState('');
    const [resultadoPronostico, setResultadoPronostico] = useState({});

    const enviarPronostico = () => {
        const dia = parseInt(diaSeleccionado);
        if (dia > 8) {
            alert('No se puede realizar la proyección para un día mayor a 8.');
            return;
        }

        const message = JSON.stringify({ dia: diaSeleccionado, sensor: sensorSeleccionado });
        client.publish('F3G5/Pronostico', message, { qos: 0 }, (error) => {
            if (error) {
                console.error('Error al publicar:', error);
            } else {
                console.log('Datos enviados:', message);
            }
        });
    };

    client.subscribe('F3G5/Pronostico/Resultado', (error) => {
        if (error) {
            console.error('Error al suscribirse:', error);
        } else {
            console.log('Suscripción exitosa a F3G5/Pronostico/Resultado');
        }
    });

    client.on('message', (topic, message) => {
        if (topic === 'F3G5/Pronostico/Resultado') {
            const forecastData = JSON.parse(message.toString());
            setResultadoPronostico(forecastData);
            console.log('Pronóstico recibido:', forecastData);
        }
    });

    return (
        <div className="forecast-container">
            <div className="forecast-form-group">
                <label>Sensor:</label>
                <select className="forecast-select" value={sensorSeleccionado} onChange={(e) => setSensorSeleccionado(e.target.value)}>
                    <option value="">Seleccionar</option>
                    <option value="mq">MQ</option>
                    <option value="dht">DHT</option>
                    <option value="ldr">LDR</option>
                </select>
            </div>

            <div className="forecast-form-group">
                <label>Día:</label>
                <input
                    className="forecast-input"
                    type="number"
                    value={diaSeleccionado}
                    onChange={(e) => setDiaSeleccionado(e.target.value)}
                    min="1"
                    placeholder="Día"
                />
            </div>

            <button className="forecast-button" onClick={enviarPronostico}>Consultar</button>

            {/* Mostrar la card de pronostico */}
            {resultadoPronostico.sensor && (
                <PronosticoCard sensor={resultadoPronostico.sensor} valor={resultadoPronostico.valor} />
            )}
        </div>
    );

};

export default Pronostico;
