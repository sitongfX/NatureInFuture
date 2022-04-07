#include <WiFi.h>
#include <HTTPClient.h>
#define USE_SERIAL Serial
#include <ESP32Servo.h>

const char *ssid_Router     = ""; //Enter the router name
const char *password_Router = ""; //Enter the router password
Servo myservo;

// Defines the number of steps per rotation
int posVal = 0;
int servoPin = 17;
int state = 30;
int outPorts[] = {12, 15, 13, 2};

String address= "";

void setup(){
   for (int i = 0; i < 4; i++) {
    pinMode(outPorts[i], OUTPUT);
  }
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2500);
  
  USE_SERIAL.begin(115200);

  WiFi.begin(ssid_Router, password_Router);
  USE_SERIAL.println(String("Connecting to ")+ssid_Router);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    USE_SERIAL.print(".");
  }
  USE_SERIAL.println("\nConnected, IP address: ");
  USE_SERIAL.println(WiFi.localIP());
  USE_SERIAL.println("Setup End");
}



void moveSteps(bool dir, int steps, byte ms){
  for (unsigned long i=0; i<steps;i++){
    moveOneStep(dir);
    delay(constrain(ms,3,20));
  }
}

void moveOneStep(bool dir) {
// Define a variable, use four low bit to indicate the state of port
static byte out = 0x01;
// Decide the shift direction according to the rotation direction
if (dir) { // ring shift left
  out != 0x08 ? out = out << 1 : out = 0x01;
}
else { // ring shift right
  out != 0x01 ? out = out >> 1 : out = 0x08;
}
for (int i = 0; i < 4; i++) {
digitalWrite(outPorts[i], (out & (0x01 << i)) ? HIGH : LOW);
}
  
}
void moveAround(bool dir, int turns, byte ms){
  for(int i=0;i<turns;i++){
    moveSteps(dir,32*63.68395,ms);
  }
}

void moveAngle(bool dir, int angle, byte ms){
  moveSteps(dir,(angle*32*63.68395/360),ms);
}

 
void loop() {
  
  if((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(address);
 
    int httpCode = http.GET(); // start connection and send HTTP header
    if (httpCode == HTTP_CODE_OK) { 
        String response = http.getString();
        if (response.equals("false")) {
            // Do not run sculpture, perhaps sleep for a couple seconds
        }
        else if(response.equals("true")) {


          for(int i = 0 ; i< 200;i++){
            // Run sculpture
               moveAround(true, 2, 3);
                state = state + 40;
                myservo.write(state);
                if(state == 110){
                  state = 30;
                  myservo.write(state);
                }

          }

            
        }
        USE_SERIAL.println("Response was: " + response);
    } else {
        USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    delay(500); // sleep for half of a second
  }
}
