//  SerialIn_SerialOut_HM-10_01
//
//  Uses hardware serial to talk to the host computer and AltSoftSerial for communication with the bluetooth module
//
//  What ever is entered in the serial monitor is sent to the connected device
//  Anything received from the connected device is copied to the serial monitor
//  Does not send line endings to the HM-10
//
//  Pins
//  BT VCC to Arduino 5V out.
//  BT GND to GND
//  Arduino D8 (SS RX) - BT TX no need voltage divider
//  Arduino D9 (SS TX) - BT RX through a voltage divider (5v to 3.3v)
//

#include <AltSoftSerial.h>
AltSoftSerial BTserial;
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html


char c = ' ';
boolean NL = true;

// FOR LDR
int LDRPin = A0;
boolean DoorOpen = true; // initialise door to open

// FOR DISTANCE
int pingPin = 6; // Pin that sends and recives ultrasonic waves
float binTop = 5; // Distance from USM to top of rubbish bin
float binBottom = 100; // Distance from USM to bottom of rubbish bin
float full = 0; // initialising the variable full
int roundup = 0; // initialising the variable, which adjust full to specific intervals




void setup()
{



  Serial.begin(9600);
  Serial.print("Sketch:   ");   Serial.println(__FILE__);
  Serial.print("Uploaded: ");   Serial.println(__DATE__);
  Serial.println(" ");

  BTserial.begin(9600);
  Serial.println("BTserial started at 9600");
  BTserial.print("AT+IMME1" );
  delay(1000);
  BTserial.print("AT+ROLE1" );
  delay(1000);
  BTserial.print("AT+CON508CB166140E");
  delay(1000);
  Serial.println("Connected!");
  delay(10000);

}

void loop()
{
  boolean DoorCheck;
  DoorCheck = DoorStatus();
  if (DoorCheck == 1) { // If door is closed
    Distance();
    Message(full);
    //BTserial.println(full);


  }
  else { // If door is open
    Serial.println("f");
    BTserial.print("f");
  }
  delay(200);
  float Dis;








  // Read from the Bluetooth module and send to the Arduino Serial Monitor
  if (BTserial.available())
  {
    c = BTserial.read();
    Serial.write(c);
  }


  // Read from the Serial Monitor and send to the Bluetooth module
  if (Serial.available())
  {
    c = Serial.read();

    // do not send line end characters to the HM-10
    if (c != 10 & c != 13 )
    {
      BTserial.write(c);
    }

    // Echo the user input to the main window.
    // If there is a new line print the ">" character.
    if (NL) {
      Serial.print("\r\n>");
      NL = false;
    }
    Serial.write(c);
    if (c == 10) {
      NL = true;
    }
  }
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}

float Distance () {
  // FOR DISTANCE
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
  full = float(100 - (cm / (binBottom - binTop)) * 100); // Perscenwise full %

  return full;
}

char Message(float full) {
  // Print Cupboard open or closed and amount of garbage in percent
  // if (DoorOpen == 0) {

  roundup = (int) full; // Makes full an integer
  if ( roundup < 20) {

    Serial.println("a"); // EMPTY
    BTserial.print("a");
  }
  else if (20 < roundup && roundup < 40) {

    Serial.println("b");
    BTserial.print("b");
  }
  else if (40 < roundup && roundup < 60) {

    Serial.println("c");
    BTserial.print("c");
  }
  else if ( 60 < roundup && roundup < 85) {

    Serial.println("d");
    BTserial.print("d");
  }
  else if (roundup > 85) { // FULL

    Serial.println("e");
    BTserial.print("e");
  }

  // }
}


boolean DoorStatus() {

  int sensorRead = analogRead(LDRPin);

  if (sensorRead < 800) { // door open
    DoorOpen = false;
    // Serial.println(sensorRead);
  }
  else {          // Door closed
    DoorOpen = true;
    // Serial.println(sensorRead);
  }
  return DoorOpen;
}

