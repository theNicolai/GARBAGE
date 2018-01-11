/*
  Software serial multple serial test

  Receives from the hardware serial, sends to software serial.
  Receives from software serial, sends to hardware serial.

  The circuit:
   RX is digital pin 10 (connect to TX of other device)
   TX is digital pin 11 (connect to RX of other device)

  Note:
  Not all pins on the Mega and Mega 2560 support change interrupts,
  so only the following can be used for RX:
  10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

  Not all pins on the Leonardo and Micro support change interrupts,
  so only the following can be used for RX:
  8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

  created back in the mists of time
  modified 25 May 2012
  by Tom Igoe
  based on Mikal Hart's example

  This example code is in the public domain.

*/
#include <SoftwareSerial.h>

SoftwareSerial mySerial( D6, D7); // RX, TX


char c = ' ';
boolean NL = true;





void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);



  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Sketch:   ");   Serial.println(__FILE__);
  Serial.print("Uploaded: ");   Serial.println(__DATE__);
  Serial.println(" ");
  Serial.println("BTserial started at 9600");



  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  delay(100);
  mySerial.println("Hello, world?");
  delay(100);
}

void loop() { // run over and over


  if (mySerial.available()) { // hvid den modtager et signal fra den anden BLE
    c = mySerial.read();
    Serial.println(c);
    

  }
  delay(2);
  Send(c);
  
  

  if (Serial.available()) // Hvis man vil sende noget til BLE
  {
    c = Serial.read();
    // do not send line end characters to the HM-10
    if (c != 10 & c != 13 )
    {
      mySerial.write(c);
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


void Send(char c) {
  String ged;
  if (c == 'a') {
    ged = "10%";

  }
  else if (c == 'b') {
    ged = "30%";

  }
  else if (c == 'c') {
    ged = "50%";
  }
  else if (c == 'd') {
    ged = "70%";

  }
  else if (c == 'e') {
    ged = "FULL";

  }
  else if (c == 'f') {
    ged = "door open, unable to read";
  }
  else {
    Serial.println("fejl");
  }
  Serial.println(c);

}




