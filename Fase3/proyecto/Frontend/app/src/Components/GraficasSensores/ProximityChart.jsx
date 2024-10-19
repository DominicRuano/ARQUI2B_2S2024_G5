import React, { useState, useEffect } from 'react';
import { Line } from 'react-chartjs-2';
import mqtt from 'mqtt';

import { CategoryScale } from 'chart.js'; 
import Chart from 'chart.js/auto';

Chart.register(CategoryScale);

const ProximityChart = () => {
    const [data, setData] = useState({ labels: [], datasets: [{ label: 'Proximidad', data: [] }] });
    
    useEffect(() => {
        const client = mqtt.connect('wss://test.mosquitto.org:8081/mqtt', { keepalive: 60 });

        client.on('connect', () => {
            client.subscribe('F3G5/proximidad', (err) => {
                if (!err) {
                    console.log('Suscrito a F3G5/proximidad');
                }
            });
        });
        
        client.on('message', (topic, message) => {
            const parsedMessage = JSON.parse(message.toString());
            setData((prevData) => ({
                ...prevData,
                labels: [...prevData.labels, new Date().toLocaleTimeString()],
                datasets: [{ label: 'Proximidad', data: [...prevData.datasets[0].data, parsedMessage.proximidad] }]
            }));
        });
        
        return () => client.end();
    }, []);

    return (
        <div>
            <Line 
                data={data} 
                options={{
                    scales: {
                        y: {
                            title: {
                                display: true,
                                text: '(cm)'
                            }
                        }
                    }
                }} 
            />
        </div>
    );
};

export default ProximityChart;
