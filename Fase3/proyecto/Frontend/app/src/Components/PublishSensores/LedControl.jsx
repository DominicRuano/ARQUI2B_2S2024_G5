import React, { useState } from 'react';
import mqtt from 'mqtt';
import { FaLightbulb } from "react-icons/fa";

const client = mqtt.connect('wss://test.mosquitto.org:8081/mqtt', { keepalive: 60 });

const LedControl = () => {
    const [isOn, setIsOn] = useState(false);

    const toggleLed = () => {
        const ledState = isOn ? 0 : 1;  // 0 = off, 1 = on
        const message = JSON.stringify({ LedState: ledState });
        client.publish('F3G5/LedControl', message, { qos: 0 }, (error) => {
            if (error) {
                console.error('Publish error:', error);
            } else {
                console.log('Published:', message);
                setIsOn(!isOn); 
            }
        });
    };

    return (
        <div>
            <h2></h2>
            <button onClick={toggleLed}>
                <FaLightbulb style={{ fontSize: '30px', color: isOn ? 'yellow' : 'white' }} />
            </button>
        </div>
    );
};

export default LedControl;
