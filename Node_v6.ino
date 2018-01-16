/*
  modified 25 May 2012
  by Tom Igoe
  based on Mikal Hart's example
  This example code is in the public domain.
*/
// FOR BLUETOOTH
#include <SoftwareSerial.h>
SoftwareSerial mySerial( D6, D7); // RX, TX
char blu = ' ';

/// FOR WEBSITE
const char* ssid     = "DTU-Networks";  //Hjemme: WhyGoBigWhenYouCanGoHome
const char* password = "Fotonik343"; // Hjemme: internet95

#include <ESP8266WiFi.h> // ESP8266WiFi.h library
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

char bluCheck = ' '; //Copy of c, to check if there has been change in GARBAGE State

WiFiServer server(80);
String HTTP_req;          // stores the HTTP request

char c = ' '; //FOR HTML
boolean NL = true;
String ged = "";


void setup() {
  // Serial print and my serial setup

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

void loop() { // run over and over


  if (mySerial.available()) { // hvid den modtager et signal fra den anden BLE
    blu = mySerial.read();
    Serial.println(blu);
    delay(2);
    Send(blu);

  }
  /*
    if (blu != bluCheck){
    }
  */
  Serial.print(ged);
  HTML(ged);
  

  /*
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
    url += String(full);
    url += "\r\n";
    // delay(1000);
    // Request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
  */

  // REST AF BLUETOOTH


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


//////
// END OF LOOP
/////

String Send(char c) {

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
    //Serial.println("fejl");
  }
  //Serial.println(ged);
  return (ged);

}


void HTML(String ged) {
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
          client.println("<h1>Garbagecan Status</h1>");

          Serial.print(ged);
          client.println(ged);
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
