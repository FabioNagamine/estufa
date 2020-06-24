
/*
  24/06/2020
  Author: Fabio
  Platforms: ESP32
  Language: C++
  File: Estufa.ino
  ------------------------------------------------------------------------
  Description: 
  Control a greenhouse and send information to a dashboard 

*/
#include <Wire.h>
//#include <ACROBOTIC_SSD1306.h>
#include <Esp32WifiManager.h>
#include <HTTPClient.h>
#include "DHTesp.h"
#define TIME_TO_SLEEP 2
#define uS_TO_S_FACTOR 60000000
//Create a wifi manager
WifiManager manager;

DHTesp dht;

const int umidadeSolopin = 34;
int umidadeSolo;
int ligaSensor=32;
int bomba=25;
int dhtPin=33;
bool irriga=0;
int luzPin=35;
int luminosidade;
bool nivel=0;


void setup()
{
  //manager.erase();// This will erase the stored passwords
  manager.setupScan();
  Wire.begin(5,4);	
  //oled.init();                      // Initialze SSD1306 OLED display
  //oled.clearDisplay();
  pinMode(ligaSensor, OUTPUT);
  pinMode(bomba, OUTPUT);
  pinMode(nivel, INPUT);
  dht.setup(dhtPin, DHTesp::DHT11);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop()
{  
  delay(dht.getMinimumSamplingPeriod());
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  
  digitalWrite(ligaSensor, HIGH); 
  umidadeSolo= map(analogRead(umidadeSolopin),0,4095,100,0);
  luminosidade=map(analogRead(luzPin),0,4095,0,100);
  Serial.print("luz=");
  Serial.println(luminosidade);
  if (umidadeSolo<40){
    irriga=1;
    digitalWrite(bomba, HIGH);    
  }else{
    irriga=0;
    digitalWrite(bomba, LOW);
  }
  
  while(manager.getState() != Connected){
    manager.loop();
    Serial.println("Conectando");
    oled.clearDisplay();
    oled.setTextXY(1,0); 
    oled.putString("Conectando!");
    delay(2000);
  }

  delay(1000);
  digitalWrite(ligaSensor, LOW);
/*
  oled.clearDisplay();
  oled.setTextXY(1,0); 
  oled.putString("Temperatura");
  oled.setTextXY(2,2);              // Set cursor position, start of line 2
  oled.putFloat(temperature);
  oled.setTextXY(3,0);              // Set cursor position, start of line 1
  oled.putString("Umidade do SOLO");
  oled.setTextXY(4,2);              // Set cursor position, start of line 2
  oled.putNumber(umidadeSolo);
  oled.setTextXY(4,8);              // Set cursor position, start of line 1
  oled.putString("%");
  oled.setTextXY(5,0); 
  oled.putString("Umidade do AR");
  oled.setTextXY(6,2);              // Set cursor position, start of line 2
  oled.putFloat(humidity);
  oled.setTextXY(6,8);              // Set cursor position, start of line 1
  oled.putString("%");
  delay(1000);
  */
  
  // read the serial port for new passwords and maintain connections
  if (manager.getState() == Connected) {// use the Wifi Stack now connected   
    HTTPClient http;
    String payload = "{\"temperatura\":";      // Inicia uma String associando ao endereço
    payload += temperature;                          // Atribui o valor de leitura de cont a String
    payload += ",\"umidade\":";
    payload += umidadeSolo;
    payload += ",\"umidadeAr\":";
    payload += humidity;
    payload += ",\"bomba\":";
    payload += irriga;  
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
  
  
 if(irriga==0){
  esp_deep_sleep_start();
 }
  
}
