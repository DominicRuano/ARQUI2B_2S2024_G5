import React, { useState, useEffect } from 'react';
import { Line } from 'react-chartjs-2';
import 'chart.js/auto';
import { TextField, Button, Grid, Box, Typography } from '@mui/material';
import './App.css';

// Para renderizar los gráficos de cada sensor
const ChartComponent = ({ data, title }) => {
  return (
    <div className="chart-container">
      <Typography variant="h6">{title}</Typography>
      <Line
        data={{
          labels: data.map(item => item.time),
          datasets: [
            {
              label: 'Sensor Value',
              data: data.map(item => item.value),
              borderColor: 'rgba(75,192,192,1)',
              fill: false,
            },
          ],
        }}
        options={{
          plugins: {
            legend: {
              display: false,
            },
          },
        }}
      />
    </div>
  );
};

function App() {
  const [startTime, setStartTime] = useState('');
  const [endTime, setEndTime] = useState('');
  const [sensorData, setSensorData] = useState([[], [], [], [], []]);
                            // Tempearatura, Humedad, Luminosidad, CO2, Infrarrojo

  const fetchSensorData = async () => {
    /* 
    /// Aqui se obtendría la data de el API usando startTime y endTime como parámetros (Codigo comentado solo para tener un ejemplo)
    const response = await fetch(
      `http://your-api-url.com/sensors?start=${startTime}&end=${endTime}`
    );
    const data = await response.json();
    
    /// sumiendo que data es un array con 5 arrays de sensores
    setSensorData(data);
    */
  };

  const sensorNames = [
    'Temperatura',
    'Humedad',
    'Luminosidad',
    'CO2',
    'Infrarrojo',
  ];

  // Función para generar datos ficticios
  const generateFakeData = () => {
    const fakeData = [];
    const now = new Date();
    
    // Generar datos para un rango de 100 puntos de tiempo
    for (let i = 0; i < 100; i++) {
      fakeData.push({
        time: new Date(now.getTime() - i * 60000).toLocaleTimeString(), // cada minuto
        value: Math.floor(Math.random() * 10000), // valor aleatorio entre 0 y 10000
      });
    }
    return fakeData.reverse(); // invertir para que los datos estén en orden ascendente de tiempo
  };

  // Función para generar datos ficticios para todos los sensores
  const generateFakeSensorData = () => {
    const fakeSensorData = [];
    for (let i = 0; i < 5; i++) {
      fakeSensorData.push(generateFakeData());
    }
    setSensorData(fakeSensorData);
  };


  const handleFetchData = () => {
    if (startTime && endTime) {
      console.log('Start Time:', startTime);
      console.log('End Time:', endTime);
      // Aquí iría la lógica de fetch, por ejemplo:
      generateFakeSensorData();
      // fetchSensorData();
    } else {
      console.log('Please select both start and end times.');
    }
  };

  return (
    <div className="App">
      <header className="App-header">
        <Typography variant="h4" gutterBottom>
          Sensor Data
        </Typography>

        <Grid container spacing={2} justifyContent="center" alignItems="center">
          <Grid item>
            <TextField
              label="Fecha Inicio"
              type="datetime-local"
              value={startTime}
              onChange={(e) => setStartTime(e.target.value)}
              InputLabelProps={{
                shrink: true,
              }}
            />
          </Grid>
          <Grid item>
            <TextField
              label="Fecha Fin"
              type="datetime-local"
              value={endTime}
              onChange={(e) => setEndTime(e.target.value)}
              InputLabelProps={{
                shrink: true,
              }}
            />
          </Grid>
          <Grid item>
            <Button variant="contained" onClick={handleFetchData}>
              Obtener Datos
            </Button>
          </Grid>
        </Grid>

        <Box mt={4} className="grid-container">
            {sensorData.map((data, index) => (
              <Grid item xs={12} sm={6} md={4} key={index}>
                <ChartComponent data={data} title={sensorNames[index]} />
              </Grid>
            ))}
        </Box>
      </header>
    </div>
  );
}

export default App;
