#include <Time.h>
#include <DHT.h>
#include <DHT_U.h>

String datos = "";
//constantes 
#define pinValv 3
#define pinExtA 4
#define pinExtB 5
#define pinComp 6

#define tiempoEspera 30000
#define tiempoNeb 15000
#define tiempoExt 15000

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

//Conexion wifi
String ssid = "TURBONETT_0AD7D2";
String password = "767240b666";

String server = "www.cunori.edu.gt";//"192.168.43.240"; //IP del server
String uri = "/invernadero/writejson.php"; //uri de php que responde a request de arduino





//Variables de inicio fecha
int anio;
int mes;
int dia;
int hora;
int minu;
int seg;

void setup() {
  Serial.begin(115200); // comunicacion arduino-pc para debug
  Serial1.begin(115200); /*comunicacion TTL con  ESP8266
                          Serial1*/
  //conectarse al access-point y pedir al server la fecha y hora
  iniciar();
  datenow();
  delay(5000);
  datenow(); 
  delay(6000);//tiempo de retardo para esperar que se ejecute la funcion datenow()*/
}



void loop() {
  Serial.println(String(year(now())));
  while(year(now()) < 2017){
    Serial.println("Corrigiendo fecha....");
    datenow();    
    }
  
  delay(5000);
  readData();
 }

 

void iniciar(){ //función que ejecuta comando AT para conectarse al access-point
  delay(1000);
  String connStr = "AT+CWJAP=\""+ssid+"\",\""+password+"\"";//comando AT para conectarse a una red
  Serial1.println(connStr);
      Serial.println(connStr);
  delay(5000); 
 
}


void datenow(){
   uri = "/invernadero/datetime.php?codigo=1";
   
   Serial1.println("AT+CIPSTART=\"TCP\",\""+ server +"\",80");
      Serial.println("AT+CIPSTART=\"TCP\",\""+ server +"\",80");//echo del comando anterior
   
   if(Serial1.find("OK")){//respuesta en caso de establecer conexion TCP exitosa
      Serial.println("Conexion TCP lista.");
   }

      delay(1000);
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
                           
                   while(Serial1.available()){ //leyendo respuesta
                      Serial.print("RESPONSE: ");
                      String tmp = Serial1.readString();
                      Serial.println(tmp);
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
                                          " Dia: "+(String)dia +
                                          " -- Hora: "+(String)hora +
                                          " Minuto: "+(String)minu +
                                          " Segundo: "+(String)seg+"\n");

                            setTime(hora,minu,seg,dia,mes,anio);                            
                            
                        }
                        
                 //cerrando conexion tcp
                 Serial1.println("AT+CIPCLOSE");
                }           
            
   
 }

 
/*recibe lecturas del sensor dht y luego llama a funciones para activar actuadores o enviar datos al servidor segun sea necesario*/
 void readData(){
    delay(tiempoEspera);
    temperatura[counter]=dht.readTemperature(); //lee temperatura en grados Celcius
    humedad[counter]=dht.readHumidity();
    counter++;
    //si alguna de las variables esta fuera de rango
    encenderActuadores();//evalua si es necesario activar actuadores y guarda el tiempo en el que se ejecuta
     
      
    if(counter < 4){
        readData();
      }else{
        if(counter == 4){//cuando se haya terminado una secuencia de cuatro ciclos de medicion y activacion de actuadores
            datauri();
            int k = 0; //contador para limitar numero de intentos de envio
            while(counter && k<10){
              sendData();
              k++;
            }
            int i = 0;
            for(i = 0; i < 4; i++){
                  nebulizadores[i] = 0;
                  extractores[i] = 0;
              }
            return;
          }
        }
}



//funcion que activa los actuadores si es necesario
void encenderActuadores(){
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




  
/*la variable de returno se inicializa con 1/true, si en el reccorrido del arreglo hay un valor que sea 0 o este en el rango
permitido, el valor cambia a 0 y este se devuelve*/
int controlarTemperatura(){
    int activar = 1;
    int i;
    for(i=0; i<4; i++){
        Serial.println((String)i +": Temperatura: "+(String)temperatura[i]+" Humedad: "+(String)humedad[i]+" Tiempo: "+(String)hour(tiempos[i]));
        if(temperatura[i]<=tempMaxima || temperatura[i] == 0){
            activar = 0;
          }
      }
    Serial.println("\n");
    return activar;
  }



/*la variable de returno se inicializa con 1/true, si en el reccorrido del arreglo hay un valor que sea 0 o este en el rango
permitido, el valor cambia a 0 y este se devuelve*/
 int controlarHumedad(){
    int activar = 1;
    int i;
    for(i=0; i<4; i++){
        if(humedad[i]>humMinima || humedad[i] == 0){
            activar = 0;
          }
      }
    Serial.println("\n");
    return activar;
  }



void sendData(){
    uri = "/invernadero/writejson.php?"+datos;
    
    //AT command para iniciar conexion como cliente tipo TCP por el puerto 80
    Serial1.println("AT+CIPSTART=\"TCP\",\""+ server +"\",80");
      Serial.println("AT+CIPSTART=\"TCP\",\""+ server +"\",80");//echo del comando anterior
    
     if(Serial1.find("OK")){//respuesta en caso de establecer conexion TCP exitosa
      Serial.println("Conexion TCP lista.");
     }

       delay(1000);    
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
                     while(Serial1.available()){ //leyendo respuesta
                        String tmp = Serial1.readString();
                        Serial.println(tmp);
                        result = tmp.substring(tmp.indexOf('{'), tmp.indexOf('}')+1);                    
                                         
                      }
                      Serial.println(result);
                      if(decodeJson(result, "HumedadMin").toFloat()>0){
                          humMinima = decodeJson(result, "HumedadMin").toFloat();
                        }
                      if(decodeJson(result, "TemperaturaMax").toFloat()> 0){
                          tempMaxima = decodeJson(result, "TemperaturaMax").toFloat();
                        }
                      
                      Serial.println("Humedad Minima: "+ (String)humMinima + " Temperatura Maxima: "+(String)tempMaxima);

                      
                      //cerrar conexion
                      Serial1.println("AT+CIPCLOSE");//cierra la conexion TCP o UDP
                      counter=0;
                      Serial.println("Counter = "+ String(counter));
                      return;
                      }     
                   
           }
           
}



/*construye la url con los datos de los arreglos de registros*/
void datauri(){
  int i;
  for(i=1; i<5; i++){
    if(i==1){
      datos = "temp"+ (String)i +"="+ (String)temperatura[i-1] +"&hume"+ (String)i +"="+ (String)humedad[i-1] +"&extrac"+ (String)i +"="+ (String)extractores[i-1] +"&nebu"+ (String)i +"="+ (String)nebulizadores[i-1]+"&tiemp"+ (String)i +"="+ (String)(year(tiempos[i-1]))+"-"+(String)(month(tiempos[i-1]))+"-"+(String)(day(tiempos[i-1])) +"-"+(String)(hour(tiempos[i-1]))+":"+(String)(minute(tiempos[i-1]))+":"+(String)(second(tiempos[i-1])); 
     }if(i>1){
      datos = datos + "&temp"+ (String)i +"="+ (String)temperatura[i-1] +"&hume"+ (String)i +"="+ (String)humedad[i-1] +"&extrac"+ (String)i +"="+ (String)extractores[i-1] +"&nebu"+ (String)i +"="+ (String)nebulizadores[i-1]+"&tiemp"+ (String)i +"="+ (String)(year(tiempos[i-1]))+"-"+(String)(month(tiempos[i-1]))+"-"+(String)(day(tiempos[i-1])) +"-"+(String)(hour(tiempos[i-1]))+":"+(String)(minute(tiempos[i-1]))+":"+(String)(second(tiempos[i-1])); 
      }    
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
