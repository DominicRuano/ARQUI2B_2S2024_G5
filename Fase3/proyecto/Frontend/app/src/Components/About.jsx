import React from 'react';
import '../Styles/About.css';

const About = () => {
    return (
        <div className="about-container">
            <h1 className="header--title">WeatherApp</h1>
            <div className="about-content">
                <p>
                    Ha sido desarrollada para proporcionar un monitoreo 
                    continuo y preciso de las condiciones meteorológicas locales. 
                    Este proyecto aborda los desafíos del cambio climático al recopilar datos en 
                    tiempo real sobre temperatura, humedad, luminosidad, calidad 
                    del aire ademas del pronostico del tiempo.
                </p>
                <p>ACYE2-Grupo05</p>
            </div>
        </div>
    );
};

export default About;
