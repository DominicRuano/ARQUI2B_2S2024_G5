import React from 'react';
import Content from './Content';
import '../Styles/Content.css';

import GasChart from './GraficasSensores/GChart';
import HumidityChart from './GraficasSensores/HumidityChart';
import LightChart from './GraficasSensores/LightChart';
import ProximityChart from './GraficasSensores/ProximityChart';
import TemperatureChart from './GraficasSensores/TemperatureChart';
import AlertaVehiculo from './GraficasSensores/AlertaVehiculo';

const Clima = () => {
    return (
        <div >
            <Content />
            <div className="grafica1">
                <GasChart />
            </div>
            <div className="grafica2">
                <HumidityChart />
            </div>
            <div className="grafica3">
                <LightChart />
            </div>
            <div className="grafica4">
                <ProximityChart />
            </div>
            <div className="grafica5">
                <TemperatureChart />
            </div>
            <div className="alerta">
                <AlertaVehiculo />
            </div>
        </div>
    );
};

export default Clima;
