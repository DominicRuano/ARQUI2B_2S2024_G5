import React, { useState, useEffect } from 'react';
import { Line } from 'react-chartjs-2';
import mqtt from 'mqtt';

import { CategoryScale } from 'chart.js'; 
import Chart from 'chart.js/auto';

Chart.register(CategoryScale);

const GasChart = () => {
    const [data, setData] = useState({ labels: [], datasets: [{ label: 'Co2', data: [] }] });
    
    useEffect(() => {
        const client = mqtt.connect('ws://localhost:8083/mqtt', { keepalive: 60 });

        
        client.on('connect', () => {
            client.subscribe('F3G5/calidadAire', (err) => {
                if (!err) {
                    console.log('Suscrito a F3G5/calidadAire');
                }
            });
        });
        
        client.on('message', (topic, message) => {
            const parsedMessage = JSON.parse(message.toString());
            setData((prevData) => ({
                ...prevData,
                labels: [...prevData.labels, new Date().toLocaleTimeString()],
                datasets: [{ label: 'Gas', data: [...prevData.datasets[0].data, parsedMessage.calidadAire] }]
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
                                text: '(ppm)'
                            }
                        }
                    }
                }} 
            />
        </div>
    );
};

export default GasChart;
