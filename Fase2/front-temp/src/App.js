import React, { useState } from 'react';
import { Line } from 'react-chartjs-2';
import 'chart.js/auto';
import { TextField, Button, Grid, Box, Typography } from '@mui/material';
import axios from 'axios';
import './App.css';

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

  const sensorNames = [
    'Temperatura',
    'Humedad',
    'Luminosidad',
    'CO2',
    'Infrarrojo',
  ];

    // Función para parsear los datos
    const parseSensorData = (data) => {
      // Crear un arreglo vacío para cada sensor
      const humedadData = [];
      const tempData = [];
      const gasData = [];
      const luzData = [];
      const infraData = [];
  
      // Recorrer los datos originales y distribuirlos según el tipo de sensor
      data.forEach((item) => {
        const { nombre, valor, fecha } = item;
  
        const parsedData = { time: new Date(fecha).toLocaleTimeString(), value: valor };
  
        switch (nombre) {
          case 'HUMEDAD':
            humedadData.push(parsedData);
            break;
          case 'TEMP':
            tempData.push(parsedData);
            break;
          case 'GAS':
            gasData.push(parsedData);
            break;
          case 'LUZ':
            luzData.push(parsedData);
            break;          
          case 'INFRA':
            infraData.push(parsedData);
            break;
          default:
            console.log('Sensor no reconocido:', nombre);
            break;
        }
      });
  
      setSensorData([humedadData, tempData, gasData, [], []]); // Asumimos que hay 5 sensores en total
    };

  const handleFetchData = () => {
    if (startTime && endTime) {
      axios
        .get('http://localhost:3002/datos', {
          params: {
            startDate: startTime,
            endDate: endTime,
          },
        })
        .then((response) => {
          //console.log(startTime);
          //console.log(endTime);
          //console.log(response.data);
          parseSensorData(response.data);
        })
        .catch((error) => {
          console.error('Error fetching data:', error);
        });
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
