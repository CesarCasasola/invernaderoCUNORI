<?php

require "json-file-decode.class.php";

if (empty($_GET['codigo'])) {
	# code...
	exit();
}


ini_set('date.timezone','America/Guatemala');


//ini_set('date.timezone','Asia/Tokyo');

$anio = date("Y");
$mes = date("m");
$dia = date("j");
$hora = date("H");
$min = date("i");
$seg = date("s");
/*
echo "mes".$mes."\n";
echo "dia".$dia."\n";
echo "hora".$hora."\n";
echo "minuto".$min."\n";
echo "segundo".$seg."\n";
/*/

$response = array(
				"Anio" => $anio,
				"Mes" => $mes,
				"Dia" => $dia,
				"Hora" => $hora,
				"Minuto" => $min,
				"Segundo" => $seg
				);

$json_response = json_encode($response);
echo $json_response;
