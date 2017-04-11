#include "DHT.h"
#include <ArduinoJson.h>

String ssid = "CRed";
String password = "a2b4c6d8";

String server = "192.168.43.130"; //IP del server
String uri = "/invernadero/Lectorjson.php"; //uri de php que responde a request de arduino


float temp, hum;            //para recibir valores de medicion
String temperatura, humedad;//para el paso de parametros en peticion

#define DHTPIN 2     // Pin donde está conectado el sensor
#define DHTTYPE DHT22   // Sensor DHT22
DHT dht(DHTPIN, DHTTYPE); //variable tipo DHT22


#define pinNebulizadores 3
#define pinVentiladores 4


void setup() {
  Serial.begin(115200);  //comunicacion arduino-pc
  Serial1.begin(115200); /*comunicacion TTL con  ESP8266
                          Serial1  (RX=19, TX=18)*/
                          
  pinMode(pinNebulizadores, OUTPUT);//para encender nebulizadores
  pinMode(pinVentiladores, OUTPUT);//para encender ventiladores
                          
  iniciar();
}


void iniciar(){
  delay(1000);
  String connStr = "AT+CWJAP=\""+ssid+"\",\""+password+"\"";//comando AT para conectarse a una red
  Serial1.println(connStr);
  delay(5000);
}


void loop() {

/*Enviar datos desde la consola serial al Serial1 ESp y mostrar
 lo enviado por el ESP a nuestra consola.*/

 if(Serial1.available()){
    Serial.write(Serial1.read());
 }
 
 if(Serial.available()){
    Serial1.write(Serial.read());
 }

 //peticiones
 httpget();
 delay(30000);
  
}

void leerDHT(){//lee del DHT la temperatura y humedad y luego construye el objeto arbol en memoria

    //DHT
    temp = dht.readTemperature();//lectura de temperatura en grados Celcius
    hum = dht.readHumidity();//lectura de humedad
    /*temp = 20;
    hum = 40;*/
    temperatura = (String)temp;
    humedad = (String)hum;  
}


void httpget(){ 
  
  leerDHT();
  uri = "/invernadero/Lectorjson.php?temperatura=" + temperatura + "&humedad=" + humedad;
  
  //AT command para iniciar conexion como cliente tipo TCP por el puerto 80
  Serial1.println("AT+CIPSTART=\"TCP\",\""+ server +"\",80");
    Serial.println("AT+CIPSTART=\"TCP\",\""+ server +"\",80");//echo del comando anterior

  if(Serial1.find("OK")){//respuesta en caso de establecer conexion TCP exitosa
      Serial.println("Conexion TCP lista.");
  }

  

  String getRequest =
  
  "GET " + uri + " HTTP/1.0\r\n" +
  
  "Host: " + server + "\r\n\r\n";
  
  String sendCmd = "AT+CIPSEND=";//comando para enviar datos, seguido se debe escribir la longitud de datos a enviarse, maximo 2048

  Serial1.print(sendCmd);
    Serial.print(sendCmd);//echo

  Serial1.println(getRequest.length());//longitud de los datos que se enviaran, para completar comando AT+CIPSEND
    Serial.println(getRequest.length());//echo

  delay(1000);

  if(Serial1.find(">")){//el comando CIPSEND retorna ">" despues de ejecutarse y esta listo para enviar datos a continuacion
          Serial.println("Sending....");
          Serial1.print(getRequest);
          Serial.print(getRequest);
          

          
          if(Serial1.find("SEND OK")){//mensaje que se recibe al enviar exitosamente los datos
               Serial.println("SEND OK");
               

               String result = "";//contendra el json
               boolean httpBody = false;   //bandera que indica que parte del response se esta leyendo        
               while(Serial1.available()){ //leyendo respuesta
                  String tmp = Serial1.readString();
                  result = tmp.substring(tmp.indexOf('{'), tmp.indexOf('}')+1);
                  
                                   
                }
                int tam = result.length()+1;
                char json[tam];
                result.toCharArray(json, tam);
                StaticJsonBuffer<JSON_OBJECT_SIZE(2)> jsonBuffer;
                JsonObject& root = jsonBuffer.parseObject(json);
                if(!root.success()){
                    Serial.println("No se pudo parsear el json.");
                  }else{
                        int ventiladores = root["Ventiladores"];
                        int nebulizadores = root["Nebulizadores"];
                        Serial.println("Ventiladores: "+ (String)ventiladores + " Nebulizadores : "+(String)nebulizadores);
                        actuadores(ventiladores, nebulizadores);
                    }
                
        
             //cerrar conexion
             Serial1.println("AT+CIPCLOSE");//cierra la conexion TCP o UDP
            }
  }
  
  
}


void actuadores(int ventiladores, int nebulizadores){
      if(ventiladores == 1 && nebulizadores == 1){
          digitalWrite(pinNebulizadores, HIGH);
          delay(15000);//enciende nebulizadores por 15 segundos
          digitalWrite(pinNebulizadores, LOW);
          digitalWrite(pinVentiladores, HIGH);
          delay(15000);//enciende ventiladores por 15 segundos
          digitalWrite(pinVentiladores, LOW);
         }else if(ventiladores == 1 && nebulizadores == 0){
            digitalWrite(pinVentiladores, HIGH);
            delay(15000);//enciende nebulizadores por 15 segundos
            digitalWrite(pinVentiladores, LOW);
            delay(15000);//completando los 30 segundos
          }else if(ventiladores == 0 && nebulizadores == 1){
              digitalWrite(pinNebulizadores, HIGH);
              delay(15000);//enciende ventiladores por 15 segundos
              digitalWrite(pinNebulizadores, LOW);
              delay(15000);//completando los 30 segundos
            }else{
              delay(30000);//equivalente a 30 segundos 
              }           
  }

