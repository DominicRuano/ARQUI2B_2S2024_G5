import React from 'react';
import { TiWeatherWindyCloudy } from "react-icons/ti";
import { BiHomeAlt, BiSolidReport, BiInfoCircle, BiLinkExternal } from "react-icons/bi";
import { TiWeatherSunny } from "react-icons/ti";
import { Link } from 'react-router-dom';
import '../Styles/SideBar.css';

const SideBar = () => {
  return (
    <div className='menu'>
      <div className="logo">
        <TiWeatherWindyCloudy className='logo-icon'/>
        <h2>WeatherApp</h2>
      </div>
      <div className="menu--list">
      <Link to="/" className="item">
          <BiHomeAlt className='icon'/>
          <span>Dashboard</span>
        </Link>
        <Link to="/Clima" className="item">
          <TiWeatherSunny className='icon'/>
          <span>Clima</span>
        </Link>
        <Link to="/pronostico" className="item">
          <BiSolidReport className='icon'/>
          <span>Pronóstico</span>
        </Link>
        <a href="https://arqui2.grafana.net/public-dashboards/ac5b0f21d075458589f3108c6ddb2847" target="_blank" className="item">
          <BiLinkExternal className='icon'/>
          <span>Grafana</span>
        </a>
        <Link to="/about" className="item">
          <BiInfoCircle className='icon'/>
          <span>About</span>
        </Link>
      </div>
    </div>
  );
};

export default SideBar;