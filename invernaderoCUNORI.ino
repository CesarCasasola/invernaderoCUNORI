#include <DHT.h>
#include <DHT_U.h>
#include <Time.h>
#include <ArduinoJson.h>

//Conexion wifi
String ssid = "CRed";
String password = "a2b4c6d8";

String server = "192.168.43.240"; //IP del server
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
float humMinima = 50;
float tempMaxima = 20;
float temperatura[4];
float humedad[4];
//arreglos para status de actuadores y tiempo
int extractores[4];
int nebulizadores[4];
time_t tiempos[4];

//Variables de inicio fecha
int anio;
int mes;
int dia;
int hora;
int minu;
int seg;


void datauri();
void sendData();
String decodeJson(String Json, String key);


void setup() {
  Serial.begin(115200);// comunicacion arduino-pc para debug
  Serial1.begin(115200); /*comunicacion TTL con  ESP8266
                          Serial1*/
                          
  pinMode(pinExtA, OUTPUT);
  pinMode(pinExtB, OUTPUT);
  pinMode(pinValv, OUTPUT);
  pinMode(pinComp, OUTPUT);

  int counter = 0;
  reestablecerArreglos();

  iniciar();
  datenow();
  setTime(hora,minu,seg,dia,mes,anio);
  
  
}

void iniciar(){
  delay(1000);
  String connStr = "AT+CWJAP=\""+ssid+"\",\""+password+"\"";//comando AT para conectarse a una red
  Serial1.println(connStr);
  Serial.println(connStr);
  delay(5000);
  Serial.println(Serial1.read());
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
            datauri();
            sendData();
            counter = 0;
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
       }else{
            //guardando datos para los arregos de status
            nebulizadores[counter-1]=0;
            extractores[counter-1]=0;
            tiempos[counter-1]=now();
          }       
  }

int controlarTemperatura(){/*la variable de returno se inicializa con 1/true, si en el reccorrido del arreglo hay un valor que sea 0 o este en el rango
                            permitido, el valor cambia a 0 y este se devuelve*/
    int activar = 1;
    int i;
    for(i=0; i<4; i++){
        Serial.println((String)i +": Temperatura: "+(String)temperatura[i]+" Humedad: "+(String)humedad[i]+" Tiempo: "+(String)minute(tiempos[i]));
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

void datenow(){
   uri = "/invernadero/datetime.php?codigo=1";
   Serial1.println("AT+CIPSTART=\"TCP\",\""+ server +"\",80");
   Serial.println("AT+CIPSTART=\"TCP\",\""+ server +"\",80");//echo del comando anterior

   if(Serial1.find("OK")){//respuesta en caso de establecer conexion TCP exitosa
      Serial.println("Conexion TCP lista.");

      String getRequest =
  
       "GET " + uri + " HTTP/1.0\r\n" +
      
       "Host: " + server + "\r\n\r\n";
      
        String sendCmd = "AT+CIPSEND=";//comando para enviar datos, seguido se debe escribir la longitud de datos a enviarse, maximo 2048
    
       Serial1.print(sendCmd);
       Serial.print(sendCmd);//echo
       Serial.print(getRequest);
    
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
                    Serial.println(result);
                    
                            anio = decodeJson(result, "Anio").toInt();
                            mes = decodeJson(result, "Mes").toInt();
                            dia = decodeJson(result, "Dia").toInt();
                            hora = decodeJson(result, "Hora").toInt();
                            minu = decodeJson(result, "Minuto").toInt();
                            seg = decodeJson(result, "Segundo").toInt();
                            Serial.println("Anio: "+ (String)anio + 
                                          " Mes: "+(String)mes +
                                          "Dia: "+(String)dia +
                                          " -- Hora: "+(String)hora +
                                          "Minuto: "+(String)minu +
                                          "Segundo: "+(String)seg);
                        }
                    
            
                 //cerrar conexion
                 Serial1.println("AT+CIPCLOSE");//cierra la conexion TCP o UDP
                }           
            }
   
 }
  
void sendData(){
    uri = "/invernadero/Lectorjson.php?"+datos;

    //AT command para iniciar conexion como cliente tipo TCP por el puerto 80
    Serial1.println("AT+CIPSTART=\"TCP\",\""+ server +"\",80");
    Serial.println("AT+CIPSTART=\"TCP\",\""+ server +"\",80");//echo del comando anterior

     if(Serial1.find("OK")){//respuesta en caso de establecer conexion TCP exitosa
      Serial.println("Conexion TCP lista.");
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
                      Serial.println(result);
                      humMinima = decodeJson(result, "HumedadMiin").toFloat();
                      tempMaxima = decodeJson(result, "TemperaturaMax").toFloat();
                      Serial.println("Humedad Minima: "+ (String)humMinima + " Temperatura Maxima: "+(String)tempMaxima);
                          }
                      
              
                   //cerrar conexion
                   Serial1.println("AT+CIPCLOSE");//cierra la conexion TCP o UDP
                  }
       }else{//la conexion no se establecio, por lo que se intenta nuevamente llamando de forma recursiva al metodo.
           iniciar();
           sendData();
      }
}
     


void datauri(){
  int i;
  for(i=1; i<5; i++){
    if(i==1){
      datos = datos + "temp"+ (String)i +"="+ (String)temperatura[i-1] +"&hume"+ (String)i +"="+ (String)humedad[i-1] +"&extrac"+ (String)i +"="+ (String)extractores[i-1] +"&nebu"+ (String)i +"="+ (String)nebulizadores[i-1]+"&tiemp"+ (String)i +"="+ (String)(year(tiempos[i-1]))+"-"+(String)(month(tiempos[i-1]))+"-"+(String)(day(tiempos[i-1])) +"-"+(String)(hour(tiempos[i-1]))+":"+(String)(minute(tiempos[i-1]))+":"+(String)(second(tiempos[i-1]));    }
    if(i>1){
      datos = datos + "&temp"+ (String)i +"="+ (String)temperatura[i-1] +"&hume"+ (String)i +"="+ (String)humedad[i-1] +"&extrac"+ (String)i +"="+ (String)extractores[i-1] +"&nebu"+ (String)i +"="+ (String)nebulizadores[i-1]+"&tiemp"+ (String)i +"="+ (String)(year(tiempos[i-1]))+"-"+(String)(month(tiempos[i-1]))+"-"+(String)(day(tiempos[i-1])) +"-"+(String)(hour(tiempos[i-1]))+":"+(String)(minute(tiempos[i-1]))+":"+(String)(second(tiempos[i-1]));      }
    }
  }
  

String decodeJson(String Json, String key){
    key = "\""+key+"\"";
    int posKeyEnd = Json.indexOf(key)+key.length();
    //Serial.println(posKeyEnd);
    
    String valueApr = Json.substring( posKeyEnd, Json.indexOf(',', posKeyEnd+1));
    //Serial.println(valueApr);
    int posQuo1 = valueApr.indexOf('"');
    String value = valueApr.substring(posQuo1+1, valueApr.indexOf('"', posQuo1+1));
    //Serial.println(value);
    return value;    
  } 

