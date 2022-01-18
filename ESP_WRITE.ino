/* Main file for 'ESP_WRITE' board. Board mesures data from sensors and uploads it to Thingspeak.
 * It measures CO2, hummidity and temperature.
 * File contains follwing functions:
 * void setup();         -> Setup for serial com, wifi, thingspeak, and sensors
 * void main();          -> main loop
 * void print_moniter(); -> Prints data to moniter
 * void send_data();     -> Sends data to thingspeak
 * void measure_data();  -> Reads data from sensor
 * Created by Malthe Thingvad
 */
// WIFI
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
const char* ssid     = "Baghus";
const char* password = "Odinsvej-3";
WiFiClient client;
unsigned long channelID = 1629214;
const char* APIKey = "6LEH3V44E0AWFCNK";
const char* server = "api.thingspeak.com";

// Sensor
#include "DHT.h" 
#define DHTPIN 14         // D5 Possible pins: 3, 4, 5, 12, 13 or 14
#define DHTTYPE DHT11     // Sensor type
DHT dht(DHTPIN, DHTTYPE); // Initialize sensor

// Sensor data
long h;
long t;
long hic;
long CO2;

// Declare functions
void print_moniter();
void send_data();
void measure_data();


void setup() {
  Serial.begin(115200);       // Serial com
  dht.begin();                // Temp+hum sensor setup
  WiFi.begin(ssid,password);  // Wifi setup
  ThingSpeak.begin(client);   // Thinkspeak setup
  client.connect(server, 80);
  return;
}

void loop() {
  measure_data();   // Get data from sensor
  send_data();      // Send data
  print_moniter();  // Print data to moniter
  delay(60*1000);   // Send interval
}



void send_data(){
  // Send data to Thinspeak
  ThingSpeak.setField(1, h);
  ThingSpeak.setField(2, t); 
  ThingSpeak.setField(3, hic);
  ThingSpeak.setField(4, CO2); 
  ThingSpeak.writeFields(channelID, APIKey);
  return;
  }

void measure_data(){
  // Get data som sensors
  // Read DHT11
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)){ // Check if reading is completed
    Serial.println(F("Failed to read DHT sensor"));
    return;
  }
  hic = dht.computeHeatIndex(t, h, false); // Compute heat index
  
  // Read C02
  CO2 = analogRead(A0)/2;   // Divided by 2 to get realistic CO2 measurement
  return;
  }


void print_moniter(){
  // Print data to moniter
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
