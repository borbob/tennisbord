#include <TheThingsNetwork.h>

// Set your DevAddr, NwkSKey, AppSKey and the frequency plan
const char *devAddr = "{insert device address}";
const char *nwkSKey = "{insert your network session key here}";
const char *appSKey = "{insert your app session key here";

const int tiltSensors = 6; // number of tilt sensors connected 
bool states[tiltSensors]; //store board state in array. E.g. 01010. First 7 sensors for the left side of the board. Second 7 sensors for the right side.
//

#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  debugSerial.println("-- PERSONALIZE");
  ttn.personalize(devAddr, nwkSKey, appSKey);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  //initial digital input 0
  for(int i = 0; i<tiltSensors; i++) {
    pinMode(i, INPUT);
    states[i] = 0;
  }
}


int state = 0;

void loop()
{
  debugSerial.println("-- LOOP");
  
  // Prepare payload of 1 byte to indicate LED status
  byte payload[1];
  payload[0] = B0;
  payload[1] = B1;
  bool changed = false; //boolean indicating wether a flap value changes on the board
  for(int i =0; i<tiltSensors; i++) {
    debugSerial.print("reading sensor: ");
    debugSerial.println(i);
    int currentValue = digitalRead(i);
    if(currentValue) {
      payload[0] = payload[0] | ( 1 << i);
    }
    if(currentValue == states[i]) {
      debugSerial.print(" sensor value is: ");
      debugSerial.println(digitalRead(i));
    }
    else {
      debugSerial.print(" sensor value changed to: ");
      debugSerial.println(digitalRead(i));
      states[i] = currentValue;
      changed = true;
    }
  }
  if(changed) {
    //send new value to backend over LoRa
    ttn.sendBytes(payload, sizeof(payload));
    delay(8000);
  }
  delay(1000);
}
