import processing.serial.*;
import grafica.*;

GPlot plotTemperature, plotHumidity, plotCO2, plotColors, plotValues;
int maxPoints = 50;  // Máximo número de puntos en la gráfica
float temperature = 20;  // Temperatura inicial
float humidity = 50;      // Humedad inicial
float co2 = 10;          // Nivel de CO2 inicial
float rojo = 10;
float verde = 20;
float azul = 30;
float infrarojo = 0;
float[] temperatures = new float[50];  // Historial de temperaturas
float[] humidities = new float[50];    // Historial de humedades
float[] co2Levels = new float[50];     // Historial de niveles de CO2
float[] redValues = new float[50];    // Historial de valores para rojo
float[] greenValues = new float[50];  // Historial de valores para verde
float[] blueValues = new float[50];   // Historial de valores para azul
float[] infrarojoValues = new float[50];   // Historial de valores para infrarojo
int tempIndex = 0;
float TempMax = 100;
float HumidityMax = 100;
float CO2Max = 5000;       // Máximo valor esperado para CO2 en pp
float colorpordefecto = 0;
float temp = 0;

//LDR
float valorLuzSuavizado = 0;               // animacion ldr
PGraphics pg;                              // Dibuja una región específica
GPlot plotLuz;                             // Objeto para la gráfica

float tamanoLuna = 50;                    // Tamaño de la luna
float tamanoSol = 50;                     // Tamaño del sol
float[] lightLevels = new float[50];       // Historial nivel de luz para el gráfico

String estadoLDR = "apagado";              // Variable para el estado del LDR 
String estadoLDRAnterior = "apagado"; 
int valorLuz = 0 ;                      

Serial myPort;  // Objeto Serial para la comunicación

void setup() {
  size(1100, 610);                           //tamanio ventana principal     
  smooth();
  //println(Serial.list());
  
    // Configuración de la gráfica
  plotValues = new GPlot(this);
  plotValues.setPos(800, 200);  // Posición de la gráfica en la ventana
  plotValues.setDim(200, 200);  // Dimensiones de la gráfica
  plotValues.getXAxis().setAxisLabelText("Time (s)");
  plotValues.getYAxis().setAxisLabelText("Obstuido");
  plotValues.setTitleText("infrared Values");

  // Inicializar la gráfica con algunos valores
  for (int i = 0; i < maxPoints; i++) {
    infrarojoValues[i] = 0;  // Generar valores aleatorios iniciales
  }
  
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
  plotCO2.setPos(100, 320);  // Posición debajo del gráfico de temperatura
  plotCO2.setDim(150, 200);
  plotCO2.getXAxis().setAxisLabelText("Time (s)");
  plotCO2.getYAxis().setAxisLabelText("CO2 (ppm)");
  plotCO2.setTitleText("CO2 History");
  
  // configuracion del gráfico LDR
  configurarGraficoLuz();
  
    // Configuración del gráfico de colores (Rojo, Verde, Azul)
  /*plotColors = new GPlot(this);
  plotColors.setPos(800, 25);  // Posición de la gráfica en la ventana
  plotColors.setDim(200, 200);  // Dimensiones de la gráfica
  plotColors.getXAxis().setAxisLabelText("Sample Index");
  plotColors.getYAxis().setAxisLabelText("Color Value");
  plotColors.setTitleText("Red, Green, Blue Values");
  
    // Crear capas para cada color
  GPointsArray redPoints = new GPointsArray();
  GPointsArray greenPoints = new GPointsArray();
  GPointsArray bluePoints = new GPointsArray();
  
  // Rellenar puntos con valores aleatorios para rojo, verde y azul
  for (int i = 0; i < 50; i++) {
    redPoints.add(i, colorpordefecto);
    greenPoints.add(i, colorpordefecto);
    bluePoints.add(i, colorpordefecto);
  }
  
    // Añadir capas de color a la gráfica
  plotColors.addLayer("Red", redPoints);
  plotColors.addLayer("Green", greenPoints);
  plotColors.addLayer("Blue", bluePoints);
  
  // Configurar colores de las líneas de las capas
  plotColors.getLayer("Red").setLineColor(color(255, 0, 0));
  plotColors.getLayer("Green").setLineColor(color(0, 255, 0));
  plotColors.getLayer("Blue").setLineColor(color(0, 0, 255));
  */
  
  // Comentar y descomentar segun sea necesario.  String portName = "/dev/ttyACM0"; 
  //myPort = new Serial(this, portName, 9600);
}

void draw() {
  background(255);
  
  if (myPort.available() > 0) {
    String inData = myPort.readStringUntil('\n');
    if (inData != null) {
      inData = trim(inData); // Eliminar espacios en blanco
      String[] values = split(inData, ',');
      if (values.length == 5) {
        humidity = float(values[0]);
        temperature = float(values[1]);
       co2 = float(values[2]);
        valorLuz = int(values[3]);
        infrarojo = float(values[4]);
        println("Humedad: ", humidity, " | Temperatura: ", temperature, " | CO2: ", co2, " | Luz: ", valorLuz);
      }
    }
  }
  
  CantidadLuminosidad();  // procesa la grafica y animacion LDR
  TempGraph();
  humiGraph();
  co2Graph();
  plotValues.defaultDraw();
  actualizarGraficainfra(infrarojo);
  //plotColors.defaultDraw();
  //actualizarGraficacolor(rojo, verde, azul);

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
  float mercuryHeight = map(temperature, 0, TempMax, 0, TempMax*2);
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

// ========= LDR ===========
void CantidadLuminosidad() {
  // Actualizar el PGraphics con el método LuzAmbiente
  pg.beginDraw(); 
  LuzAmbiente();                              // Actualiza el PGraphics
  pg.endDraw();
  
  image(pg, 460, 330);                        // ====== Posición del recuadro del sol y la luna ====== x,y
  
  updateLDRData();                            // Actualizar el historial de LDR
  plotLuz.setPoints(lightLevelsToGPoints());  // Actualizar puntos de la gráfica
  plotLuz.beginDraw();
  plotLuz.drawBackground();
  plotLuz.drawBox();
  plotLuz.drawXAxis();
  plotLuz.drawYAxis();
  plotLuz.drawTitle();
  plotLuz.drawLines();                         // Dibujar la línea del historial de luz
  plotLuz.endDraw();
}

void configurarGraficoLuz() {
  
  pg = createGraphics(95, 230);               // ====== Tamaño del recuadro del sol y la luna ======= x,y
  
  // Configuración del gráfico de luz
  plotLuz = new GPlot(this);
  plotLuz.setPos(560, 320);                    // ====== Posición del gráfico ====== x,y
  plotLuz.setDim(150, 200);                    // ====== Dimensiones del gráfico ======
  plotLuz.getXAxis().setAxisLabelText("Tiempo (s)");
  plotLuz.getYAxis().setAxisLabelText("Nivel de Luz");
  plotLuz.setYLim(0, 1023);                    // Establecer los límites del eje Y
  plotLuz.setTitleText("Light Level History");
  plotLuz.setPoints(lightLevelsToGPoints());   // Cargar puntos iniciales
}

void LuzAmbiente() {
  valorLuzSuavizado = lerp(valorLuzSuavizado, valorLuz, 0.1); // Suaviza el valor de luz

  // Actualiza el estado del LDR
  actualizarEstadoLDR();
  
  float t = map(valorLuzSuavizado, 50, 100, 0, 1); // Mapea el valor de luz a un rango de 0 a 1 para interpolación

  // Colores de día y noche
  color fondoDia = color(135, 206, 250);          // Azul claro
  color fondoNoche = color(0, 0, 30);             // Negro
  color sol = color(255, 204, 0);                 // Amarillo
  color luna = color(255);                        // Blanco

  // Interpolación de colores
  color fondoActual = lerpColor(fondoNoche, fondoDia, t);
  color circuloActual = lerpColor(luna, sol, t);

  pg.background(fondoActual);                     // Dibuja el fondo en el recuadro

  float transition = smoothStep(0.4, 0.6, t);     // Transición suave
  float desplazamiento = lerp(50, 0, transition); // Desplazamiento para la media luna

  pg.fill(circuloActual);
  pg.noStroke();

  if (t < 0.5) { // Dibujar media luna en transición
    pg.ellipse(pg.width / 2, pg.height / 2, tamanoLuna, tamanoLuna); // Círculo principal para la luna
    pg.fill(fondoActual);                                            // Color del fondo para "recortar" la media luna
    pg.ellipse(pg.width / 2 + desplazamiento, pg.height / 2, tamanoLuna, tamanoLuna); // Círculo para recortar
  } else {
    pg.ellipse(pg.width / 2, pg.height / 2, tamanoSol, tamanoSol); // Dibuja círculo completo del sol
  }
}

// Función para una interpolación suave
float smoothStep(float edge0, float edge1, float x) {
  float t = constrain((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  return t * t * (3.0 - 2.0 * t);
}

void actualizarEstadoLDR() {
  // Define los rangos de valores para el estado del LDR
  float rangoInferior = 50; 
  float rangoSuperior = 100; 

  // Determina el estado actual del LDR
  String estadoLDRActual;
  if (valorLuzSuavizado >= rangoInferior && valorLuzSuavizado <= rangoSuperior) {
    estadoLDRActual = "encendido";
  } else {
    estadoLDRActual = "apagado";
  }

  
  if (!estadoLDRActual.equals(estadoLDRAnterior)) { // Actualiza solo si el estado ha cambiado
    estadoLDR = estadoLDRActual;
    estadoLDRAnterior = estadoLDRActual;
    //println("Estado del LDR cambiado a: " + estadoLDR);
  }
}

void updateLDRData() {
  for (int i = 0; i < lightLevels.length - 1; i++) {
    lightLevels[i] = lightLevels[i + 1];
  }
  lightLevels[lightLevels.length - 1] = valorLuzSuavizado;
}

// Convierte el historial de luz a un arreglo de GPoints para la gráfica
GPointsArray lightLevelsToGPoints() {
  GPointsArray points = new GPointsArray(lightLevels.length);
  for (int i = 0; i < lightLevels.length; i++) {
    points.add(i, lightLevels[i]);
  }
  return points;
}

void actualizarGrafica(float r, float g, float b) {
  // Desplazar los datos para mantener el historial
  for (int i = 0; i < 50 - 1; i++) {
    redValues[i] = redValues[i + 1];
    greenValues[i] = greenValues[i + 1];
    blueValues[i] = blueValues[i + 1];
  }
  
  // Añadir los nuevos valores al final del historial
  redValues[50 - 1] = r;
  greenValues[50 - 1] = g;
  blueValues[50 - 1] = b;

  // Crear puntos para la gráfica usando los valores del historial
  GPointsArray redPoints = new GPointsArray();
  GPointsArray greenPoints = new GPointsArray();
  GPointsArray bluePoints = new GPointsArray();
  
  for (int i = 0; i < 50; i++) {
    redPoints.add(i, redValues[i]);
    greenPoints.add(i, greenValues[i]);
    bluePoints.add(i, blueValues[i]);
  }
  
  // Actualizar los puntos en la gráfica
  plotColors.getLayer("Red").setPoints(redPoints);
  plotColors.getLayer("Green").setPoints(greenPoints);
  plotColors.getLayer("Blue").setPoints(bluePoints);
}

void actualizarGraficainfra(float nuevoValor) {
  // Desplazar los datos para mantener el historial y eliminar el valor más antiguo
  for (int i = 0; i < maxPoints - 1; i++) {
    infrarojoValues[i] = infrarojoValues[i + 1];
  }
  
  // Añadir el nuevo valor al final del historial
  infrarojoValues[maxPoints - 1] = nuevoValor;

  // Crear puntos para la gráfica usando los valores del historial
  GPointsArray points = new GPointsArray();
  
  for (int i = 0; i < maxPoints - 1; i++) {
    points.add(i, infrarojoValues[i]);
  }
  
  // Actualizar los puntos en la gráfica
  plotValues.setPoints(points);
  
  // Ajustar automáticamente la escala de los ejes Y en la gráfica
  float minValue = min(infrarojoValues);
  float maxValue = max(infrarojoValues);
  plotValues.setYLim(minValue - 10, maxValue + 10);  // Añadir un pequeño margen
}


void keyPressed() {
  if (key == ESC) {
    key = 0; // Ignora la tecla ESC para que no cierre el programa
  }
  // Puedes agregar otros comportamientos si es necesario
}
