# WHID 31337 (aka WHID-Elite) 
WHID Elite is a GSM-enabled Open-Source Multi-Purpose Offensive Device that allows a threat actor to remotely inject keystrokes, bypass air-gapped systems, conduct mousejacking attacks, do acoustic surveillance, RF replay attacks and much more. In practice, is THE Wet Dream of any Security Consultant out there!

Some of the main features of WHIDelite are:
- Remote control over GSM/2G Networks
- Remote HID Injection
- [Bypass Air-Gapped Environments](https://twitter.com/LucaBongiorni/status/963693468282445824)
- [Support for Mousejacking Attacks](https://twitter.com/WHID_Injector/status/1050482412218707968)
- [Radio Hacking related Attacks](https://twitter.com/WHID_Injector/status/1065332359426392068)
- [Embedded USB HUB to easily weaponize USB Gadgets](https://twitter.com/WHID_Injector/status/1067517057074380806)
- GPS Location of WHIDElite
- [Remote Audio Surveillance](https://twitter.com/WHID_Injector/status/1005464541336129536)

[![Black Hat Arsenal Europe](https://github.com/toolswatch/badges/blob/master/arsenal/europe/2017.svg)](http://www.toolswatch.org/2018/01/black-hat-arsenal-top-10-security-tools/) [![Black Hat Arsenal USA](https://github.com/toolswatch/badges/blob/master/arsenal/usa/2018.svg)](https://www.blackhat.com/us-18/arsenal/schedule/index.html#whid-injector-and-whid-elite-a-new-generation-of-hid-offensive-devices-10459) [![ToolsWatch Best Tools](https://github.com/toolswatch/badges/blob/master/toptools/2017.svg)](http://www.toolswatch.org/2018/01/black-hat-arsenal-top-10-security-tools/)

Hardware Design Author: Luca Bongiorni - https://twitter.com/lucabongiorni

For more information about it checkout http://elite.whid.ninja and do follow the news feed at https://twitter.com/WHID_Injector

Disclaimer: This research tool has been designed to be used ONLY by experienced personnel against targets in-scope during a Security Engagement or to raise security awareness during trainings. Neither the manufacturer, nor the Author are responsible for any damage occurred by its misuse.

# Mandatory Hardware Needed:
- **A SIM Card** able to work with a GSM/2G operator. 
- **>= 500mAh LiPo battery** (e.g. <https://tinyurl.com/ycvf2r4x> ) 
	Beside WHID Elite itself, the only further hardware needed for testing WHID Elite is a LiPo battery of at least 500mAh (PERSONALLY I RECOMMEND >600mAh. BETTER >1000mAh).<br/>
	This requirement is dictated by the GSM module SIM800, which in some conditions it requires more Ampere than a standard 500mAh USB port can provide.<br/>	**It is NOT recommended to run WHID-Elite WITHOUT LiPo!!!**
 - To connect the LiPo battery, I do recommend to use JST-PH 1.25 male/female connectors. Thus you will be able to easily detach it when not in use. (e.g. <https://tinyurl.com/y9kmq27b> )
 
  - **[NOTE]** In case you live in a country where the 2G/GSM networks are not available anymore… you will need to create your own GSM network. Which means you will need to run either OpenBTS or OpenBSC with a dedicated HW (e.g. LimeSDR, USRP, BladeRF, NanoBTS, etc.). You will just need to be able to send and receive SMS. No data traffic (i.e. GPRS/EDGE) needed at this stage.
 Said that, you can always use the standalone sketches to test indiviual features (e.g. Mousejacking, Jamming, Transmitting ASK/OOK sequences, etc.)<br/>
 
 - **[FOR WORLD]** In case you have no National Operator that sells GSM SIMs, You can always try to check if your country is supported by https://www.thingsmobile.com Check Coverage here https://www.thingsmobile.com/business/coverage
 
 - **[FOR US]** You can always check if your area is covered by TING, which still provides 2G network within USA  https://ting.com/shop/gsmSIM Check Coverage here https://ting.com/coverage# <br/>
 
 ![image](https://user-images.githubusercontent.com/26245612/52553931-eab8f600-2ded-11e9-9c16-18989a11fc96.png)
 
# Optional Hardware Needed:
- **In case you would like to test the ASKTX, ASKRX and JAM commands as well, you will need an external TX connected on the D11 and/or D7 Pins[o] and the RX on the D3 pin [o]. And a test lab equipment.**
	- 433MHz OR 315MHz Transmitters (e.g. <https://tinyurl.com/yaldp5hx> ) 
	- SDR device to analyze RF packets with URH (e.g. RTL-SDR, LimeSDR, HackRF, etc.)
	- Any RF enabled device (e.g. power-sockets, alarm systems, etc. Example: <https://tinyurl.com/yaorrapx> )
  - 433MHz AND 315MHz Receiver:  as RX, I recommend the one from this <https://tinyurl.com/y8f7tocs> because I discovered that (despite is sold as 433MHz) it can be used to sniff ALSO 315MHz :)
		
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
	

[o]**Wiring of external 433/315MHz TX:**<br/>
GND >> GND<br/>
DATA >> D11/D7<br/>
VCC >> VCC<br/> Use the P3 5V Vcc pin to power it. (If you are planning to use it as portable device, do use a powerbank)

[o]**Wiring of external 433/315MHz RX:**<br/>
GND >> GND<br/>
DATA >> D3<br/>
VCC >> VCC<br/> Use the P3 5V Vcc pin to power it. (If you are planning to use it as portable device, do use a powerbank)

# Pinout:

![WHID Elite Pinout v 0 9](https://user-images.githubusercontent.com/26245612/64094329-04a72f80-cd64-11e9-942b-605f30b9d7cb.jpg)

I recommend to add a switch or JST connectors between the LiPo and the PCB. Thus u can easily turn ON-OFF the board.

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

# Antenna Mod
The NRF24 Module is using by default the internal PCB antenna. However by moving the SMD component (a 0402/47pF capacitor) in the image below you can the route the radio signal to the UFL connector. Thus you will have to attach a 2.4GHz antenna through the UFL connector.

IMPORTANT: Do this ONLY if you know what you are doing. Obviously don't open issues nor complain to the manufacturer if you fuckup something. You got warned!

![image](https://user-images.githubusercontent.com/26245612/67474042-74e37a80-f65c-11e9-89a2-4b7f50a9a7ce.png)



