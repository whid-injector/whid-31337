/*
  Example for receiving
  
  https://github.com/sui77/rc-switch/
  
  If you want to visualize a telegram copy the raw data and 
  paste it into http://test.sui.li/oszi/
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // (6)  We use D7, which is INT6, atmega32u4 chip pin #1 ### https://web.archive.org/web/20140914105154/http://propaneandelectrons.com/blog/int6-on-arduino-leonardo-atmega32u4
}                             //(0)  FF Pin 3, INT0

void loop() {
  if (mySwitch.available()) {
    output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
//Decimal: 15532481 (24Bit) Binary: 111011010000000111000001
    mySwitch.resetAvailable();
  }
}
