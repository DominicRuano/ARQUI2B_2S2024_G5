CREATE DATABASE IF NOT EXISTS accesovehicular;
USE accesovehicular;

/* Tabla de catalogo de sensores */

DROP TABLE IF EXISTS sensor;

CREATE TABLE IF NOT EXISTS sensor
(
	sensor		SMALLINT	NOT NULL AUTO_INCREMENT,
	nombre		VARCHAR(50)	NOT NULL,
	descripcion	VARCHAR(80) NULL,
	PRIMARY KEY (sensor)
);

/* Tabla para almacenar el registro de las mediciones de los sensores */

CREATE TABLE IF NOT EXISTS medicion
(
	sensor	SMALLINT	NOT NULL,
	valor	FLOAT		NOT NULL,
	fecha	DATETIME 	NOT NULL,
	error	VARCHAR(80) NULL,
	FOREIGN KEY (sensor) REFERENCES sensor (sensor)
);

CREATE INDEX idxfecha ON medicion(fecha);

