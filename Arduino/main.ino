/*
Projet capteur - Basile Vidalenc et Max Simonnet
Code permettant l'envoie de donnée sur la résistance du potentiomètre digital, du flex sensor et du capteur à base de graphite sur un écran OLED et notre appli bluetooth et sur le serial monitor
Pour réaliser ce code nous nous sommmes inspiré du code réalisé précédement par Maël LE BLANC, Daniel PERALES RIOS. 
Lien de leur Github : https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-LE-BLANC--PERALES-RIOS
*/

#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <SPI.h>

////////////////////////////// --- INITIALISATION DES VARIABLES --- //////////////////////////////

// --- Paramètres matériels de l'arduino ---
#define encoderPinA 2
#define encoderPinB 4
#define Switch 5
#define nombreDePixelsEnLargeur 128
#define nombreDePixelsEnHauteur 64
#define brocheResetOLED -1
#define adresseI2CecranOLED 0x3C

#define MCP_WRITE 0b00010001
#define rxPin 0 
#define txPin 1 
#define baudrate 9600

Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED); 
SoftwareSerial mySerial(rxPin, txPin);
char data_[16]; // Buffer pour stocker des chaînes de caractères

// --- GRAPHITE SENSOR ---
float range = pow(10,-6);
float Vadc = 0.0;
float R1 = 100000.0;
float R3 = 100000.0;
float R5 = 10000.0;
float Rc = 0.0; // Résistance finale mesurée du capteur graphite

// --- FLEX SENSOR ---
const int flexPin = A1;
const float VCC = 5.0;
const float R_DIV = 29000.0;
const float flatResistance = 37000.0; // Résistance sans contrainte sur le flex sensor
const float bendResistance = 110000.0; // Résistance à 90°
float Rflex = 0.0;
float angle = 0.0;

// --- VARIABLES DE CONTROLE ---
int lastSwitchState = HIGH;
long lastEncoderValue = 0;
long lastPotValue = 0;
long lastDebounceTime = 0;
long debounceDelay = 40;
bool button_pressed = false;
volatile long encoderValue = 0;
volatile long encoder_for_pot = 0;

bool mainMenu = true;
const char* menuItems[] = {"Potentiometer", "Flex Sensor", "Graphite Sensor"};
int selectMenu = 0;

// --- DIGITAL POTENTIOMETER ---
const int ssMCPin = 10; // Déclaration de la Pin sur l'Arduino
float rWiper = 125.0;
float max_value_pot = 50000.0;
int valuePot = 10;
float R2 = (float(valuePot)/256.0)*max_value_pot;

/////////////////////////////////// --- SETUP --- ///////////////////////////////////
void setup() {
    Serial.begin(9600);
    
    // Définition du Bus SPI
    pinMode(ssMCPin, OUTPUT);
    digitalWrite(ssMCPin, HIGH);

    // Initialisation de l'écran
    delay(1000); // Stabilisation de la tension
    if (!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED)) {
       Serial.println(F("OLED introuvable"));
       while (1); // Bloque le programme si l'écran ne répond pas
    }
    
    ecranOLED.clearDisplay();
    ecranOLED.setTextColor(SSD1306_WHITE);
    ecranOLED.setCursor(20, 25);
    ecranOLED.print(F("INITIALISATION..."));
    ecranOLED.display(); 

    // Initialisation des autres composants connectés à l'Arduino
    pinMode(encoderPinA, INPUT_PULLUP);
    pinMode(encoderPinB, INPUT_PULLUP);
    pinMode(Switch, INPUT_PULLUP);
    
    SPI.begin();
    mySerial.begin(baudrate);
    attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, RISING);  // Lie l'interruption sur le front montant de la pin A
    
    delay(1000);
    Serial.println(F("System Ready"));
}

//////////////////////////////////// --- LOOP --- ////////////////////////////////////
void loop() {
    if (mainMenu) {
        displayMenu();
        updateSelectedItem(encoderValue, 3);
        switchButton();
    } else {
        handleMenuItemSelection(selectMenu);
        switchButton();
    }
    delay(50); // Plus réactif
}

////////////////////////////////// --- FONCTIONS DES COMPOSANTS --- //////////////////////////////////

// --- FONCTIONS AFFICHAGE ---
void displayMenu() {
    ecranOLED.clearDisplay();
    for (int i = 0; i < 3; i++) {
        if (i == selectMenu) {
            ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Surlignage
        } else {
            ecranOLED.setTextColor(SSD1306_WHITE);
        }
        ecranOLED.setCursor(0, i * 15);
        ecranOLED.print(menuItems[i]);
    }
    ecranOLED.display();
}

void displayPotentiometer(int val) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextColor(SSD1306_WHITE);
    SPIWrite(MCP_WRITE, val, ssMCPin);
    
    ecranOLED.setCursor(0, 0);
    ecranOLED.print(F("DigiPot Setting"));
    ecranOLED.setCursor(0, 20);
    ecranOLED.print(F("R2: ")); ecranOLED.print(R2); ecranOLED.print(F(" Ohm"));
    
    ecranOLED.setCursor(0, 50);
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    ecranOLED.print(F("< BACK"));
    ecranOLED.display();
}

void displayFlexSensor() {
    ecranOLED.clearDisplay();
    ecranOLED.setTextColor(SSD1306_WHITE);
    flexMeasure();
    
    ecranOLED.setCursor(0, 0);
    ecranOLED.print(F("Flex Res: ")); ecranOLED.print(Rflex);
    ecranOLED.setCursor(0, 20);
    ecranOLED.print(F("Angle: ")); ecranOLED.print(angle);
    
    ecranOLED.setCursor(0, 50);
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    ecranOLED.print(F("< BACK"));
    
    sendBluetooth(); 
    ecranOLED.display();
}

void displayGraphiteSensor() {
    ecranOLED.clearDisplay();
    ecranOLED.setTextColor(SSD1306_WHITE);
    graphiteMeasure();
    
    ecranOLED.setCursor(0, 0);
    ecranOLED.print(F("Graphite Res:"));
    ecranOLED.setCursor(0, 20);
    ecranOLED.print(Rc); ecranOLED.print(F(" MOhm"));
    
    ecranOLED.setCursor(0, 50);
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    ecranOLED.print(F("< BACK"));
    
    sendBluetooth();
    ecranOLED.display();
}

/////////////////////////////////// --- MESURES --- ///////////////////////////////////

void graphiteMeasure() {
    float V = analogRead(A2);
    Vadc = V * 5.0 / 1023.0;
    // Formule corrigée avec flottants
    Rc = ((R1 * (1.0 + R3 / R2) * (VCC / Vadc)) - R1 - R5) * range; // Résistance finale du capteur graphite
}

void flexMeasure() {
    int ADCflex = analogRead(flexPin);
    float Vflex = (ADCflex / 1023.0) * VCC;
    Rflex = R_DIV * (VCC / Vflex - 1.0);
    angle = map(Rflex, flatResistance, bendResistance, 0, 90.0);
}

// Fonction appelée par l'interruption à chaque cran de l'encodeur
void updateEncoder() {
    if (mainMenu) {
        if (digitalRead(encoderPinB) == LOW) encoderValue++;
        else encoderValue--;
    } else if (selectMenu == 0) {
        if (digitalRead(encoderPinB) == LOW) encoder_for_pot++;
        else encoder_for_pot--;
    }
}
// Gestion de la sélection cyclique
void updateSelectedItem(int encVal, int NumberOfItems) {
    int val = abs(encVal % NumberOfItems);
    if (val != lastEncoderValue) {
        selectMenu = val;
        lastEncoderValue = val;
    }
}
// Mise à jour de la valeur numérique du potentiomètre
void updatePotentiometerValue() {
    if (encoder_for_pot != lastPotValue) {
        valuePot = constrain(valuePot + (encoder_for_pot > lastPotValue ? 5 : -5), 0, 255);
        lastPotValue = encoder_for_pot;
    }
}

void SPIWrite(uint8_t cmd, uint8_t res, uint8_t ssPin) { // Envoi effectif des données au MCP41xxx
    updatePotentiometerValue(); // On calcule d'abord la valeur
    R2 = ((max_value_pot * res) / 256.0) + rWiper; // On calcule la résistance réelle correspondante pour l'affichage 
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0)); // On démarre la transaction SPI 
    digitalWrite(ssPin, LOW); // On active le composant (CS bas)
    SPI.transfer(cmd); // On envoie l'octet de commande
    SPI.transfer(res); // On envoie la valeur de résistance 
    digitalWrite(ssPin, HIGH); // On désactive le composant (CS haut)
    SPI.endTransaction(); // On termine la transaction SPI
} 


void switchButton() {
    if (digitalRead(Switch) == LOW && (millis() - lastDebounceTime) >= debounceDelay) {
        lastDebounceTime = millis();
        mainMenu = !mainMenu; // Bascule menu principal / sous-menu
        while(digitalRead(Switch) == LOW); // Attendre relâchement
    }
}

void sendBluetooth() {
  /* * On prépare la ligne au format : Potentiometre|Flex|Graphite
   * On utilise directement les variables globales calculées dans les autres fonctions
   */

  // Envoi de la valeur R2 (Potentiomètre)
  mySerial.print(R2); 
  mySerial.print("|");
  Serial.print(R2); Serial.print("|");

  // Envoi de la valeur Rflex (Flex)
  mySerial.print(Rflex);
  mySerial.print("|");
  Serial.print(Rflex); Serial.print("|");

  // Envoi de la valeur Rc (Graphite)
  mySerial.println(Rc); 
  Serial.println(Rc);
}

// Switch pour choisir quel écran afficher
void handleMenuItemSelection(int sel) {
    switch (sel) {
        case 0: displayPotentiometer(valuePot); break;
        case 1: displayFlexSensor(); break;
        case 2: displayGraphiteSensor(); break;
    }
}