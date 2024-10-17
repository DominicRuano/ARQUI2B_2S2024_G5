import React from 'react';
import '../Styles/Content.css';
import ContentHeader3 from './ContentHeader3';
import ServoControl from './PublishSensores/ServoControl';
import LedControl from './PublishSensores/LedControl';
import Login from './PublishSensores/Login.jsx';

const Dashboard = () => {
    return (
        <div >
            <ContentHeader3 />
            <div className="boton">
                <ServoControl />
            </div>
            <div className="boton2">
                <LedControl />
            </div>
            <div className="datos">
                <Login />
            </div>
        </div>
            
    );
};

export default Dashboard;
