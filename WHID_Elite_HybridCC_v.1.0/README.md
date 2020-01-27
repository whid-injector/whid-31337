# Hybrid C&C Workflow SMS & GPRS

This code allows the Hybrid C2 function in WHID Elite.

You will be able to communicate with WHID Elite via SMS and run a GPRS data exfiltration.

New commands:

* hybridC2:

* hybridC2off:

# HybridC2 configuration:

1. Open the WHID_Elite_HybridCC_v.1.0 skecth

2. Add your APN, USER and PASSWORD in the hybridC2 function:

![Enable HybridC2](https://github.com/joelsernamoreno/whid-31337/blob/master/WHID_Elite_HybridCC_v.1.0/images/enablehybrid.png)

3. Add your web server URL in the next section of the code:

![Web Server Configuration](https://github.com/joelsernamoreno/whid-31337/blob/master/WHID_Elite_HybridCC_v.1.0/images/webserver_conf.png)

4. Compile and upload the code to WHID Elite.

**Note:** You can uncomment the following lines to debug the hybridC2 code on the serial monitor.

![Debug](https://github.com/joelsernamoreno/whid-31337/blob/master/WHID_Elite_HybridCC_v.1.0/images/debug.png)

# Use the HybridC2 function with WHID Elite

1. Send "hybridC2:" via SMS to WHID Elite to enable the hybrid function

2. Use any exfiltration command (example: airgapnix:whoami)

3. Disable hybrid communication by sending an SMS to WHID Elite: hybridC2off:

**Demo:** https://twitter.com/JoelSernaMoreno/status/1220039697159856133

**NOTE:** In the demo, Burpsuite is used to set up a private web server. You can use other web servers. You can also use other ports for communication (TCP), but you will have to implement this with other AT commands in the WHID Elite code.
