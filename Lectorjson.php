<?php

require "json-file-decode.class.php";


if(empty($_GET['temp1']) or empty($_GET['hume1'])  or empty($_GET['extrac1']) or empty($_GET['nebu1']) or empty($_GET['tiemp1']) or empty($_GET['temp2']) or empty($_GET['hume2'])  or empty($_GET['extrac2']) or empty($_GET['nebu2']) or empty($_GET['tiemp2']) or empty($_GET['temp3']) or empty($_GET['hume3'])  or empty($_GET['extrac3']) or empty($_GET['nebu3']) or empty($_GET['tiemp3']) or empty($_GET['temp4']) or empty($_GET['hume4'])  or empty($_GET['extrac4']) or empty($_GET['nebu4']) or empty($_GET['tiemp4'])){
	echo "ERROR";
	exit();

}

$codProy;

$inttemp1 =(float)$_GET['temp1'];
$inthum1 =(float)$_GET['hume1'];
$intextrac1 = $_GET['extrac1'];
$intnebuli1 = $_GET['nebu1'];
$inttiem1 = $_GET['tiemp1'];

$inttemp2 =(float)$_GET['temp2'];
$inthum2 =(float)$_GET['hume2'];
$intextrac2 = $_GET['extrac2'];
$intnebuli2 = $_GET['nebu2'];
$inttiem2 = $_GET['tiemp2'];

$inttemp3 =(float)$_GET['temp3'];
$inthum3 =(float)$_GET['hume3'];
$intextrac3 = $_GET['extrac3'];
$intnebuli3 = $_GET['nebu3'];
$inttiem3 = $_GET['tiemp3'];

$inttemp4 =(float)$_GET['temp4'];
$inthum4 =(float)$_GET['hume4'];
$intextrac4 = $_GET['extrac4'];
$intnebuli4 = $_GET['nebu4'];
$inttiem4 = $_GET['tiemp4'];

//insertar a Mysql
 $dbserver = "127.0.0.1";
  $dbuser = "invernadero";
  $password = "123456";
  $dbname = "sensorial";
 
  $database = new mysqli($dbserver, $dbuser, $password, $dbname);

  if($database->connect_errno) {
    die("No se pudo conectar a la base de datos");
  }

//Seleccion de datos historicos 
  	$queryconsul = "SELECT PROY_CODE FROM PROYECTO WHERE ACTIVO = 1";
	$resultact = $database->query($queryconsul);

	while ($registrProy = mysqli_fetch_array($resultact)) {
		# code...
		$query = 'INSERT INTO STATUS_RECORD (PROY_CODE, TEMPERATURA, HUMEDAD, EXTRACTORES, NEBULIZADORES, TIEMPO) VALUES ('.$registrProy['PROY_CODE'].', '.$inttemp1.', '.$inthum1.', '.$intextrac1.', '.$intnebuli1.', '.$inttiem1.') ';     // Esta linea hace la consulta 
    	$result = $database->query($query);  

    	$query1 = 'INSERT INTO STATUS_RECORD (PROY_CODE, TEMPERATURA, HUMEDAD, EXTRACTORES, NEBULIZADORES, TIEMPO) VALUES ('.$registrProy['PROY_CODE'].', '.$inttemp2.', '.$inthum2.', '.$intextrac2.', '.$intnebuli2.', '.$inttiem2.') ';     // Esta linea hace la consulta 
    	$result1 = $database->query($query1); 

    	$query2 = 'INSERT INTO STATUS_RECORD (PROY_CODE, TEMPERATURA, HUMEDAD, EXTRACTORES, NEBULIZADORES, TIEMPO) VALUES ('.$registrProy['PROY_CODE'].', '.$inttemp3.', '.$inthum3.', '.$intextrac3.', '.$intnebuli3.', '.$inttiem3.') ';     // Esta linea hace la consulta 
    	$result2 = $database->query($query2);

    	$query3 = 'INSERT INTO STATUS_RECORD (PROY_CODE, TEMPERATURA, HUMEDAD, EXTRACTORES, NEBULIZADORES, TIEMPO) VALUES ('.$registrProy['PROY_CODE'].', '.$inttemp4.', '.$inthum4.', '.$intextrac4.', '.$intnebuli4.', '.$inttiem4.') ';     // Esta linea hace la consulta 
    	$result3 = $database->query($query3);  

    	if($result == TRUE || $result1 == TRUE || $result2 == TRUE || $result3 == TRUE ){
		
		} else {
		echo "Error: ".$sql."<br>".$result1->error;
		}
    	$codProy = $registrProy['PROY_CODE'];
	}
	
    

	//echo "Numero: ".$nu1."<br>";

	$queryParam = "SELECT * FROM PARAMETROS WHERE PROY_CODE=".$codProy;
	$resultParam = $database->query($queryParam);

	while ( $registroParam = mysqli_fetch_array($resultParam)) {
		# code...
	
	$response = array(
				"HumedadMiin" => $resultParam['HUM_MIN'],
				"TemperaturaMax" => $resultParam['TEM_MAX']
				);

	$json_response = json_encode($response);
	echo $json_response;

	}
	


$database->close();