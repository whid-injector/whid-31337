# WHID 31337 (aka WHID-Elite) 
WHID Elite is a GSM-enabled Open-Source Multi-Purpose Offensive Device that allows a threat actor to remotely inject keystrokes, bypass air-gapped systems, conduct mousejacking attacks, do acoustic surveillance, RF replay attacks and much more. In practice, is THE Wet Dream of any Security Consultant out there!

Some of the main features of WHIDelite are:
- Remote control over GSM/2G Networks
- Remote HID Injection
- Bypass Air-Gapped Environments [0]
- Support for Mousejacking Attacks [1] [2]
- Radio Hacking related Attacks [3] [5]
- Embedded USB HUB to easily weaponize USB Gadgets [4]
- GPS Location of WHIDElite
- Remote Audio Surveillance [6]

[![Black Hat Arsenal Europe](https://github.com/toolswatch/badges/blob/master/arsenal/europe/2017.svg)](http://www.toolswatch.org/2018/01/black-hat-arsenal-top-10-security-tools/) [![Black Hat Arsenal USA](https://github.com/toolswatch/badges/blob/master/arsenal/usa/2018.svg)](https://www.blackhat.com/us-18/arsenal/schedule/index.html#whid-injector-and-whid-elite-a-new-generation-of-hid-offensive-devices-10459) [![ToolsWatch Best Tools](https://github.com/toolswatch/badges/blob/master/toptools/2017.svg)](http://www.toolswatch.org/2018/01/black-hat-arsenal-top-10-security-tools/)

Hardware Design Author: Luca Bongiorni - https://twitter.com/lucabongiorni

For more information about it checkout http://elite.whid.ninja and do follow the news feed at https://twitter.com/WHID_Injector

Disclaimer: This research tool has been designed to be used ONLY by experienced personnel against targets in-scope during a Security Engagement or to raise security awareness during trainings. Neither the manufacturer, nor the Author are responsible for any damage occurred by its misuse.

[0] https://twitter.com/LucaBongiorni/status/963693468282445824
[1] https://www.mousejack.com/
[2] https://twitter.com/WHID_Injector/status/1050482412218707968
[3] https://twitter.com/WHID_Injector/status/1065332359426392068
[4] https://twitter.com/WHID_Injector/status/1067517057074380806
[5] https://twitter.com/WHID_Injector/status/1066425435851055104
[6] https://twitter.com/WHID_Injector/status/1005464541336129536

# Mandatory Hardware Needed:
- **A SIM Card** able to work with a GSM/2G operator.
- **>= 500mAh LiPo battery** (e.g. <https://tinyurl.com/ycvf2r4x> ) 
	Beside WHID Elite itself, the only further hardware needed for testing WHID Elite is a LiPo battery of at least 500mAh (PERSONALLY I RECOMMEND >600mAh. BETTER >1000mAh).<br/>
	This requirement is dictated by the GSM module SIM800, which in some conditions it requires more Ampere than a standard 500mAh USB port can provide.<br/>	**It is NOT recommended to run WHID-Elite WITHOUT LiPo!!!**
 - To connect the LiPo battery, I do recommend to use JST-PH 1.25 male/female connectors. Thus you will be able to easily detach it when not in use. (e.g. <https://tinyurl.com/y9kmq27b> )
 
 - **[FOR WORLD]** In case you have no National Operator that sells GSM SIMs, You can always try to check if your country is supported by https://www.thingsmobile.com
 
 - **[FOR US/CANADA/JAPAN]** In case you live in a country where the 2G/GSM networks are not available anymore… you will need to create your own GSM network. Which means you will need to run either OpenBTS or OpenBSC with a dedicated HW (e.g. LimeSDR, USRP, BladeRF, NanoBTS, etc.). You will just need to be able to send and receive SMS. No data traffic (i.e. GPRS/EDGE) needed at this stage.
 Said that, you can always use the standalone sketches to test indiviual features (e.g. Mousejacking, Jamming, Transmitting ASK/OOK sequences, etc.)<br/>
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
