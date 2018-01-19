/*
  This script is compatible with UNO_v10_clean
  This script recives a char via bluetooth, from the UNO.
  This char is deciphered and sent to ThingSpeak, and
  a local Website hosted on the NodeMCU esp8266.
  Hardware:
  -  NodeMCU esp8266
  -  HM-10 BLE (conected to another HM-10 BLE)
*/


// FOR BLUETOOTH
#include <SoftwareSerial.h>
SoftwareSerial mySerial(D6, D7); // RX, TX read/write pins to BT-unit
char blu = 'a'; // "raw-data" recived form BlueTooth. Used in Send() and ThinkValue() functions.

/// FOR WEBSITE
const char* ssid     = "DTU-Networks";  // Network name
const char* password = "Fotonik343"; // Network password

#include <ESP8266WiFi.h> // ESP8266WiFi.h library
#include <ESP8266WebServer.h> //ESP8266WebServer.h library
#include <WiFiClient.h> // WiFiClient.h library

WiFiServer server(80);    // Creates a server that listens for incoming connections on port 80.
String HTTP_req;          // Stores the HTTP request.

char c = ' '; //the char sent from the Arduino UNO, to be used in function Send()
String GarbageStatus = "GarbageStatus"; // Return String from Send() function. Contains bin-status for HTLM-website.


//// FOR ThingSpeak
int full = 0; // Sends an integer value to ThingSpeak, with the garbage amount in percent.
const char* host = "api.thingspeak.com";      // API
const char* writeAPIKey = "1Z76DOYEDDLQ132R"; // API-Key


//// FOR Timetracking in loop
unsigned long HtlmPrevMillis = 0;
unsigned long ThingSpeakPrevMillis = 0;

const long HtlmInterval = 8 * 1000; // 8 seconds
const long ThingSpeakInterval = 30 * 1000;  // 30 seconds




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
////////// SETUP
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
  // Serial print and my serial setup
  Serial.begin(9600);
  Serial.println("Jeg starter");


  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600); // Set Baud rate for Bluetooth
  delay(100);

  //  Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Server started");
  WiFiClient client = server.available();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/////////////// LOOP START
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void loop() {

  // Counts milliseconds from program start. Will overflow in approx 50 days.
  // See https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
  unsigned long currentMillis = millis();

  /// FOR BlueTooth
  if (mySerial.available()) { // If BT-unit recives data
    blu = mySerial.read(); // Asign incomming char to blu
    delay(2);
    Send(blu); // Call function Send()
    Serial.println(GarbageStatus); // Show return from Send()
  }

  ///// HTLM WEBSITE
  //If 8 seconds or more has passed, then re-run the Website (refresh the info on the page)
  if (currentMillis - HtlmPrevMillis >= HtlmInterval) {
    HTML(GarbageStatus);          //Call function HTLM(). Run website (update)
    HtlmPrevMillis = currentMillis; // Reset timer
  }

  //If 30 seconds or more has passed, then send current trash-percentage to ThinkSpeak. (refresh the info on the page)
  if (currentMillis - ThingSpeakPrevMillis >= ThingSpeakInterval) {
    Serial.println("THINK THINK THINK!!!!!");
    ThingSpeakFunc();     // Call function ThinSpeakFunc(). (Send data to ThingSpeak)
    ThingSpeakPrevMillis = currentMillis; // Reset timer.
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END OF LOOP
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// The function Send is used to "decrypt" the char sent from the Arduino UNO. (See function Message in NODE code).
// The Arduino UNO sends either a ('a','b','c','d','e','f').
// This is converted to a string, ready for insertion in the HTML function.

String Send(char c) {
  if (c == 'a') {
    GarbageStatus = "10% full";
  }

  else if (c == 'b') {
    GarbageStatus = "30% full";
  }

  else if (c == 'c') {
    GarbageStatus = "50% full";
  }

  else if (c == 'd') {
    GarbageStatus = "70% full";
  }

  else if (c == 'e') {
    GarbageStatus = "100% full";
  }

  else if (c == 'f') {
    GarbageStatus = "door open, unable to read";
  }

  else { //The char is discarded
  }
  return (GarbageStatus);
}

//////////////////

//Similar to Send(), this function takes the same char ('a','b','c','d','e','f'),
// but converts into a int, ready for the ThinkSpeakFunc().
int ThinkValue(char c) {
  if (c == 'a') {
    full = 10;
  }

  else if (c == 'b') {
    full = 30;
  }

  else if (c == 'c') {
    full = 50;
  }

  else if (c == 'd') {
    full = 70;
  }

  else if (c == 'e') {
    full = 100;
  }

  else {
  }
  return (full);
}

///////////

// This function sends the garbage full percentage to ThinkSpeak.
// It uses the the ThinkValue() function to send an int instead of a char.
// Code built uppon: http://www.instructables.com/id/Send-sensor-data-DHT11-BMP180-to-ThingSpeak-with-a/


void ThingSpeakFunc() {
  // ThingSpeak
  delay(500);
  // make TCP connections
  const int httpPort = 80;
  WiFiClient client = server.available();
  if (!client.connect(host, httpPort)) {
    return;
  }
  String url = "/update?key=";
  url += writeAPIKey;
  url += "&field1=";
  url += String(ThinkValue(blu));
  url += "\r\n";

  // Request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
}


//////////////////////////////

// This function sends the converted sensordata to at website hosted on the NodeMCU espESP8266.
// It uses the return value from the Send function.
// Built upon this code: http://web-files.ait.dtu.dk/mnpe/WiFiWebServer_LED.txt

void HTML(String GarbageStatus) {
  // WEBSERVER HTML
  WiFiClient client = server.available();

  if (client) {  // got client?
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {   // client data available to read
        char c = client.read(); // read 1 byte (character) from client
        HTTP_req += c;  // save the HTTP request 1 char at a time
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 5"); // Refresh webpage
          client.println();
          // send web page
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Arduino Garbage Control</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1>Garbage Can Status:</h1>");

          client.println("<b>Current Garbage Status: </b>");
          client.println(GarbageStatus);
          client.println("</form>");
          client.println("<body>");
          client.println("<html>");

          Serial.print(HTTP_req);
          HTTP_req = "";    // finished with request, empty string
          break;
        }
        // every line of text received from the client ends with \r\n
        if (c == '\n') {
          // last character on line of received text
          // starting new line with next character read
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // a text character was received from client
          currentLineIsBlank = false;
        }
      } // end if (client.available())
    } // end while (client.connected())
    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
  } // end if (client)
}
