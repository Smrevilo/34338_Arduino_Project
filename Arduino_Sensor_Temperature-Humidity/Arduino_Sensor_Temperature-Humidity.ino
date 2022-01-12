// WIFI
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
const char* ssid     = "LGN";
const char* password = "313061larsnahnsen";
WiFiClient client;
unsigned long channelID = 1629214;
const char* APIKey = "6LEH3V44E0AWFCNK";
const char* server = "api.thingspeak.com";

// SENSOR
#include "DHT.h" 
#define DHTPIN 14         //D5 Possible pins: 3, 4, 5, 12, 13 or 14
#define DHTTYPE DHT11     //Sensor type
DHT dht(DHTPIN, DHTTYPE); //Initialize sensor
long h;
long t;
long hic;
long CO2;

// Declare functions
void print_moniter();
void send_data();
void measure_data();


void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid,password);
  ThingSpeak.begin(client);
  client.connect(server, 80);
  return;
}

void loop() {
  measure_data();
  send_data();
  //print_moniter(); 
  delay(500);
}



void send_data(){
  ThingSpeak.setField(1, h);
  ThingSpeak.setField(2, t); 
  ThingSpeak.setField(3, hic);
  ThingSpeak.setField(4, CO2); 
  ThingSpeak.writeFields(channelID, APIKey);
  }

void measure_data(){
  // Read DHT11
  h = dht.readHumidity();
  t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)){ // Check if reading is completed
    Serial.println(F("Failed to read DHT sensor"));
    return;
  }
  hic = dht.computeHeatIndex(t, h, false); // Compute heat index
  
  // Read C02
  CO2 = analogRead(A0);
  return;
  }


void print_moniter(){
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C  CO2 level: "));
  Serial.print(CO2);
  Serial.println(F(" ppm")); 
  return;
  }
