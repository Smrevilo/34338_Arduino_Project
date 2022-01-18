/* Main file for 'ESP_display' board. Board reads data from Thingspeak, and displays it on a LCD display.
 * It read the input form a bottun via an interrupt function and changes what values the LCD displays.
 * File contains follwing functions:
 * void setup();         -> Setup for serial com, wifi, thingspeak, interrupt, and LCD
 * void main();          -> main loop
 * void update_LCD       -> Updates LCD
 * void read_data();     -> Reads data from thingspeak
 * void ISR();           -> Interrupt Service routine. Updates LCD state variable. 
 * Created by Malthe Thingvad
 */
// WiFi 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
const char* ssid = "Baghus";
const char* password = "Odinsvej-3";
WiFiClient client;

// ThingSpeak
#include <ThingSpeak.h>
unsigned long myChannelNumber = 1629214;
const char* myReadAPIKey = "5T4HBV8UA320KO1O";
int statuscode = 0;

// LCD
#include <LiquidCrystal.h>
const int RS = D2, EN = D3, d4 = D5, d5 = D6, d6 = D7, d7 = D8;   
LiquidCrystal lcd(RS, EN, d4, d5, d6, d7);

// Button
const int interruptPin = 2;  // Interrupt Pin D5  
int state_LCD = 0;

// Data
long temp = 0;
long indx = 0;
long hum = 0;
long CO2 = 0;
String LCD_text;

// Declare functions
void update_LCD();  
ICACHE_RAM_ATTR void ISR(); 
void read_data();

void setup() {
  lcd.begin(16, 2);           // Setup LCD
  Serial.begin(115200);       // Serial com
  attachInterrupt(digitalPinToInterrupt(interruptPin), ISR, FALLING); // Attach interrupt
  WiFi.begin(ssid,password);  // Wifi setup
  ThingSpeak.begin(client);   // Thingspeak setup
}

void loop() {
  read_data();    // Get data from thinkspeak
  update_LCD();   // Update LCD
  delay(500);     // Update interval
}

void read_data(){
  // Read Thingspeak
  hum = ThingSpeak.readLongField(myChannelNumber, 1, myReadAPIKey);
  temp = ThingSpeak.readLongField(myChannelNumber, 2, myReadAPIKey);
  indx = ThingSpeak.readLongField(myChannelNumber, 3, myReadAPIKey);
  CO2 = ThingSpeak.readLongField(myChannelNumber, 4, myReadAPIKey);
  statuscode = ThingSpeak.getLastReadStatus(); // Check data is read correct
  if (statuscode == 200){             
      Serial.println("Reading data");
      Serial.println(hum);
      Serial.println(temp);
      Serial.println(indx);
      Serial.println(CO2);
  }
  else{
      Serial.println("Wifi not connected");
  }
  }

void update_LCD(){
  // Choose and print data to LCD
  switch (state_LCD) {
  case 0:
    LCD_text  =  String(temp) + " " + (char)223 + "C       ";
    lcd.setCursor(0,0);
    lcd.print("Temperature:    ");
    lcd.setCursor(0,1);
    lcd.print(LCD_text);
    break;
  case 1: 
    LCD_text  = String(indx) + " " + (char)223 + "C       ";
    lcd.setCursor(0,0);
    lcd.print("Index:       ");
    lcd.setCursor(0,1);
    lcd.print(LCD_text);
    break;
  case 2:
    LCD_text  = String(hum) + " %        ";
    lcd.setCursor(0,0);
    lcd.print("Humidity level: ");
    lcd.setCursor(0,1);
    lcd.print(LCD_text);
    break;
  case 3:
    LCD_text  = String(CO2) + " ppm      ";
    lcd.setCursor(0,0);
    lcd.print("CO2 level:      ");
    lcd.setCursor(0,1);
    lcd.print(LCD_text);
    break; 
  } 
  return; 
}

ICACHE_RAM_ATTR void ISR(){
    // Update LCD state variable
    if(state_LCD == 3){
    state_LCD = 0;
    }
    else{
    state_LCD++;
    }
}
