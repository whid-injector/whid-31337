/*
 * Sniff ASK/OOK packet and Re-Transmit it after 10 Seconds.
 * Based on RF Sniffer (C) Elia Yehuda 2014
 */
 
#include <RCSwitch.h>
// number of times to resend sniffed value. use 0 to disable.
#define RESEND_SNIFFED_VALUES 50
RCSwitch rfSwitch = RCSwitch();
 
void setup()
{
        Serial.begin(4800);
        // Receiver on interrupt INT0 (Pin #D3 of FF too)
        rfSwitch.enableReceive(0);  
        // Transmitter on pin #11 or pin #7
        rfSwitch.enableTransmit(11);
        rfSwitch.setRepeatTransmit(RESEND_SNIFFED_VALUES);         
}
 
//Decimal-to-binary-ascii procedure
char *tobin32(unsigned long x)
{
        static char b[33];
        b[32] = '\0';  
        for ( int z = 0; z < 32; z++) {
                b[31 - z] = ((x >> z) & 0x1) ? '1' : '0';
        }
        return b;
}
 
void process_rf_value(RCSwitch rfswitch)
{
        char str[120];
        unsigned long value;   
        value = rfswitch.getReceivedValue();
        Serial.println(F("------------------------------------------- SNIFF & REPLAY -------------------------------------------"));
        if (value) {
                sprintf(str, "[O] [Packet Received] Binary: %s / Decimal: %010lu / %02d bit / Protocol = %d", tobin32(value), value, rfswitch.getReceivedBitlength(), rfswitch.getReceivedProtocol() );
                //sprintf(str, "[O] [Packet Received] Binary: %s / Bit Length: %02d bit", tobin32(value), rfswitch.getReceivedBitlength());
        } else {
                sprintf(str, "[o] Packet Received: Unknown encoding (0)");
        }
        
        Serial.println(str);
        delay(1000);
        Serial.println(F("[O] Sending back the packet sniffed in 3 seconds..."));
        delay(1000);
        Serial.println(F("[O] Sending back the packet sniffed in 2 seconds..."));
        delay(1000);
        Serial.println(F("[O] Sending back the packet sniffed in 1 second..."));
        delay(1000);
        Serial.println(F("[O] BOOOM!"));
        rfswitch.send(value, rfswitch.getReceivedBitlength());
        //rfswitch.send(value+1, rfswitch.getReceivedBitlength()); //### PoC
        //delay(1000);
//        for (int i = 0; i <= 11; i++) {
//                  rfswitch.send(value + i, 24);
//                  Serial.print(F("Fuzzing: "));
//                  Serial.println(value + i);
//                  delay(500);
//                }
        Serial.println(F("[O] Packets Sent!"));
        Serial.println(F("-----------------------------------------------------------------------------------------------------"));
        // reset the switch to allow more data to come
        rfswitch.resetAvailable();
}
 
void loop()
{
         if (rfSwitch.available()) {
                process_rf_value(rfSwitch);
        }
}
