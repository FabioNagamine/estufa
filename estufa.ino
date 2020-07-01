
/*
  09/03/2020
  Author: Fabio
  Platforms: ESP32
  Language: C++
  File: Estufa.ino
  ------------------------------------------------------------------------
  Description: 
  Control a greenhouse and send information to a dashboard 

*/
#include <Wire.h>
#include <Esp32WifiManager.h>
#include <HTTPClient.h>
#include "DHTesp.h"
#define TIME_TO_SLEEP 10
#define uS_TO_S_FACTOR 60000000
//Create a wifi manager
WifiManager manager;

DHTesp dht;

const int umidadeSolo1pin = 34;
const int umidadeSolo2pin = 36;
int umidadeSolo1;
int umidadeSolo2;
int ligaSensor=32;
int bomba1=25;
int bomba2=26;
int dhtPin=33;
bool irriga1=0;
bool irriga2=0;
int luzPin=35;
int luminosidade;
bool nivel=0;

void setup()
{
  //manager.erase();// This will erase the stored passwords
  manager.setupScan();
  Wire.begin(5,4);	
  pinMode(ligaSensor, OUTPUT);      // pin mode 
  pinMode(bomba1, OUTPUT); 
  pinMode(bomba2, OUTPUT); //
  pinMode(nivel, INPUT);            //
  dht.setup(dhtPin, DHTesp::DHT11); //
  Serial.println("DHT initiated");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);   // enable sleep mode
}

void loop()
{  
  delay(dht.getMinimumSamplingPeriod());
  delay(1000);
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  
  digitalWrite(ligaSensor, LOW); 
  umidadeSolo1=map(analogRead(umidadeSolo1pin),0,4095,100,0);//analogRead(umidadeSolopin);
  umidadeSolo2=map(analogRead(umidadeSolo2pin),0,4095,100,0);//analogRead(umidadeSolopin);
  luminosidade=map(analogRead(luzPin),0,4095,0,100);

  if (umidadeSolo1<45){
    irriga1=1;
    digitalWrite(bomba1, LOW);  
    delay(2000);
    digitalWrite(bomba1, HIGH);  
  }else{
    irriga1=0;
    digitalWrite(bomba1, HIGH);
  }
  
  if (umidadeSolo2<20){
    irriga2=1;
    digitalWrite(bomba2, LOW);    
      delay(2000);
    digitalWrite(bomba2, HIGH);  
  }else{
    irriga2=0;
    digitalWrite(bomba2, HIGH);
  }
  while(manager.getState() != Connected){
    manager.loop();
    Serial.println("Conectando");
    delay(1000);
  }

  delay(1000);
  digitalWrite(ligaSensor, LOW);

 
  // read the serial port for new passwords and maintain connections
 
  if (manager.getState() == Connected) {// use the Wifi Stack now connected
  // oled.clearDisplay();      
    HTTPClient http;
    String payload = "{\"temperatura\":";      // Inicia uma String associando ao endereço
    payload += temperature;                          // Atribui o valor de leitura de cont a String
    payload += ",\"umidade\":";
    payload += umidadeSolo1;
     payload += ",\"umidade2\":";
    payload += umidadeSolo2;
    payload += ",\"umidadeAr\":";
    payload += humidity;
    payload += ",\"bomba\":";
    payload += irriga1;  
    payload += ",\"luminosidade\":";
    payload += luminosidade;  
    payload += ",\"User\":";
    payload += "0"; 
    payload += "}";                          // Finaliza a String
    Serial.println(payload);
    http.begin("http://fabioniot.mybluemix.net/estufa");   
    http.addHeader("Content-Type", "application/json; charset=UTF-8");
    http.POST(payload);  
    http.end();    
    Serial.println(payload);
    }
  
  
 if(irriga1==0){
   // esp_deep_sleep_start();
 }

 
  delay(1000);
}
