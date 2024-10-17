import React from 'react';
import '../Styles/Report.css';
import ContentHeader2 from './ContentHeader2';
import Forecast from './PronosticoData/Forecast';

const Pronostico = () => {
  return (
    <div>
      <ContentHeader2 />
      <div className="forecast">
        <Forecast />
      </div>
    </div>
  );
};

export default Pronostico;

