import React from 'react';
import { BrowserRouter as Router, Route, Routes } from 'react-router-dom';
import SideBar from './Components/SideBar';
import Dashboard from './Components/Dashboard';
import Pronostico from './Components/Pronostico';
import Clima from './Components/Clima';
import About from './Components/About';
import './App.css';

const App = () => {
  return (
    <Router>
      <div className='dashboard'>
        <SideBar />
        <div className="dashboard--content">
          <Routes>
            <Route path="/" element={<Dashboard />} />
            <Route path="/clima" element={<Clima />} />
            <Route path="/pronostico" element={<Pronostico />} />
            <Route path="/about" element={<About />} />
          </Routes>
        </div>
      </div>
    </Router>
  );
};

export default App;
