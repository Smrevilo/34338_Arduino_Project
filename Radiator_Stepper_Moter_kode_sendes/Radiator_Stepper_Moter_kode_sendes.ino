/*Stepper Motor used to adjusting the temperature.
   by Christian Fogh Sørensen. Study nr. S204193

   this motor will adjust the tempereture acording to a Ideal temperature, the user sets.
   it will read this value from Thingspeak
   it will read the current temperature of the room from Thingspeak too.
   it will then compare the 2 temperatures, to decide if the temperature should be turned up or down or neither.
   it has a Manual Mode which can be used to overwrite the thermostat.
   it will enter Automatic Mode again, when the user is finished overwriting the thermostat.


*/
//pin to motor digital write
int A = 14;
int B = 12;
int C = 13;
int D = 15;
//static number defining the exact number of steps exactly 1 rund consist of on the spefific motor in use
#define NUMBER_OF_STEPS_PER_REV 512
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>


//PUT IN YOUR Wi-Fi DETAILS HERE//
//JUST FILL IN THE NEXT 2 LINES HERE//
const char* ssid     = "Name"; //The name of your Wi-Fi router here//
const char* password = "Password"; //The password to your Wi-Fi here//
WiFiClient client;
unsigned long channelID = 1629214;
const char* APIKey = "6LEH3V44E0AWFCNK";
const char* myReadAPIKey = "5T4HBV8UA320KO1O";
const char* server = "api.thingspeak.com";

long temp = 0;
float ideal = 33;
int i;
int ManualTemp = 0;
int ManualOn = 0;
int State = 150;

void onestep_UP();
void onestep_DOWN();

void write(int a, int b, int c, int d);

void setup() {
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  Serial.begin(115200);
  Serial.println(F("Thingspeak test!"));
  WiFi.begin(ssid, password);
  ThingSpeak.begin(client);
  client.connect(server, 80);
}




void loop() {
  //start by reading the information needed from Thingspeak.
  //temperature / overwrited state (Manualtemp) / if Manual overwrite should be turned on or not (ManualOn)
  temp = ThingSpeak.readLongField(channelID, 2, myReadAPIKey);
  Serial.print("Temperatur: ");
  Serial.println(temp);
  ManualTemp = ThingSpeak.readLongField(channelID, 7, myReadAPIKey);
  ManualOn = ThingSpeak.readLongField(channelID, 8, myReadAPIKey);
  //information printed on the monitor.
  //the User will not be able to see this, it's only for the technician.
  Serial.println(ManualTemp);
  Serial.println(ManualOn);
  Serial.print(F("State: "));
  Serial.println(State);

  //Auto Mode
  if (ManualOn == 0) {

    /////////////////////////////////////////
    /////////// IF TEMP UNDER IDEAL ////////
    ////////////////////////////////////////
    if (temp < ideal) {
      if (temp + 25 < ideal - 1) {
        if (State < 375) {
          i = 0;
          while (i < 375 - State) {
            onestep_UP();
            i++;
          }
          State = 375;
          delay(1000);
        }
      }

      else if (temp + 20 < ideal - 1) {
        if (State < 350) {
          i = 0;
          while (i < 350 - State) {
            onestep_UP();
            i++;
          }


        }
        else if (State > 350) {
          i = 0;
          while (i < State - 350) {
            onestep_DOWN();
            i++;
          }
        }
        State = 350;
        delay(1000);
      }

      else if (temp + 15 < ideal - 1) {
        if (State < 325) {
          i = 0;
          while (i < 325 - State) {
            onestep_UP();
            i++;
          }
        }
        else if (State > 325) {
          i = 0;
          while (i < State - 325) {
            onestep_DOWN();
            i++;
          }
        }
        State = 325;
        delay(1000);
      }

      else if (temp + 10 < ideal - 1) {
        if (State < 300) {
          i = 0;
          while (i < 300 - State) {
            onestep_UP();
            i++;
          }
        }
        else if (State > 300) {
          i = 0;
          while (i < State - 300) {
            onestep_DOWN();
            i++;
          }
        }
        State = 300;
        delay(1000);
      }

      else if (temp + 5 < ideal - 1) {
        if (State < 275) {
          i = 0;
          while (i < 275) {
            onestep_UP();
            i++;
          }
        }
        else if (State > 275) {
          i = 0;
          while (i < State - 275) {
            onestep_DOWN();
            i++;
          }
        }
        State = 275;
        delay(1000);
      }

      else {
        if (State < 250) {
          i = 0;
          while (i < 5) {
            onestep_UP();
            i++;
          }
        }
        else if (State > 250) {
          i = 0;
          while (i < State - 250) {
            onestep_DOWN();
            i++;
          }
        }
        State = 250;
        delay(1000);

      }
    }



    /////////////////////////////////////////
    /////////// IF TEMP OVER IDEAL //////////
    ////////////////////////////////////////
    else if (temp > ideal + 1) {
      if (temp - 25 > ideal + 1) {
        if (State > 0) {
          i = 0;
          while (i < State) {
            onestep_DOWN();
            i++;
          }
          State = 0;
          delay(1000);
        }
      }

      else if (temp - 20 > ideal + 1) {
        if (State < 25) {
          i = 0;
          while (i < 25 - State) {
            onestep_UP();
            i++;
          }
        }
        else if (State > 25) {
          i = 0;
          while (i < State - 25) {
            onestep_DOWN();
            i++;
          }
        }
        State = 25;
        delay(1000);
      }

      else if (temp - 15 > ideal + 1) {
        if (State < 50) {
          i = 0;
          while (i < 50 - State) {
            onestep_UP();
            i++;
          }
        }
        else if (State > 50) {
          i = 0;
          while (i < State - 50) {
            onestep_DOWN();
            i++;
          }
        }
        State = 50;
        delay(1000);
      }

      else if (temp - 10 > ideal + 1) {
        if (State < 75) {
          i = 0;
          while (i < 75 - State) {
            onestep_UP();
            i++;
          }
        }
        else if (State > 75) {
          i = 0;
          while (i < State - 75) {
            onestep_DOWN();
            i++;
          }
        }
        State = 75;
        delay(1000);
      }

      else if (temp - 5 > ideal + 1) {
        if (State < 100) {
          i = 0;
          while (i < 100 - State) {
            onestep_UP();
            i++;
          }
        }
        else if (State > 100) {
          i = 0;
          while (i < State - 100) {
            onestep_DOWN();
            i++;
          }
        }
        State = 100;
        delay(1000);
      }

      else {
        if (State < 125) {
          i = 0;
          while (i < 125 - State) {
            onestep_UP();
            i++;
          }
        }
        else if (State > 125) {
          i = 0;
          while (i < State - 125) {
            onestep_DOWN();
            i++;
          }
        }
        State = 125;
        delay(1000);
      }
    }



    //IF TEMP AROUND IDEAL (+/- 1 DEGREE CELSIUS)//
    else {
      if (State > 225) {
        while (i < State - 225) {
          onestep_DOWN();
          i++;
        }
      }
      else if (State < 225) {
        while (i < 225 - State) {
          onestep_UP();
          i++;
        }
      }
    }
    delay(1000);
  }



  
  //Manual Mode
  if (ManualOn != 0) {
    ManualTemp = ThingSpeak.readLongField(channelID, 7, myReadAPIKey);
    //State = ManualTemp * 25;
    Serial.print(F("State: "));
    Serial.println(State);


    if (ManualTemp == 0) {
      if (State > 0) {
        i = 0;
        while (i < State) {
          onestep_DOWN();
          i++;
        }
        State = 0;
      }
    }
    if (ManualTemp == 1) {
      if (State > 75) {
        i = 0;
        while (i < State - 75) {
          onestep_DOWN();
          i++;
        }
        if (State < 75) {
          while (i < 75 - State) {
            onestep_UP();
            i++;
          }
        }
        State = 75;
      }
    }
    if (ManualTemp == 2) {
      if (State > 150) {
        i = 0;
        while (i < State - 150) {
          onestep_DOWN();
          i++;
        }
      }
      if (State < 150) {
        while (i < 150 - State) {
          onestep_UP();
          i++;
        }
      }
      State = 150;
    }
    if (ManualTemp == 3) {
      i = 0;
      if (State > 225) {
        while (i < State - 225) {
          onestep_DOWN();
          i++;
        }
      }
      if (State < 225) {
        while (i < 225 - State) {
          onestep_UP();
          i++;
        }
      }
      State = 225;
    }
    if (ManualTemp == 4) {
      i = 0;
      if (State > 300) {
        while (i < State - 300) {
          onestep_DOWN();
          i++;
        }
      }
      if (State < 300) {
        while (i < 300 - State) {
          onestep_UP();
          i++;
        }
      }
      State = 300;
    }
    if (ManualTemp == 5) {
      if (State < 375) {
        i = 0;
        while (i < 375 - State) {
          onestep_UP();
          i++;
        }
        State = 375;
      }
    }
    delay (1000);
  }
}





//Motor setup
void write(int a, int b, int c, int d) {
  digitalWrite(A, a);
  digitalWrite(B, b);
  digitalWrite(C, c);
  digitalWrite(D, d);
}




//1 step on the motor. Counter Clockwise.
void onestep_DOWN() {
  write(1, 0, 0, 0);
  delay(5);
  write(1, 1, 0, 0);
  delay(5);
  write(0, 1, 0, 0);
  delay(5);
  write(0, 1, 1, 0);
  delay(5);
  write(0, 0, 1, 0);
  delay(5);
  write(0, 0, 1, 1);
  delay(5);
  write(0, 0, 0, 1);
  delay(5);
  write(1, 0, 0, 1);
  delay(5);
}
//1 step on the motor. Clockwise.
void onestep_UP() {
  write(1, 0, 0, 1);
  delay(5);
  write(0, 0, 0, 1);
  delay(5);
  write(0, 0, 1, 1);
  delay(5);
  write(0, 0, 1, 0);
  delay(5);
  write(0, 1, 1, 0);
  delay(5);
  write(0, 1, 0, 0);
  delay(5);
  write(1, 1, 0, 0);
  delay(5);
  write(1, 0, 0, 0);
  delay(5);
}
