#include <IRremote.h>

const int IR_SEND_PIN = 3; // The pin the IR LED is connected to
IRsend irsend;

void setup() {
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
  irsend.begin(3);
  Serial.begin(9600);
  Serial.println("IR Transmitter Initialized");
}

void loop() {
  unsigned long hexValue = 0x7777777; // Replace with your HEX value
//  Serial.print("Sending HEX value: ");
//  Serial.println(hexValue, HEX);
//  for(int i=0;i<3;i++){
  irsend.sendNEC(hexValue, 32); // Sending NEC protocol with 32 bits
//  delay(40);
//  }
  delay(600); // Wait 0.55 seconds before sending again
}
