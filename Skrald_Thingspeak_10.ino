#include <ESP8266WiFi.h> // ESP8266WiFi.h library

const char* ssid     = "DTU-Networks";
const char* password = "Fotonik343";
const char* host = "api.thingspeak.com";
const char* writeAPIKey = "1Z76DOYEDDLQ132R";

// FOR LDR
int sensorPin = A0; // select the input pin for LDR
int sensorValue = 0; // variable to store the value coming from the sensor
boolean DoorOpen = true; // initialise door to open

// FOR DISTANCE
int pingPin = D6; // Pin that sends and recives ultrasonic waves
float binTop = 5; // Distance from USM to top of rubbish bin
float binBottom = 100; // Distance from USM to bottom of rubbish bin
float full = 0; // initialising the variable full
int roundup = 0; // initialising the variable, which adjust full to specific intervals
float duration, cm; // establish variables for duration of the ping, and the distance result in cencimeters

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance to the trash.
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}

void setup() {
  Serial.begin(9600);

  //  Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {

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

  full = float(100 - (cm / (binBottom - binTop)) * 100);
  delay(500);
  Serial.println(full);

  // make TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    return;
  }

  String url = "/update?key=";
  url += writeAPIKey;
  url += "&field1=";
  url += String(full);
  url += "\r\n";

  // Request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(100);
}
