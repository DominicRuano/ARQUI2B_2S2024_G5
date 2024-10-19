import { FaRoadBarrier } from "react-icons/fa6";
import React from 'react';
import mqtt from 'mqtt';


const client = mqtt.connect('wss://test.mosquitto.org:8081/mqtt', { keepalive: 60 });


const ServoControl = () => {
    const publishControl = () => {
        const message = JSON.stringify({ ControlServo: 90 });
        client.publish('F3G5/Control', message, { qos: 0 }, (error) => {
            if (error) {
                console.error('Publish error:', error);
            } else {
                console.log('Published:', message);
            }
        });
    };

    return (
        <div>
            <h2></h2>
            <button onClick={publishControl}><FaRoadBarrier style={{ fontSize: '30px' }} /></button>
        </div>
    );
};

export default ServoControl;
