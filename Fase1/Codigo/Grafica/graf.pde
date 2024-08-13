import processing.serial.*;
import grafica.*;

GPlot plotTemperature, plotHumidity, plotCO2;
float temperature = 20;  // Temperatura inicial
float humidity = 50;      // Humedad inicial
float co2 = 10;          // Nivel de CO2 inicial
float[] temperatures = new float[50];  // Historial de temperaturas
float[] humidities = new float[50];    // Historial de humedades
float[] co2Levels = new float[50];     // Historial de niveles de CO2
int tempIndex = 0;
float TempMax = 100;
float HumidityMax = 100;
float CO2Max = 5000;       // Máximo valor esperado para CO2 en ppm

Serial myPort;  // Objeto Serial para la comunicación

void setup() {
  size(1000, 600);
  smooth();
  
  // Configuración del gráfico de temperatura
  plotTemperature = new GPlot(this);
  plotTemperature.setPos(100, 25);
  plotTemperature.setDim(150, 200);
  plotTemperature.getXAxis().setAxisLabelText("Time (s)");
  plotTemperature.getYAxis().setAxisLabelText("Temperature (°C)");
  plotTemperature.setTitleText("Temperature History");
  
  // Configuración del gráfico de humedad
  plotHumidity = new GPlot(this);
  plotHumidity.setPos(560, 25);  // Posición al lado del gráfico de temperatura
  plotHumidity.setDim(150, 200);
  plotHumidity.getXAxis().setAxisLabelText("Time (s)");
  plotHumidity.getYAxis().setAxisLabelText("Humidity (%)");
  plotHumidity.setTitleText("Humidity History");
  
  // Configuración del gráfico de CO2
  plotCO2 = new GPlot(this);
  plotCO2.setPos(100, 300);  // Posición debajo del gráfico de temperatura
  plotCO2.setDim(150, 200);
  plotCO2.getXAxis().setAxisLabelText("Time (s)");
  plotCO2.getYAxis().setAxisLabelText("CO2 (ppm)");
  plotCO2.setTitleText("CO2 History");
  
  String portName = Serial.list()[1]; 
  myPort = new Serial(this, portName, 9600);
}

void draw() {
  background(255);
  
  if (myPort.available() > 0) {
    String inData = myPort.readStringUntil('\n');
    if (inData != null) {
      inData = trim(inData); // Eliminar espacios en blanco
      String[] values = split(inData, ',');
      if (values.length == 3) {
        humidity = float(values[0]);
        temperature = float(values[1]);
        co2 = float(values[2]);
        //println("Humedad: ", humidity, " | Temperatura: ", temperature, " | CO2: ", co2);
      }
    }
  }
  
  TempGraph();
  humiGraph();
  co2Graph();

  delay(100);
}

void humiGraph(){
  // Dibuja Higrómetro
  drawHygrometer(humidity);
  
  // Dibujar el gráfico de la humedad
  updateHumidityData();
  plotHumidity.setPoints(createHumidityPoints());
  plotHumidity.defaultDraw();
}

void co2Graph() {
  // Dibujar el medidor de CO2
  drawCO2Meter();
  
  // Dibujar el gráfico de las ppm de CO2
  updateCO2Data();
  plotCO2.setPoints(createCO2Points());
  plotCO2.defaultDraw();
}

void TempGraph(){  
  // Dibujar el termómetro
  drawThermometer();
  
  // Dibujar el gráfico de la temperatura
  updateTemperatureData();
  plotTemperature.setPoints(createTemperaturePoints());
  plotTemperature.defaultDraw();
}

void drawThermometer() {
  fill(200);
  noStroke();
  
  // Termómetro
  rect(50, 50, 50, 200);
  
  // Elipse superior (parte redondeada)
  fill(200);
  ellipse(75, 50, 50, 25);
  
  // Mercurio
  float mercuryHeight = map(temperature, 0, TempMax, 0, TempMax*5);
  fill(map(temperature, 0, TempMax, 0, 255), 0, map(temperature, 0, TempMax, 255, 0));
  rect(50, 250 - mercuryHeight, 50, mercuryHeight);
  
  // Elipse superior (parte redondeada)
  noFill();              // Elimina el relleno de la elipse
  stroke(200);           // Define el color del contorno
  ellipse(75, 50, 49, 15);
  
  // Texto de temperatura
  fill(0);
  textAlign(CENTER, CENTER);
  text(nf(temperature, 1, 1) + "°C", 75, 260);
}

void drawHygrometer(float humidity) {
  float angle = map(humidity, 0, HumidityMax, 0, 2*PI);  // Mapea la humedad a un ángulo
  
  // Dibujar el fondo del medidor
  fill(200);
  ellipse(450, 160, 200, 200);
  
  // Dibujar la aguja
  stroke(0);
  strokeWeight(2);
  line(450, 160, 450 + cos(angle) * 80, 160 - sin(angle) * 80);
  
  // Dibujar las marcas
  for (int i = 0; i <= 8; i++) {
    float a = map(i, 0, 8, 0, 2*PI);
    line(450 + cos(a) * 80, 160 - sin(a) * 80, 450 + cos(a) * 90, 160 - sin(a) * 90);
  }
  
  // Etiqueta de humedad
  fill(0);
  textAlign(CENTER);
  text(int(humidity) + "%", 450, 280);
}

void updateTemperatureData() {
  // Desplazar los datos para mantener el historial
  for (int i = 0; i < temperatures.length - 1; i++) {
    temperatures[i] = temperatures[i + 1];
  }
  // Añadir la nueva temperatura
  temperatures[temperatures.length - 1] = temperature;
}

GPointsArray createTemperaturePoints() {
  GPointsArray points = new GPointsArray(temperatures.length);
  for (int i = 0; i < temperatures.length; i++) {
    points.add(i, temperatures[i]);
  }
  return points;
}

void updateHumidityData() {
  // Desplazar los datos para mantener el historial
  for (int i = 0; i < humidities.length - 1; i++) {
    humidities[i] = humidities[i + 1];
  }
  // Añadir la nueva humedad
  humidities[humidities.length - 1] = humidity;
}

GPointsArray createHumidityPoints() {
  GPointsArray points = new GPointsArray(humidities.length);
  for (int i = 0; i < humidities.length; i++) {
    points.add(i, humidities[i]);
  }
  return points;
}

void updateCO2Data() {
  // Desplazar los datos para mantener el historial
  for (int i = 0; i < co2Levels.length - 1; i++) {
    co2Levels[i] = co2Levels[i + 1];
  }
  // Añadir el nuevo nivel de CO2
  co2Levels[co2Levels.length - 1] = co2;
}

GPointsArray createCO2Points() {
  GPointsArray points = new GPointsArray(co2Levels.length);
  for (int i = 0; i < co2Levels.length; i++) {
    points.add(i, co2Levels[i]);
  }
  return points;
}

void drawCO2Meter() {
  fill(200);
  noStroke();
  
  // Base del medidor de CO2
  rect(50, 340, 50, 200);
  
  // Parte superior redondeada
  fill(200);
  ellipse(75, 340, 50, 25);
  
  // Nivel de CO2
  float co2Height = map(co2, 0, CO2Max, 0, 200);  // Usar el valor máximo de CO2 definido
  fill(map(co2, 0, CO2Max, 0, 255), 0, map(co2, 0, CO2Max, 255, 0));
  rect(50, 540 - co2Height, 50, co2Height);
  
  // Elipse superior (parte redondeada)
  noFill();
  stroke(200);
  ellipse(75, 50, 49, 15);
  
  // Texto de ppm de CO2
  fill(0);
  textAlign(CENTER, CENTER);
  text(co2 + " ppm", 75, 550);
}