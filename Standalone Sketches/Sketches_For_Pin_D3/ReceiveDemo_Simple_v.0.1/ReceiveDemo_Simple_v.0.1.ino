/*
  Simple example for receiving
  https://github.com/sui77/rc-switch/
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(4800);
  mySwitch.enableReceive(0);  // We use D7, which is INT6, atmega32u4 chip pin #1 ### https://web.archive.org/web/20140914105154/http://propaneandelectrons.com/blog/int6-on-arduino-leonardo-atmega32u4
}

void loop() {
  if (mySwitch.available()) {
    
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Delay: "); 
    Serial.println( mySwitch.getReceivedDelay() );
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );

    mySwitch.resetAvailable();
  }
}


