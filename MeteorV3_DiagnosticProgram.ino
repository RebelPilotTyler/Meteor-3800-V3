// MeteorV3_DiagnosticProgram, a program to check switch states and times for the Meteor 3800 V3
// Written for the Meteor 3800 V3, an Arduino Controlled Nerf Blaster
// Written by Tyler Widener, tylerwidenerlm@gmail.com

const int selector1 = 4;
const int selector2 = 5;
const int revTrig = 2;
const int mainTrig = 3;

int startTime = 0; // Average time between triggers is about 353.52ms

void setup() {
  pinMode(selector1, INPUT); // if selector is at AUTO, 1. // If both of these are 0, selector is at SEMI
  pinMode(selector2, INPUT); // if selector is at SAFE, 1.
  pinMode(revTrig, INPUT); // 1 when pressed, 0 when depressed
  pinMode(mainTrig, INPUT); // 1 when pressed, 0 when depressed
  Serial.begin(9600);
}
void loop() {
  while(digitalRead(revTrig)) {}
  startTime = millis();
  while(!digitalRead(mainTrig)) {}
  Serial.print("Time Between Triggers: ");
  Serial.println(millis() - startTime);
  while(!digitalRead(revTrig)) {}
}