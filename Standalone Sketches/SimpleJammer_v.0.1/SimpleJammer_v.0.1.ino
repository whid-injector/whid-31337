  //### Pin D11 or Pin D7 <==> Data (315/433MHz TX)
  //### Pin Vcc <==> Vcc (315/433MHz TX)
  //### Pin GND <==> GND (315/433MHz TX)

void setup() {
  //tone(11, 15000, 5000);
}

void loop() {
  // Transmitter on pin #11 or pin #7
  tone(11, 15000, 10000); // It generates a square wave  Default:15000
  //tone(7, 15000, 10000);
  //tone(11, 15000);
}
