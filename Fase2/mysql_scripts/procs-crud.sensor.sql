USE accesovehicular;

DELIMITER $$

/* Creación de sensor */
DROP FUNCTION IF EXISTS insertarSensor;
CREATE FUNCTION insertarSensor(
	IN p_nombre			VARCHAR(50),
	IN p_descripcion	VARCHAR(80) = NULL
)
RETURNS SMALLINT 
BEGIN
	SELECT sensor INTO @sensor
	FROM sensor
	WHERE nombre = p_nombre);

	IF @sensor IS NULL
		INSERT INTO sensor(nombre, descripcion)
		VALUES (p_nombre, p_descripcion);
		SELECT LAST_INSERT_ID() INTO @sensor;
	END IF;

	RETURN @sensor;
END$$

DELIMITER ;
