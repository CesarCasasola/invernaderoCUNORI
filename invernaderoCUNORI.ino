#include <DHT.h>
#include <DHT_U.h>
#include <Time.h>
#include <ArduinoJson.h>

//Conexion wifi
String ssid = "CRed";
String password = "a2b4c6d8";

String server = "192.168.43.130"; //IP del server
String uri = "/invernadero/Lectorjson.php"; //uri de php que responde a request de arduino

String datos = "";
//constantes 
#define pinValv 3
#define pinExtA 4
#define pinExtB 5
#define pinComp 6

#define tiempoEspera 30000
#define tiempoNeb 10000
#define tiempoExt 10000

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//variables globales
int counter;
float humMinima;
float tempMaxima;
float temperatura[4];
float humedad[4];
//arreglos para status de actuadores y tiempo
int extractores[4];
int nebulizadores[4];
time_t tiempos[4];

void setup() {
  Serial.begin(115200);// comunicacion arduino-pc para debug
  Serial1.begin(115200); /*comunicacion TTL con  ESP8266
                          Serial1  (RX=19, TX=18)*/
                          
  pinMode(pinExtA, OUTPUT);
  pinMode(pinExtB, OUTPUT);
  pinMode(pinValv, OUTPUT);
  pinMode(pinComp, OUTPUT);

  int counter = 0;
  reestablecerArreglos();

  //TODO: funcion establecerTiempo que recibira los datos del servidor
  setTime(9,58,00,11,4,2017);
  iniciar();
}

void iniciar(){
  delay(1000);
  String connStr = "AT+CWJAP=\""+ssid+"\",\""+password+"\"";//comando AT para conectarse a una red
  Serial1.println(connStr);
  delay(5000);
}

void reestablecerArreglos(){//reestablece el valor de los arreglos que le sirven como memoria al programa
    int i;
    for(i=0; i<4; i++){
        temperatura[i]=0;
        humedad[i]=0;
      }
  }

void loop() {
  
   if(Serial1.available()){
    Serial.write(Serial1.read());
 }
 
 if(Serial.available()){
    Serial1.write(Serial.read());
 }
  readData();
}

void readData(){
    delay(tiempoEspera);
    temperatura[counter]=dht.readTemperature(); //lee temperatura en grados Celcius
    humedad[counter]=dht.readHumidity();
    counter++;
    if(temperatura[counter-1]>tempMaxima || humedad[counter-1]<humMinima){//si alguna de las variables esta fuera de rango
        encenderActuadores();
      }
      
    if(counter < 4){
        readData();
      }else{
        if(counter == 4){//cuando se haya terminado una secuencia de cuatro ciclos de medicion y activacion de actuadores
            datos = "temp1="+ temperatura[0] +"&hume1="+ humedad[0] +"&extrac1="+ extractores[0] +"&nebu1="+ nebulizadores[0] +"&tiemp1="+ tiempos[0] +"&temp2="+ temperatura[1] +"&hume2="+ humedad[1] +"&extrac2="+ extractores[1] +"&nebu2="+ nebulizadores[1] +"&tiemp2="+ tiempos[1]+"&temp3="+ temperatura[2] +"&hume3="+ humedad[2] +"&extrac3="+ extractores[2] +"&nebu3="+ nebulizadores[2] +"&tiemp3="+ tiempos[2]+"&temp4="+ temperatura[3] +"&hume4="+ humedad[3] +"&extrac4="+ extractores[3] +"&nebu4="+ nebulizadores[3] +"&tiemp4="+ tiempos[3];
            sendData();
          }
        }
  }

void encenderActuadores(){//funcion que activa los actuadores si es necesario
    if(controlarTemperatura()){//cuando es necesario controlar la temperatura, se encienden los nebulizadores y luego los extractores
        //guardando datos para los arregos de status
        nebulizadores[counter-1]=1;
        extractores[counter-1]=1;
        tiempos[counter-1]=now();
        //secuencia de activacion de actuadores
        digitalWrite(pinComp, HIGH);
        digitalWrite(pinValv, HIGH);
        delay(tiempoNeb);
        digitalWrite(pinValv, LOW);
        digitalWrite(pinComp, LOW);
        digitalWrite(pinExtA, HIGH);
        digitalWrite(pinExtB, HIGH);
        delay(tiempoExt);
        digitalWrite(pinExtA, LOW);
        digitalWrite(pinExtB, LOW);
        readData();
      }else if(controlarHumedad()){
        //guardando datos para los arregos de status
        nebulizadores[counter-1]=1;
        extractores[counter-1]=0;
        tiempos[counter-1]=now();
        //secuencia de activacion de actuadores
        digitalWrite(pinComp, HIGH);
        digitalWrite(pinValv, HIGH);
        delay(tiempoNeb);
        digitalWrite(pinValv, LOW);
        digitalWrite(pinComp, LOW);
        readData();
        }else{
            //guardando datos para los arregos de status
            nebulizadores[counter-1]=0;
            extractores[counter-1]=0;
            tiempos[counter-1]=now();
            readData();
          }
  }

int controlarTemperatura(){/*la variable de returno se inicializa con 1/true, si en el reccorrido del arreglo hay un valor que sea 0 o este en el rango
                            permitido, el valor cambia a 0 y este se devuelve*/
    int activar = 1;
    int i;
    for(i=0; i<4; i++){
        Serial.println((String)i +": Temperatura: "+(String)temperatura[i]+" Humedad: "+(String)humedad[i]+" Minuto: "+ (String)(minute(tiempos[i])));
        if(temperatura[i]>=tempMaxima || temperatura[i] == 0){
            activar = 0;
          }
      }
    return activar;
  }

 int controlarHumedad(){/*la variable de returno se inicializa con 1/true, si en el reccorrido del arreglo hay un valor que sea 0 o este en el rango
                            permitido, el valor cambia a 0 y este se devuelve*/
    int activar = 1;
    int i;
    for(i=0; i<4; i++){
        if(humedad[i]<humMinima || humedad[i] == 0){
            activar = 0;
          }
      }
    return activar;
  }

void sendData(){
    uri = "/invernadero/Lectorjson.php?"+datos;

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
                        humMinima = root["HumedadMiin"];
                        tempMaxima = root["TemperaturaMax"];
                        Serial.println("Humedad Minima: "+ (String)humMinima + " Temperatura Maxima: "+(String)tempMaxima);
                    }
                
        
             //cerrar conexion
             Serial1.println("AT+CIPCLOSE");//cierra la conexion TCP o UDP
            }
 }
  
