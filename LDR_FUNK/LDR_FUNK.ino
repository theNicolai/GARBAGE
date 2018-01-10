
int Pin = A0;
int sensorRead = 0;
boolean DoorOpen = true;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(Pin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print(DoorStatus());
  delay(100);
}

boolean DoorStatus() {

  sensorRead = analogRead(Pin);

  if (sensorRead < 800) {
    DoorOpen = false;
    Serial.println(sensorRead);
  }
  else {
    DoorOpen = true;
    Serial.println(sensorRead);
  }
  return DoorOpen;
}

