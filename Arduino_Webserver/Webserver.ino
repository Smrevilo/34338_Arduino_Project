#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <ThingSpeak.h>
#include <millisDelay.h>

#define channelID 1629214
#define windowFieldValue 5
#define windowFieldTime 6
#define tempFieldValue 7
#define tempFieldTime 8
#define readKey "5T4HBV8UA320KO1O"
#define writeKey "6LEH3V44E0AWFCNK"

// #define DEBUG

/*
 * Main file for 'ESP_Webserver' board.
 * 
 * This board hold the webserver and controls the
 * commands comming from the website to ThingSpeak
 * this code also displays the graphs from 
 * ThingSpeak on the website.
 * 
 * Created by Oliver M. Stege
 */



  // create timers for the window/radiator to be set manually
millisDelay openWindowTimer;
millisDelay temperatureTimer;

  // Create an instance of the server, WiFIclient and multiWifi
WiFiClient client;
ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);


/*
 * This function sets up the Serial object, 
 * the wifi connection to the AP 
 * both for the connection with ThingSpeak
 * and the connection with the website
 */
void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    delay(10);
    Serial.println();
  #endif
  
  // Connect to WiFi network
  WiFi.begin("Baghus", "Odinsvej-3");
  wifiMulti.addAP("IoTFotonik", "Cyberteknologi");  
  wifiMulti.addAP("Baghus", "Odinsvej-3");

  #ifdef DEBUG
    Serial.println();
    Serial.print("Connecting ...");
  #endif
  
  //WiFi.begin(ssid, password);
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    #ifdef DEBUG
      Serial.print(".");
    #endif
  }

  #ifdef DEBUG
    Serial.println("");
    Serial.println("WiFi connected to ");
    Serial.println(WiFi.SSID());
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  #endif

  // Start the mDNS responder for esp8266.local
  if (MDNS.begin("iot")) { 
    #ifdef DEBUG          
      Serial.println("mDNS responder started");
    #endif
  } else {
    #ifdef DEBUG
      Serial.println("Error setting up MDNS responder!");
    #endif
  }


  // Sets up the function calls that should be called for different http requests
  server.on("/", HTTP_GET, handleRoot);
  server.on("/Window", HTTP_POST, handleWindow);
  server.on("/Temperature", HTTP_POST, handleTemperature);
  server.onNotFound(handleNotFound);

  // Start the server
  server.begin();
  #ifdef DEBUG
    Serial.println("Server started");
  #endif
}

/*
 * Handles the http requests and checks if 
 * the timers for either the radiator or 
 * the window has times out.
 * 
 */
void loop() {
  // Check if a client has connected
  server.handleClient();
  checkWindowTimer();
  checktemperatureTimer();
}

/*
 * Checks the temperature timer and resets ThingSpeak
 * graph if its timed out
 */
void checktemperatureTimer() { // the led task
  // check if delay has timed out
  if (temperatureTimer.justFinished()) { // only returns TRUE once.
    ThingSpeak.begin(client);
    client.connect("https://api.thingspeak.com/update?api_key=6LEH3V44E0AWFCNK&field1=0", 80);
    do {
      ThingSpeak.setField(tempFieldTime, 0);
      ThingSpeak.writeFields(channelID, writeKey);
      #ifdef DEBUG
        Serial.println("Trying to reset manual override...");
      #endif
    } while (!checkField(0, tempFieldTime));
    client.stop();
  }
}

/*
 * Checks the Window timer and resets ThingSpeak
 * graph if its timed out
 */
void checkWindowTimer() { // the led task
  // check if delay has timed out
  if (openWindowTimer.justFinished()) { // only returns TRUE once.
    ThingSpeak.begin(client);
    client.connect("https://api.thingspeak.com/update?api_key=6LEH3V44E0AWFCNK&field1=0", 80);
    do {
      ThingSpeak.setField(windowFieldTime, 0);
      ThingSpeak.writeFields(channelID, writeKey);
      #ifdef DEBUG
        Serial.println("Trying to reset manual override...");
      #endif
    } while (!checkField(0, windowFieldTime));
    client.stop();
  }
}

/*
 * Handles the initial http GET request for
 * the html page.
 */
void handleRoot() {                         
  server.send(200, "text/html", "<html><title>Arduino Amigo's</title><meta charset=\"utf-8\" \/> \
      </head> \
      <center><body style=\"background-color: #FF69B4;\"> \
      <h1>Arduino Amigo's &reg; home-system control-center</h1></center> \
      <p>Welcome to your own private IoT home environment control-center. From here you will be able to see, manage and control the temperature and venting of your home.</p> \
      <p>The only thing you need to do, for you to manage the environment in your home from ANYWHERE is the following. You need to bring your laptop, your breadboard and your ESP8266 with a micro USB cable to connect it to the laptop. Then you need turn on your laptop to power up the MCU. You then have to open the arduino IDE to insert the wifi password and ssid such that the ESP can connect to the nearest WiFi (alternatively wifi sharing from an iphone). At This point anyone on the concerned wifi can connect to the Ip-adress found in the Serial Monitor and manage your home IoT system!!! WOAW</p> \
      <br> \ 
      <br> \     
      <center> \
      <h4>The following is your control panel to control and manage your home!</h4> \
      </center> \
      <center> \
      <style type=\"text/css\"> \
          td {  \
            padding: 0 100px; \
          }  \
      </style>  \
        <table> \
            <tr> \
              <th><h2>Open or close your window</h2></th> \
              <th><h2>Set your radiator setting</h2></th> \
            </tr> \
            <tr> \
              <td> \
                <form action=\"/Window\" method=\"POST\" > \
                    <section> \
                        <label for=\"open window\">How open do you want the window to be?</label> \
                         <br> \
                         <br> \
                         <center> \
                         <span>Closed</span> \
                         <input type=\"range\" name=\"open window\" id=\"open window\" value=\"5\" min=\"0\" max=\"90\"> \
                         <span>Wide open </span> \
                         </center> \
                    </section> \
                    <br> \
                     <section> \
                        <label for=\"time window\">How long do you want the window to be opened/closed? (minutes) </label> \
                         <br> \
                         <input type=\"number\" name=\"time window\" id=\"time window\" pattern=\"\\d+\" min=\"1\" required > \
                      </section> \
                      <br> \
                      <input type=\"submit\" value=\"Open/Close\" style=\"width:500px; height:100px; font-size:24px\">  \
                </form> \
              </td> \
              <td> \
                <form action=\"/Temperature\" method=\"POST\" > \
                    <section> \
                        <label for=\"temp\">Which setting do you want the radiator to be at?</label> \
                         <br> \
                         <br> \
                         <center> \
                         <span>Off</span> \
                         <input type=\"range\" name=\"temp\" id=\"temp\" value=\"3\" min=\"0\" max=\"5\"> \
                         <span>Full power</span> \
                         </center> \
                    </section> \
                    <br> \
                     <section> \
                        <label for=\"time temp\">How long do you want the radiator to be set at this setting? (minutes) </label> \
                         <br> \
                         <input type=\"number\" name=\"time temp\" id=\"time temp\" pattern=\"\\d+\" min=\"1\" required > \
                      </section> \
                      <br> \
                      <input type=\"submit\" value=\"Up/Down\" style=\"width:500px; height:100px; font-size:24px\">  \
                </form> \
              </td> \
            </tr> \
        </table> \
      </center> \
      <center> \
      <h4>From here you can get an overview of the current environment in your home!</h4> \
      </center> \
      <table> \
        <tr> \
            <td> <iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/apps/matlab_visualizations/443150\"></iframe> </td> \
            <td> <iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/1629214/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15\"></iframe> </td> \
        </tr> \
        <tr> \
            <td> <iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/1629214/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15\"></iframe> </td> \
            <td> <iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/1629214/charts/3?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15\"></iframe> </td> \
        </tr> \
      </table> \
      </body></html>");
}

/*
 * Handles the html form for the window
 * and sets the concerned ThingSpeak 
 * Graph appropriatly
 * 
 */
void handleWindow() {
  ThingSpeak.begin(client);
  client.connect("https://api.thingspeak.com/update?api_key=6LEH3V44E0AWFCNK&field1=0", 80);
  String openS = server.arg("open window");
  String timeS = server.arg("time window");
  int openN = openS.toInt();
  int timeN = timeS.toInt();
  
  do {
    ThingSpeak.setField(windowFieldValue, openN);
    ThingSpeak.setField(windowFieldTime, timeN);
    ThingSpeak.writeFields(channelID, writeKey);
    openWindowTimer.start(timeN * 60000);         // Microsecond to minutes conversion
    
    #ifdef DEBUG
      Serial.print("trying to set the ThingSpeak graph for window time to = ");
      Serial.println(timeN);
      Serial.print("and the window value to = ");
      Serial.println(openN);
    #endif
    
    delay(100);
  } while (!checkField(openN, windowFieldValue, timeN, windowFieldTime));

  #ifdef DEBUG
    Serial.println("Succeded in changing the ThingSpeak graph");
  #endif
  client.stop();                                  
  server.sendHeader("Location", "/");             // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                               // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

/*
 * Handles the html form for the radiator
 * and sets the concerned ThingSpeak 
 * Graph appropriatly
 * 
 */
void handleTemperature() {
  ThingSpeak.begin(client);
  client.connect("https://api.thingspeak.com/update?api_key=6LEH3V44E0AWFCNK&field1=0", 80);
  String openS = server.arg("temp");
  String timeS = server.arg("time temp");
  int openN = openS.toInt();
  int timeN = timeS.toInt();
  
  do {
    ThingSpeak.setField(tempFieldValue, openN);
    ThingSpeak.setField(tempFieldTime, timeN);
    ThingSpeak.writeFields(channelID, writeKey);
    temperatureTimer.start(timeN * 60000);        // Microsecond to minutes conversion
    
    #ifdef DEBUG
      Serial.print("trying to set the ThingSpeak graph for radiator time to = ");
      Serial.println(timeN);
      Serial.print("and the radiator value to = ");
      Serial.println(openN);
    #endif
    
    delay(100);
  } while (!checkField(openN, tempFieldValue, timeN, tempFieldTime));

  #ifdef DEBUG
    Serial.println("Succeded in changing the ThingSpeak graph");
  #endif
  client.stop();                                  
  server.sendHeader("Location", "/");             // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                               // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

/*
 * checks if a field has been set
 * on ThingSpeak
 */
bool checkField(int value, int field) {
    long index = ThingSpeak.readLongField(channelID, field, readKey);
    return index == (long)value ? true : false;
}

/*
 * Checks if two fields has been set
 * on ThingSpeak.
 */
bool checkField(int valueOne, int fieldOne, int valueTwo, int fieldTwo) {
    long indexOne = ThingSpeak.readLongField(channelID, fieldOne, readKey);
    long indexTwo = ThingSpeak.readLongField(channelID, fieldTwo, readKey);
    if (indexOne == valueOne && indexTwo == valueTwo) {
      return true;
    }
    return false;
}

/*
 * handles a not know http request
 */
void handleNotFound() {
    server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
