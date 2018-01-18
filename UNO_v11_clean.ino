/*
  This script is compatible with NODE_v10_clean
  This script sends a char via. bluetooth.
  The char change based upon values from a LDR and an ultrasonic distance measurer.
  Hardware:
  -  Arduino UNO
  -  HM-10 BLE (conected to another HM-10 BLE)
  -  LDR
  -  Grove Ultrasonic Ranger
  -  Resistors 2x 2kΩ and 1x 1kΩ
*/


#include <AltSoftSerial.h>
AltSoftSerial BTserial;
//
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
/*
  SerialIn_SerialOut_HM-10_01
  Uses hardware serial to talk to the host computer and AltSoftSerial for communication with the bluetooth module
  Whatever is entered in the serial monitor is sent to the connected device.
  Anything received from the connected device is copied to the serial monitor.
  This is used to hard code both HM-10 modueles.
  Does not send line endings to the HM-10
  Pins
  BT VCC to Arduino 5V out.
  BT GND to GND
  Arduino D8 (SS RX) - BT TX no need voltage divider
  Arduino D9 (SS TX) - BT RX through a voltage divider (5v to 3.3v)
*/


// FOR HTML Website
char c = ' ';
boolean NL = true;

// FOR light dependant resistor
int LDRPin = A0; // Pin used to analog.read the LDR value.
boolean DoorOpen = true; // initialise cupboard door, where kitchen rubbish bin is placed, to be open

// FOR DISTANCE
int pingPin = 6; // Pin that sends and recives ultrasonic waves
float binTop = 5; // Distance from USM to top of rubbish bin in cm
float binBottom = 100; // Distance from USM to bottom of rubbish bin in cm
float full = 0; // full tells how full the bin is in percent.
int roundup = 0; // initialising the variable, which adjust full to specific intervals



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//////////      SETUP
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup()
{
  Serial.begin(9600);

  // Setup for Bluetooth unit
  BTserial.begin(9600);
  BTserial.print("AT+IMME1" ); //Turn off auto connect
  delay(1000);
  BTserial.print("AT+ROLE1" );  // Make HM-10 Master
  delay(1000);
  BTserial.print("AT+CON508CB166140E"); //Connect to known MAC addres of other BT-slave
  delay(1000);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///////////    LOOP START
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop()
{
  DoorStatus(); // Check if door is open. Return boolean DoorOpen
  if (DoorOpen == 1) { // If door is closed
    Distance(); // Measure distance, return full.
    Message(full); // convert full to char. Send char to bluetooth and serial monitor.
  }

  // If door is open: dont send measurement. instead send "f"
  // "f" from the NodeMCU understood as "door is open! can't read"
  else {
    Serial.println("f"); // Send "f" to Bluetooth
    BTserial.print("f"); //
  }
  delay(1000); // Slows the send rate down, to allow the NodeMCU to keep up.

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
////////    LOOP END
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



// Taken from https://www.arduino.cc/en/Tutorial/Ping
long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}

///////////////////////////////////////////////////////////
// This function activates the Ultrasonic distance measurer, and converts the data into centimeters.
// Based on top and bottom of the bin, it then calculates the percentwise fullness of the bin.
// Taken from https://www.arduino.cc/en/Tutorial/Ping
float Distance () {
  // establish variables for duration of the ping, and the distance result in cencimeters:
  float duration, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  delay(2);
  // Calculate bin full percentwise
  full = float(100 - (cm / (binBottom - binTop)) * 100); // Perscenwise full %

  return full;
}


////////////////////////////
//  Takes full, converts to the integer roundup.
//  if roundup fits an interval, the coresponding char wil be sent via Bluetooth.
char Message(float full) {
  roundup = (int) full; 
  if ( roundup < 20) {
    Serial.println("a"); // EMPTY
    BTserial.print("a");
  }

  else if (20 < roundup && roundup < 40) {

    Serial.println("b");  // 30%
    BTserial.print("b");
  }

  else if (40 < roundup && roundup < 60) {
    Serial.println("c");  // 50%
    BTserial.print("c");
  }

  else if ( 60 < roundup && roundup < 85) {
    Serial.println("d");  // 70%
    BTserial.print("d");
  }

  else if (roundup > 85) {
    Serial.println("e");  // FULL
    BTserial.print("e");
  }
}

///////////////////////////////////////////////////
// DoorStatus checks if the cupboard is open.
// This is done to avoid false readings on the egde of the bin, or bellow it if removed for emptying.
// This is done by reading the LDR value.
boolean DoorStatus() {
  int sensorRead = analogRead(LDRPin);
  if (sensorRead < 700) { // if too bright then door is open
    DoorOpen = false;
  }
  else {          // Door closed
    DoorOpen = true;
  }
  return DoorOpen; // Return a boolean used to deside wrether to read from ultrasonic ranger or not.
}

