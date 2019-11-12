/* Simple sketch to send an arbitrary binary sequence!
For some reasons there is a bug in rcswitch library, which adds one zero more... so remove the last one from the  binary you want to TX!!!
Example, the right signal is:
1010010111101011000001000 // original signal
101001011110101100000100   // to use (see! one zero less at the end) */


#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
int timeDelay = 135;

void setup() {
  Serial.begin(4800);
  Serial.println(F("[+] Botting Up..."));
 // Transmitter on pin #11 or pin #7
 mySwitch.enableTransmit(11); //Pin D11
 // set protocol (default is 1, will work for most outlets)
 // mySwitch.setProtocol(2);
 // Optional set pulse length. (this must be set after protocol (if set). Otherwise is overridden).
 //mySwitch.setPulseLength(456);
 // Optional set number of transmission repetitions.
 mySwitch.setRepeatTransmit(5);
}

void loop() {
// Use this to add manually specific HIGH or LOW pulses with specific length.
//      int highLength = 4;
//      int lowLength = 18;
//      digitalWrite(11, HIGH);     
//      delayMicroseconds(highLength*timeDelay); 
//      digitalWrite(11,LOW);     
//      delayMicroseconds(lowLength*timeDelay); 

// #### For some reasons there is a bug... rcswitch adds one zero more... so remove the last one from the signal binary!!!
// ### Example, the right signal is 
// 1010010111101011000001000 // original signal
// 101001011110101100000100 //to use (see! one zero less at the end)
 Serial.println(F("[+] Sending Packets!"));
 
//mySwitch.send("101001011110101100000100");  // GSM Alarm at 433MHz
mySwitch.send("010101010101010100001100");  // RFID RFLock at 315MHz
//mySwitch.send("011101101101010011101000");  // Crane Packet?!

 delay(2000);
}
