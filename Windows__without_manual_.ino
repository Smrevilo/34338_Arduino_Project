//WiFi Setup
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
const char* ssid = "1234567";
const char* password = "heythere00";
WiFiClient client;

//ThingSpeak Setup
#include <ThingSpeak.h>
unsigned long myChannelNumber = 1629214;
const char* myReadAPIKey = "5T4HBV8UA320KO1O";
int statuscode = 0;

//Motor setup
#include <Servo.h>
Servo servo;

int stat;

//Functions
void openw (){
  servo.write(90);
}

void closew (){
  servo.write(0);
}

void windowsts(){
  long humid = ThingSpeak.readLongField(myChannelNumber, 1, myReadAPIKey);
  long temp = ThingSpeak.readLongField(myChannelNumber, 2, myReadAPIKey);
  long index = ThingSpeak.readLongField(myChannelNumber, 3, myReadAPIKey);
  long ir = ThingSpeak.readLongField(myChannelNumber, 4, myReadAPIKey);
  statuscode = ThingSpeak.getLastReadStatus();

  if (statuscode == 200){
    Serial.print("Humidity: ");
    Serial.println(humid);
    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.print("Heat index: ");
    Serial.println(index);
    Serial.print("CO2 level: ");
    Serial.println(ir);

    if (humid > 60||temp > 25 || index > 125|| ir > 955){
      openw();
      stat = 90;
    } else {
      closew();
      stat = 0;
    }
  } else{
    Serial.println("Wifi not connected");
    closew();
  }
  
}

void setupwifi(){
  WiFi.begin(ssid,password);
  ThingSpeak.begin(client); 
}

void setupmotor(){
  servo.attach(5);//pin D1
  servo.write(0); //start closed 
  stat = 0;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setupwifi();
  setupmotor();

}

void loop() {
  windowsts();
  


}
