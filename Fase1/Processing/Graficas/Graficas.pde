import grafica.*;

GPlot plotTemperature, plotHumidity;
float temperature = 20;  // Temperatura inicial
float humidity = 50;      // Humedad inicial
float[] temperatures = new float[50];  // Historial de temperaturas
float[] humidities = new float[50];    // Historial de humedades
int tempIndex = 0;
float TempMax = 100;
float HumidityMax = 100;

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
}

void draw() {
  background(255);
  TempGraph();
  humiGraph();

  delay(100);
}

void humiGraph(){
  // Dibuja Hydrometro
  drawHygrometer(humidity);
  
  // Dibujar el gráfico de la humedad
  updateHumidityData();
  plotHumidity.setPoints(createHumidityPoints());
  plotHumidity.defaultDraw();
  
  // Actualizar los datos de Humedad
  humidity = map(mouseX, 0, width, 0, 100);     // Simular humedad con la posición del mouse
}

void drawHygrometer(float humidity) {
  float angle = map(humidity, 0, 100, 0, 2*PI);  // Mapea la humedad a un ángulo
  
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



void TempGraph(){  
  // Dibujar el termómetro
  drawThermometer();
  
  // Dibujar el gráfico de la temperatura
  updateTemperatureData();
  plotTemperature.setPoints(createTemperaturePoints());
  plotTemperature.defaultDraw();
  
  // Actualizar los datos de temperatura
  temperature = map(mouseY, height, 0, 0, 40);  // Simular temperatura con la posición del mouse
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
