Simple sketch to send an arbitrary binary sequence!
Transmitter on pin #11 or pin #7

For some reasons there is a bug in rcswitch library, which adds one zero more... so remove the last one from the  binary you want to TX!!!
Example, the right signal is:
1010010111101011000001000 // original signal
101001011110101100000100   // to use (see! one zero less at the end)