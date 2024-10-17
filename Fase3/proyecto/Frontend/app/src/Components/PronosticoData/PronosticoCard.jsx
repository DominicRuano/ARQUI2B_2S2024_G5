import React from 'react';
import "../../Styles/Content.css";

const PronosticoCard = ({ sensor, valor }) => {
    const getIcon = (sensor) => {
        switch(sensor) {
            case 'dht':
                if (valor.temperatura > 35) {
                    return '🌡️'; // Muy caliente
                } else if (valor.temperatura > 25) {
                    return '🌞'; // Cálido
                } else if (valor.temperatura > 15) {
                    return '⛅'; // Templado
                } else {
                    return '❄️'; // Frío
                }
            case 'mq':
                if (valor.gas > 40000) {
                    return '💀'; // Alto CO2, posible peligro
                } else if (valor.gas > 20000) {
                    return '🌫️'; // CO2 moderado, algo alto
                } else {
                    return '🍃'; // Bajo CO2, aire limpio
                }
            case 'ldr':
                if (valor.luminosidad > 800) {
                    return '💡'; // Luz muy brillante
                } else if (valor.luminosidad > 400) {
                    return '🔆'; // Luz moderada
                } else if (valor.luminosidad > 100) {
                    return '🕯️'; // Luz tenue, como una vela
                } else {
                    return '🌚'; // Oscuridad total
                }
            default:
                return '❓'; 
        }
    };

    return (
        <div className="pronostico-card">
            <h3>{sensor.toUpperCase()}</h3>
            <div className="icon">
                {getIcon(sensor)}
            </div>
            {/* Verificar si tenemos temperatura y humedad para dht11 */}
            {sensor === 'dht' ? (
                <p>Temperatura: {valor.temperatura}°C Humedad: {valor.humedad}%</p>
            ) : sensor === 'mq' ? (
                <p>CO2: {valor.gas} ppm</p>                                              // Mostrar valor de gas para mq
            ) : (
                <p>Luminosidad: {valor.luminosidad} Lm</p>                               // Mostrar luminosidad para LDR
            )}
        </div>
    );
};

export default PronosticoCard;
