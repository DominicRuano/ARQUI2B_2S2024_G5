USE accesovehicular;


DROP PROCEDURE IF EXISTS insertarMedicion;
DELIMITER $$
/* Creación de sensor */
CREATE PROCEDURE insertarMedicion(
	IN p_nombre	VARCHAR(50),
	IN p_valor	FLOAT,
	IN p_fecha	DATETIME,
	IN p_error	VARCHAR(80)
)
BEGIN
	DECLARE codigo SMALLINT;

	SET codigo = (SELECT sensor FROM sensor WHERE nombre = p_nombre);
    
	IF codigo IS NULL THEN
		INSERT INTO sensor(nombre)
		VALUES (p_nombre);
		SET codigo = LAST_INSERT_ID();
	END IF;

	IF p_fecha IS NULL THEN
		SET p_fecha = NOW();
	END IF;

	IF p_error IS NULL THEN
		SET p_error = "";
	END IF;

	INSERT INTO medicion(sensor, valor, fecha, error)
	VALUES (codigo, p_valor, p_fecha, p_error);
    
	SELECT ROW_COUNT() AS respuesta;

END$$

DELIMITER ;

DROP PROCEDURE IF EXISTS obtenerMediciones;
DELIMITER $$
/* Obtener listado de valores almacenados segun un rango de fecha */
CREATE PROCEDURE obtenerMediciones(
	IN p_finicio DATETIME,
	IN p_ffin	 DATETIME
	)
BEGIN
		SELECT nombre, valor, fecha, error
		FROM sensor s
        JOIN medicion m ON s.sensor = m.sensor
        WHERE fecha >=  p_finicio
		AND fecha <= p_ffin;
END$$
DELIMITER ;