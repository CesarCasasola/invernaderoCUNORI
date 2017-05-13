<?php

	require "json-file-decode.class.php";

  //Conexion Base de datos
 $dbserver = "localhost";
 $dbuser = "root";
 $password = "";
 $dbname = "sensorial";

 $database = new mysqli($dbserver, $dbuser, $password, $dbname);


  //variables globales
    $codProy;

 if($database->connect_errno) {
   die("No se pudo conectar a la base de datos");
 }

  if(empty($_GET['temp1']) or empty($_GET['hume1'])  or empty($_GET['extrac1']) or empty($_GET['nebu1']) or empty($_GET['tiemp1'])
	or empty($_GET['temp2']) or empty($_GET['hume2'])  or empty($_GET['extrac2']) or empty($_GET['nebu2']) or empty($_GET['tiemp2'])
	or empty($_GET['temp3']) or empty($_GET['hume3'])  or empty($_GET['extrac3']) or empty($_GET['nebu3']) or empty($_GET['tiemp3'])
	or empty($_GET['temp4']) or empty($_GET['hume4'])  or empty($_GET['extrac4']) or empty($_GET['nebu4']) or empty($_GET['tiemp4'])){
		echo "ERROR - Cadena Vacia";
		exit();
	}

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

/*  echo "Datos Primero: ".$inttemp1." -|- ".$inthum1." -|- ".$intextrac1." -|- ".$intnebuli1." -|- ".$inttiem1;
  echo "<br>";
  echo "Datos Segundo: ".$inttemp2." -|- ".$inthum2." -|- ".$intextrac2." -|- ".$intnebuli2." -|- ".$inttiem2;
  echo "<br>";
  echo "Datos Tercero: ".$inttemp3." -|- ".$inthum3." -|- ".$intextrac3." -|- ".$intnebuli3." -|- ".$inttiem3;
  echo "<br>";
  echo "Datos Cuarto: ".$inttemp4." -|- ".$inthum4." -|- ".$intextrac4." -|- ".$intnebuli4." -|- ".$inttiem4;
    echo "<br>";
      echo "<br>";
        echo "<br>";
*/

  $queryconsul = "SELECT PROY_CODE FROM PROYECTO WHERE ACTIVO = 1";
  $resultact = $database->query($queryconsul);


  while ($registrProy = $resultact->fetch_array( MYSQLI_BOTH)) {
		# code...
    $codProy = $registrProy['PROY_CODE'];
  }

/*  echo "Codigo de Proyecto:  ".$codProy;*/

  $query = "INSERT INTO STATUS_RECORD (PROY_CODE, TEMPERATURA, HUMEDAD, EXTRACTORES, NEBULIZADORES, TIEMPO)
            VALUES ($codProy, '$inttemp1', '$inthum1', $intextrac1, $intnebuli1, '$inttiem1'),
            ($codProy, '$inttemp2', '$inthum2', $intextrac2, $intnebuli2, '$inttiem2'),
            ($codProy, '$inttemp3', '$inthum3', $intextrac3, $intnebuli3, '$inttiem3'),
            ($codProy, '$inttemp4', '$inthum4', $intextrac4, $intnebuli4, '$inttiem4')";

  $database->query($query) or mysql_error();

  $queryParam = "SELECT * FROM PARAMETROS WHERE PROY_CODE=".$codProy;
  $resultParam = $database->query($queryParam);

  /*echo "<br>";
  echo "<br>";
  echo "<br>";
  echo "<br>";*/

  while ( $registroParam = $resultParam->fetch_array( MYSQLI_BOTH)) {
    # code...
    $humedadMin = $registroParam['HUM_MIN'];
    $temperaturaMax = $registroParam['TEM_MAX'];

  }

  $response = array(
        "HumedadMin" => $humedadMin,
        "TemperaturaMax" => $temperaturaMax
        );

  $json_response = json_encode($response);
  echo $json_response;

  mysqli_close($database);
?>
