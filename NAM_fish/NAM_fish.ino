#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>

#define USE_SERIAL Serial
Servo myservo;
int servoPin = 17;
int state = 60;
int outPorts[] = {12, 15, 13, 2};


const char *ssid_Router     = ""; //Enter the router name
const char *password_Router = ""; //Enter the router password

String address= "http://134.122.113.13/cn2521/running";

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



void setup(){
  //motors setup
  for (int i = 0; i < 4; i++) {
    pinMode(outPorts[i], OUTPUT);
  }
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2500);

  
  //wifi setup
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
            // Run sculpture
               for(int i = 0; i<2000;i++){
                 //servo
                  state = state + 10;
                  myservo.write(state);
                
                   //stepper
                  moveSteps(true, 5, 3);
                  if(state == 200){
                    state = 0;
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
