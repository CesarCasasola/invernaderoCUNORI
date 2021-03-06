DROP DATABASE IF EXISTS sensorial;
CREATE DATABASE sensorial;
USE sensorial;
CREATE TABLE PROYECTO(
	PROY_CODE INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
	NOMBRE VARCHAR(30),
	DESCRIPTION VARCHAR(100),
	ACTIVO TINYINT(1)
);

CREATE TABLE STATUS_RECORD(
    ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    PROY_CODE INT NOT NULL,
    TEMPERATURA DECIMAL(4, 2),
    HUMEDAD DECIMAL(4, 2),
    EXTRACTORES INT,
    NEBULIZADORES INT,
    TIEMPO DATETIME,
    FOREIGN KEY(PROY_CODE) REFERENCES PROYECTO(PROY_CODE)
);

CREATE TABLE PARAMETROS(
	PROY_CODE INT NOT NULL,
	    TEM_MAX DECIMAL,
	    HUM_MIN DECIMAL,
	    FOREIGN  KEY(PROY_CODE) REFERENCES PROYECTO(PROY_CODE)
);

INSERT INTO PROYECTO(NOMBRE, DESCRIPTION, ACTIVO)
		VALUES('Prueba', 'Prueba', 1);
        
        
INSERT INTO PARAMETROS VALUES(1, 25.0, 40.0);



