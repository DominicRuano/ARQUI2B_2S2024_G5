import React, { useEffect } from 'react';
import { toast, ToastContainer } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';
import mqtt from 'mqtt';

const AlertaVehiculo = () => {
  useEffect(() => {
    const client = mqtt.connect('wss://test.mosquitto.org:8081/mqtt', { keepalive: 60 });

    client.on('connect', () => {
      client.subscribe('F3G5/Accesos', (err) => {
        if (!err) {
          console.log('Suscrito a F3G5/Accesos');
        }
      });
    });

    client.on('message', (topic, message) => {
      toast.success('Vehículo ingresado');
      console.log('Vehículo ingresado');
    });

    return () => client.end();
  }, []);

  return <ToastContainer />;
};

export default AlertaVehiculo;
