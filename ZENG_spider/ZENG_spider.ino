/**********************************************************************
  Filename    : Servo sweep and stepper motor turn
  Description : Control the servo motor for sweeping
**********************************************************************/
#include <ESP32Servo.h>
#define ADC_Max 4095    // This is the default ADC max value on the ESP32 (12 bit ADC width); 

/*SERVO*/
Servo myservo;
int posVal = 0;
int servoPin = 32;
int potenPin = 38;
int potenVal;

/*STEPPER*/
int outPorts[] = {14, 27, 26, 25}; //stepper motor port

void setup() {

  randomSeed(analogRead(0));
  /*SERVO*/
  myservo.setPeriodHertz(50);           // standard 50 hz servo
  myservo.attach(servoPin, 500, 2500);  // attaches the servo on servoPin to the servo object
  Serial.begin(115200);

  /*STEPPER*/
  //set pins to output
  for (int i = 0; i < 4; i++) {
    pinMode(outPorts[i], OUTPUT);
  }

}

void loop() {
   long rand;
   rand = random(1, 4);  // returns a value 1, 2, 3
   switch (rand) {
      case 1:
         moveAround(true, 3, 3);
         servoSweep();
         break;

     case 2:
         moveAround(false, 2, 3);
         servoSweep();
         break;

     case 3:
         moveAround(true, 2, 3);
         servoSweep();
         servoSweep();
         servoSweep();
         break;

      default:
         moveAround(false, 1, 3);
         moveAround(true, 1, 3);
 
         break;
   }

  
//    delay(2000);
//    moveAround(true, 3, 3);
//    servoSweep();
//    moveAround(false, 2, 3);
//    servoSweep();
//    moveAround(true, 2, 3);
//    servoSweep();
//    servoSweep();
//    servoSweep();
//    moveAround(false, 1, 3);
//    moveAround(true, 3, 3);
//    moveAround(false, 5, 3);

}

  /*SERVO FUNCTION*/
 void servoSweep() {
    if (posVal < 180){
      for (int p = 0; p <= 60; p += 1) { 
            myservo.write(posVal + p);       // tell servo to go to position in variable 'pos'
            delay(25);                  
      }
      posVal += 60;
    }
    else{
      posVal = 0;
      delay(200);
    }
    
  }

  /*STEPPER FUNCTIONS*/

//Suggestion: the motor turns precisely when the ms range is between 3 and 20
void moveSteps(bool dir, int steps, byte ms) {
  for (unsigned long i = 0; i < steps; i++) {
    moveOneStep(dir); // Rotate a step
    delay(constrain(ms,3,20));        // Control the speed
  }
}

void moveOneStep(bool dir) {
  // Define a variable, use four low bit to indicate the state of port
  static byte out = 0x01;
  // Decide the shift direction according to the rotation direction
  if (dir) {  // ring shift left
    out != 0x08 ? out = out << 1 : out = 0x01;
  }
  else {      // ring shift right
    out != 0x01 ? out = out >> 1 : out = 0x08;
  }
  // Output singal to each port
  for (int i = 0; i < 4; i++) {
    digitalWrite(outPorts[i], (out & (0x01 << i)) ? HIGH : LOW);
  }
}

void moveAround(bool dir, int turns, byte ms){
  for(int i=0;i<turns;i++)
    moveSteps(dir,32*64,ms);
}
void moveAngle(bool dir, int angle, byte ms){
  moveSteps(dir,(angle*32*64/360),ms);
}
