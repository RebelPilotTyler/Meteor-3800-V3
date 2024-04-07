// MeteorV3_OperatingCode, a program that runs the Meteor 3800 V3 Foam Dart Blaster.
// Written by Tyler Widener, tylerwidenerlm@gmail.com

//Meteor 3800 V3 Specs:
//  Select Fire between Safe, Semi, Auto.
//  Brushless Motors with Speed Control via Potentiometer.
//  Solenoid Pusher for programmable fire.
//  Wiring exists for Muzzle LEDs, not currently implemented.

//----LIBRARIES----
#include <Servo.h> // Servo library for controlling ESCs

//----PINS----
//ESC Signal Pin: 9
//ESC Speed Control Potentiometer: A1
const int solenoidPin = 12; //Solenoid Pin (Has to go through MOSFET)
const int muzzleLEDs = 6; //Muzzle Flash LEDs (Has to go through transistor)
//Fire mode switch is 3-way, detected by two. Semi is off, auto is 1 way, safe is other way.
const int autoSwitch = 4; //Switch is 3 way, this is R-position (Tinkercad) Reads 1 when active
const int safeSwitch = 5; //Switch is 3 way, this is B-position (Tinkercad) Reads 1 when active
const int mainTrig = 3; //Main firing trigger
const int revTrig = 2; //Rev trigger

//----OTHER VARIABLES----
int potValue;  // value from the analog pin
int safeState; //Detects if switch is on safe
int autoState; //Detects if switch is on full auto
int semiState; //Detects if switch is on semi auto
int revState; //Detects if rev trigger is pressed
int trigState; //Detects if main trigger is pressed
int currMode; //Keeps track of current firing mode, 1: Auto | 2: Semi | 3: Safe
int trigState2; //Extra testing variable for firing trigger
int revState2; //Extra testing variable for rev trigger
int semiFired; //Tracks if a semi shot is fired so semi isn't full auto
Servo ESC;     // create servo object to control the ESC

//----MAIN----
void setup() {
  // Attach the ESC on pin 9
  ESC.attach(9,1000,2000); // (pin, min pulse width, max pulse width in microseconds) 
  pinMode(solenoidPin, OUTPUT);
  pinMode(muzzleLEDs, OUTPUT);
  pinMode(autoSwitch, INPUT);
  pinMode(safeSwitch, INPUT);
  pinMode(mainTrig, INPUT);
  pinMode(revTrig, INPUT);
  Serial.begin(9600);
   // ESC Arming Sequence
   /*
  for (int pos = 90; pos <= 91; pos += 1) { 
    ESC.write(pos);             
    delay(3700);                         // Wait for ESC to arm / Exit safety mode
    //Increase this 3700 value depending on how long it takes for your ESC to arm
  } */
  Serial.println("ESCs Armed");
  Serial.println("Ready");
}

void loop() {
  potValue = analogRead(A1);
  potValue = map(potValue, 0, 1023, 0, 180);
  safeState = digitalRead(safeSwitch);//Read the safety
  if (safeState != HIGH) {//If the safety is on, don't let the blaster do anything
    autoState = digitalRead(autoSwitch);//Read the auto switch
    if (autoState == HIGH) {//If full auto is on, track it
      currMode = 1;
    }
    if ((autoState == LOW) && (safeState == LOW)) {//If semi auto is on, track it
      currMode = 2;
    }

    revState = digitalRead(revTrig);//Read the rev trigger
    if (revState == LOW) {//If switch is not pressed, power up motors to speed
      ESC.write(potValue);
      Serial.println("Rev Trigger Pressed");
      revState2 = 1;
      delay(10);
    }
    if (revState == HIGH) {//If switch is pressed, turn off motors
      if (revState2 == 1) {
        //delay(500); // Wait half a second before powering off, just in case another shot is fired
        for(int j = 500; j >= 0; j--) {
          // Interrupt Slowing Down
           if (digitalRead(revTrig) == LOW) {
              break;
           }
           if (digitalRead(mainTrig) == HIGH) {
              break;
           }
           delay(1);
        }
        for (int i = potValue; i >= 0; i--) {
           ESC.write(i);

           // Interrupt Slowing Down
           if (digitalRead(revTrig) == LOW) {
              break;
           }
           if (digitalRead(mainTrig) == HIGH) {
              break;
           }
           
           delay(10);
        }
        revState2 = 0;
      }
    }

    trigState = digitalRead(mainTrig);//Read the main trigger
    if ((revState == LOW) && (trigState == HIGH)) {//If Rev is off and Main is on...
      delay(300); // Delay the shot by just a bit for motors to rev.
      if ((currMode == 2) && (semiFired == 0)) {//If semi auto is active

        //TEST, Auto Rev on Trigger Press
      //  if (digitalRead(revTrig) == LOW) {
     //     ESC.write(potValue);
      //    revState2 = 1;
      //    delay(50);
       // }
        
        digitalWrite(solenoidPin, HIGH);
        delay(90);
        digitalWrite(solenoidPin, LOW);
        digitalWrite(muzzleLEDs, HIGH);
        delay(100);
        digitalWrite(muzzleLEDs, LOW);
        semiFired = 1;
        Serial.println("Shot Fired");
        delay(10);
      }
      if (currMode == 1) {//If full auto is active
        while(trigState == HIGH) {//While both triggers are held down

          //TEST, Auto Rev on Trigger Press
          //if (digitalRead(revTrig) == LOW) {
         //    ESC.write(potValue);
          //   revState2 = 1;
          //   delay(50);
         // }
        
          digitalWrite(solenoidPin, HIGH);
          delay(90);
          //delay(90 * potValue);//Variable Rate of Fire
          digitalWrite(solenoidPin, LOW);
          digitalWrite(muzzleLEDs, HIGH);
          delay(100);
          digitalWrite(muzzleLEDs, LOW);
          Serial.println("Shot Fired");
          trigState = digitalRead(mainTrig);//Update trigger status
          revState = digitalRead(revTrig);
        }
      }
    }
    if (trigState == LOW) {
      semiFired = 0;
    }
  }
  else {//If the safety is active, turn arduino back to base, for emergency stop
    delay(10);
    revState = digitalRead(revTrig);//Read the rev trigger
    if (revState2 == 1) { //Turn off the motors if safety is active as an emergency stop.
      for (int i = potValue; i >= 0; i--) {
          ESC.write(i);

          // Interrupt Slowing Down
          if (digitalRead(revTrig) == LOW) {
            break;
          }
          if (digitalRead(mainTrig) == HIGH) {
            break;
          }
           
          delay(10);
      }
      revState2 = 0;
    }
  }
}