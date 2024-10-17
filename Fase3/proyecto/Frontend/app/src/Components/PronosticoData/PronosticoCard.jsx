import React from 'react';
import "../../Styles/Content.css";

const PronosticoCard = ({ sensor, valor }) => {
    const getIcon = (sensor) => {
        switch(sensor) {
            case 'dht':
                return valor.temperatura > 30 ? '🌞' : '🌥️';    // Sol si temperatura > 30°C
            case 'mq':
                return valor.gas > 100 ? '🔥' : '💨';           // Fuego si gas/Co2 > 100 ppm
            case 'ldr':
                return valor.luminosidad > 200 ? '☀️' : '🌑';   // Alta o baja luminosidad
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
                <p>CO2: {valor.gas} ppm</p>                                              // Mostrar valor de gas para mq5
            ) : (
                <p>Luminosidad: {valor.luminosidad} Lm</p>                               // Mostrar luminosidad para LDR
            )}
        </div>
    );
};

export default PronosticoCard;
