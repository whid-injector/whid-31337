WHID_TestSuite.ino is a testing suite, based on the existing one from FONA library, that allows to test if the SIM800L works properly.
I added also the command K which is used to check if the HID emulation works properly on Windows. It will run usual calc.exe
Once uploaded the sketch, open SerialMonitor. SIM800 will boot and you can type ? To see the commands available.
TODO: Finish to add k command and all other possible unit-tests to check WHID Elite's functionalities. E.g. spy, askTX, askRX, etc...
