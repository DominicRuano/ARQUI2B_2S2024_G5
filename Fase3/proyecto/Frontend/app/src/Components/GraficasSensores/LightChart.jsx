import React, { useState, useEffect } from 'react';
import { Line } from 'react-chartjs-2';
import mqtt from 'mqtt';

import { CategoryScale } from 'chart.js'; 
import Chart from 'chart.js/auto';

Chart.register(CategoryScale);

const LightChart = () => {
    const [data, setData] = useState({ labels: [], datasets: [{ label: 'Luminosidad', data: [] }] });
    
    useEffect(() => {
        const client = mqtt.connect('ws://localhost:8083/mqtt', { keepalive: 60 });

        client.on('connect', () => {
            client.subscribe('F3G5/luminosidad', (err) => {
                if (!err) {
                    console.log('Suscrito a F3G5/luminosidad');
                }
            });
        });
        
        client.on('message', (topic, message) => {
            const parsedMessage = JSON.parse(message.toString());
            setData((prevData) => ({
                ...prevData,
                labels: [...prevData.labels, new Date().toLocaleTimeString()],
                datasets: [{ label: 'Luminosidad', data: [...prevData.datasets[0].data, parsedMessage.luminosidad] }]
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
                                text: '(Lumen)'
                            }
                        }
                    }
                }} 
            />
        </div>
    );
};

export default LightChart;
