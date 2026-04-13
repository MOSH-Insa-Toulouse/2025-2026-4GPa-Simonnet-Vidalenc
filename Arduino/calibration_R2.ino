/*
code pour déterminer la valeur estimer de R2, et donc du potentiomètre digitale qu'il faudrait sélectionner.
On peut voir s'afficher la sortie en Volt de A2 pour différente valeurs de R2.
*/

#include <SPI.h>

const byte csPin           = 10;      // MCP42100 chip select pin
const int  maxPositions    = 256;     // wiper can move from 0 to 255 = 256 positions
const long rAB             = 92500;   // 100k pot resistance between terminals A and B, 
                                      // mais pour ajuster au multimètre, je mets 92500
const byte rWiper          = 125;     // 125 ohms pot wiper resistance
const byte pot0            = 0x11;    // pot0 addr // B 0001 0001
const byte pot0Shutdown    = 0x21;    // pot0 shutdown // B 0010 0001

float Vadc;

void setPotWiper(int addr, int pos) {
  float V = analogRead(A2);
  Vadc = V * 5.0 / 1023.0;
  pos = constrain(pos, 0, 255);            // limit wiper setting to range of 0 to 255
  digitalWrite(csPin, LOW);                // select chip
  SPI.transfer(addr);                      // configure target pot with wiper position
  SPI.transfer(pos);
  digitalWrite(csPin, HIGH);               // de-select chip

  // print pot resistance between wiper and B terminal
  long resistanceWB = ((rAB * pos) / maxPositions ) + rWiper;
  Serial.print("Wiper position: ");
  Serial.print(pos);
  Serial.print(" Resistance wiper to B terminal: ");
  Serial.print(resistanceWB);
  Serial.println(" ohms");
  Serial.print(Vadc);
  Serial.println(" volts");
}

void setup() {
  Serial.begin(9600);
  Serial.println("MCP41100 Demo");
  
  digitalWrite(csPin, HIGH);        // chip select default to de-selected
  pinMode(csPin, OUTPUT);           // configure chip select as output
  pinMode(A2, INPUT);
  SPI.begin();
}

void loop() {

  float V = analogRead(A2);
  // move pot1 wiper manually to specific positions
  Serial.println("Pot 0 Control ");

  // move pot0 wiper from min to max in increments of 1 position
  Serial.println("Pot 0 Control ");
  for (int i = 0; i < 256; i++) {
    setPotWiper(pot0, i);
    delay(1000);
  }
  // move pot0 wiper from max to min in decrements of 1 position
  for (int i = 256; i > 0; i--) {
    setPotWiper(pot0, i);
    delay(1000);
  }
}


