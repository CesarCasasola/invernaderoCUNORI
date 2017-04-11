#include <DHT.h>
#include <DHT_U.h>
#include <Time.h>


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

  pinMode(pinExtA, OUTPUT);
  pinMode(pinExtB, OUTPUT);
  pinMode(pinValv, OUTPUT);
  pinMode(pinComp, OUTPUT);

  int counter = 0;
  reestablecerArreglos();

  //TODO: funcion establecerTiempo que recibira los datos del servidor
  setTime(9,58,00,11,4,2017);
}


void reestablecerArreglos(){//reestablece el valor de los arreglos que le sirven como memoria al programa
    int i;
    for(i=0; i<4; i++){
        temperatura[i]=0;
        humedad[i]=0;
      }
  }

void loop() {
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
  
  }
