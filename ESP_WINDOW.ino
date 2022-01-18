/* Main file for "ESP_WINDOWCONTROL"-board. Board reads data from ThingSpeak and control windows accordingly. 
 *  Read if manual setting is available --> if available, set windows. 
 *  Otherwise uses smart setting windowsts()-->
 *  Read: Humidity, Temperature, Heat Index and CO2 level, and based on given parameters decide how open/closed windows should be.
 *  
 * NOTE: After evaluation all angles for servo motor have been multiplied by two, inorder to reach the desired angle. Example:  90*2 = 90 degrees.
 * 
 * Created by Caroline Petersen
*/

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

//Variables
long wintime;
long winset;

//Functions
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
    Serial.print("Opening: ");

    if (humid > 65||temp > 35 || index > 35|| ir > 2000 ){
      servo.write(90*2);
      Serial.println(90);
    } else {
      if (humid > 60||temp > 30 || index > 30|| ir > 1500 ){
        servo.write(45);
        Serial.println(45*2);
      } else {
        if (humid > 50||temp > 25 || index > 25|| ir > 1000 ){
          servo.write(20);
          Serial.println(20*2);
        } else {
          servo.write(0);
          Serial.println(0);
        }
      }
    }
    
  }else{
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

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setupwifi();
  setupmotor();

}

void loop() {
  
  wintime = ThingSpeak.readLongField(myChannelNumber, 6, myReadAPIKey);
  if (wintime == 0){
    windowsts();} else {
      winset = ThingSpeak.readLongField(myChannelNumber, 5, myReadAPIKey);
      Serial.print("Setting: ");
      Serial.println(winset);
      Serial.print("Time remaining: ");
      Serial.println(wintime);
      
      winset = (int)winset;
      servo.write(winset*2);
    }
}
