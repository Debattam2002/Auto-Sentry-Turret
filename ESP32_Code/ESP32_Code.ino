#include <ESP32Servo.h>
#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>

#define PAN_PIN     15  
#define TILT_PIN    13  
#define B_GND        4
#define B_DATA       2
#define IR_RECV_PIN 12
#define IR_GND      14
#define IR_VCC      27

IRrecv irrecv(IR_RECV_PIN);
decode_results results;

Servo panServo;
Servo tiltServo;

unsigned long receivedHexValue = 0; // Variable to store the received HEX value

// Define minimum and maximum angles for pan and tilt
const int PAN_MIN_ANGLE = 30;
const int PAN_MAX_ANGLE = 150;
const int TILT_MIN_ANGLE = 60;
const int TILT_MAX_ANGLE = 120;

// Invert the direction of the pan servo if needed
const bool INVERT_PAN_DIRECTION = true;

int targetPanAngle = -1;
int targetTiltAngle = -1;

void setup() {
  Serial.begin(115200); // Initialize serial communication
  pinMode(B_GND, OUTPUT);
  pinMode(B_DATA, OUTPUT);
  pinMode(IR_GND, OUTPUT);
  pinMode(IR_VCC, OUTPUT);
  panServo.attach(PAN_PIN);   // Attach the pan servo to its pin
  tiltServo.attach(TILT_PIN); // Attach the tilt servo to its pin

  digitalWrite(B_GND, LOW);
  digitalWrite(IR_GND, LOW);
  digitalWrite(IR_VCC, HIGH);
  
  calibrateServos(); // Calibrate the servos before starting
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (Serial.available() > 0) { // Check if data is available to read
    String data = Serial.readStringUntil('\n'); // Read the data
    int commaIndex = data.indexOf(','); // Find the index of the comma separating x and y
    if (commaIndex != -1) {
      // Extract x and y coordinates from the data
      int x = data.substring(0, commaIndex).toInt();
      int y = data.substring(commaIndex + 1).toInt();
      
      // Control the servos based on the received coordinates
      targetPanAngle = map(x, 0, 1280, PAN_MIN_ANGLE, PAN_MAX_ANGLE); // Map x coordinate to pan servo angle
      targetTiltAngle = map(y, 0, 720, TILT_MIN_ANGLE, TILT_MAX_ANGLE); // Map y coordinate to tilt servo angle
      
      // Clamp target angles within the defined range
      targetPanAngle = constrain(targetPanAngle, PAN_MIN_ANGLE, PAN_MAX_ANGLE);
      targetTiltAngle = constrain(targetTiltAngle, TILT_MIN_ANGLE, TILT_MAX_ANGLE);
      
      // Invert pan servo direction if needed
      if (INVERT_PAN_DIRECTION) {
        targetPanAngle = PAN_MAX_ANGLE - targetPanAngle;
      }
      
      panServo.write(targetPanAngle); // Set pan servo angle
      tiltServo.write(targetTiltAngle); // Set tilt servo angle

      
    }
  }
  else{
    targetPanAngle= -1;
    targetTiltAngle= -1;
  }

  // Check if servos have reached the target positions
  if (targetPanAngle != -1 && targetTiltAngle != -1) {
      
    // Check for IR data for 1000 milliseconds
    unsigned long startTime = millis();
//    while (millis() - startTime < 500) {
      if (irrecv.decode(&results)) {
        receivedHexValue = results.value; // Store the received HEX value
        Serial.print("0x");
        Serial.println(receivedHexValue, HEX);
        irrecv.resume(); // Receive the next value
//        break;
      }
//    }
    if(receivedHexValue==long(0x777777)){
            digitalWrite(B_DATA,LOW);
          }
          else{
            digitalWrite(B_DATA,HIGH);
            delay(800);
            digitalWrite(B_DATA,LOW);
          }
    

   
  }
}

void calibrateServos() {
  // Move pan servo to extreme positions for calibration
  panServo.write(30);
  delay(1000);
  panServo.write(150);
  delay(1000);
  panServo.write(90); // Move back to center position

  // Move tilt servo to extreme positions for calibration
  tiltServo.write(60);
  delay(1000);
  tiltServo.write(120);
  delay(1000);
  tiltServo.write(90); // Move back to center position

  Serial.println("Servos calibrated. Ready to start.");
}
