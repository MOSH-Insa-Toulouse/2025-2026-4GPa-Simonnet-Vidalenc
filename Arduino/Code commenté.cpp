/* Projet capteur - Basile Vidalenc et Max Simonnet */ // Noms des auteurs du projet
/* Code permettant l'envoie de donnée sur la résistance du potentiomètre digital, du flex sensor et du capteur à base de graphite sur un écran OLED et notre appli bluetooth et sur le serial monitor */ // Description globale des fonctionnalités du code
/* Pour réaliser ce code nous nous sommmes inspiré du code réalisé précédement par Maël LE BLANC, Daniel PERALES RIOS. */ // Mention des étudiants dont le travail a servi de base
/* Lien de leur Github : https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-LE-BLANC--PERALES-RIOS */ // Lien vers le projet de référence

#include <Adafruit_SSD1306.h> // On importe la librairie pour piloter l'écran OLED
#include <SoftwareSerial.h> // On utilise cette librairie pour créer un port série logiciel pour le Bluetooth
#include <SPI.h> // Inclusion de la librairie pour la communication SPI avec le potentiomètre

////////////////////////////// --- INITIALISATION DES VARIABLES --- ////////////////////////////// // Section de déclaration des variables

// --- Paramètres matériels de l'arduino --- // Définition des branchements physiques
#define encoderPinA 2 // Branchement de la broche A de l'encodeur rotatif sur le pin 2
#define encoderPinB 4 // Branchement de la broche B de l'encodeur rotatif sur le pin 4
#define Switch 5 // Le bouton de l'encodeur est relié au pin 5
#define nombreDePixelsEnLargeur 128 // L'écran fait 128 pixels de large
#define nombreDePixelsEnHauteur 64 // L'écran fait 64 pixels de haut
#define brocheResetOLED -1 // On n'utilise pas de pin de reset pour l'OLED
#define adresseI2CecranOLED 0x3C // Adresse I2C standard pour ce type d'écran

#define MCP_WRITE 0b00010001 // Commande pour écrire dans le registre du MCP41xxx
#define rxPin 0 // Définition de la pin de réception série (attention, déjà utilisée par l'USB)
#define txPin 1 // Définition de la pin de transmission série
#define baudrate 9600 // Vitesse de communication standard à 9600 bauds

Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED); // Création de l'objet pour l'écran
SoftwareSerial mySerial(rxPin, txPin); // Création du port série logiciel pour le module Bluetooth
char data_[16]; // Buffer pour stocker des chaînes de caractères si besoin

// --- GRAPHITE SENSOR --- // Variables liées au capteur graphite
float range = pow(10,-6); // Facteur d'échelle pour convertir en MegaOhms
float Vadc = 0.0; // Variable pour stocker la tension mesurée par l'ADC
float R1 = 100000.0; // Valeur de la résistance R1 du montage
float R3 = 100000.0; // Valeur de la résistance R3
float R5 = 10000.0; // Valeur de la résistance R5
float Rc = 0.0; // Variable de stockage pour la résistance calculée du capteur
// Résistance finale mesurée du capteur graphite // Commentaire indicatif sur Rc

// --- FLEX SENSOR --- // Variables liées au capteur de flexion
const int flexPin = A1; // Le capteur flex est branché sur l'entrée analogique A1
const float VCC = 5.0; // Tension d'alimentation du circuit
const float R_DIV = 29000.0; // Valeur de la résistance fixe du pont diviseur
const float flatResistance = 37000.0; // Résistance mesurée quand le capteur est plat
const float bendResistance = 110000.0; // Résistance mesurée quand le capteur est plié à 90 degrés
float Rflex = 0.0; // Variable pour la résistance calculée du flex sensor
float angle = 0.0; // Variable pour stocker l'angle de flexion calculé

// --- VARIABLES DE CONTROLE --- // Variables pour la navigation et l'encodeur
int lastSwitchState = HIGH; // État précédent du bouton pour détecter l'appui
long lastEncoderValue = 0; // Dernière valeur connue de l'encodeur pour le menu
long lastPotValue = 0; // Dernière valeur de l'encodeur pour le potentiomètre
long lastDebounceTime = 0; // Timer pour filtrer les rebonds du bouton
long debounceDelay = 40; // Délai de 40ms pour le debounce
bool button_pressed = false; // Flag pour savoir si le bouton est activé
volatile long encoderValue = 0; // Valeur de l'encodeur pour le menu principal (volatile car modifiée par interruption)
volatile long encoder_for_pot = 0; // Valeur de l'encodeur dédiée au réglage du potentiomètre

bool mainMenu = true; // Booléen pour savoir si on affiche le menu ou un capteur
const char* menuItems[] = {"Potentiometer", "Flex Sensor", "Graphite Sensor"}; // Tableau contenant les noms des options du menu
int selectMenu = 0; // Index de l'élément actuellement sélectionné
// --- DIGITAL POTENTIOMETER --- // Configuration du potentiomètre numérique
const int ssMCPin = 10; // Pin Slave Select (Chip Select) pour le bus SPI sur le pin 10
float rWiper = 125.0; // Résistance interne du curseur (wiper) du MCP
float max_value_pot = 50000.0; // Valeur maximale de la résistance du potentiomètre (50k)
int valuePot = 10; // Valeur initiale du réglage (0 à 255)
float R2 = (float(valuePot)/256.0)*max_value_pot; // Calcul théorique initial de R2
/////////////////////////////////// --- SETUP --- /////////////////////////////////// // Initialisation au démarrage
void setup() { // Fonction setup exécutée une seule fois
    Serial.begin(9600); // On lance la communication série pour le débuggage
// Définition du Bus SPI // Commentaire sur le SPI
    pinMode(ssMCPin, OUTPUT); // On configure la pin CS du potentiomètre en sortie
    digitalWrite(ssMCPin, HIGH); // On la met au repos (état haut)

    // Initialisation de l'écran // Paramétrage de l'OLED
    delay(1000); // Petite pause de sécurité pour le hardware
// Stabilisation de la tension // Commentaire sur la tension
    if (!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED)) { // On tente de démarrer l'écran
       Serial.println(F("OLED introuvable")); // Si ça échoue, on l'affiche sur le PC
while (1); // On bloque le programme ici si l'écran ne répond pas
    }
    
    ecranOLED.clearDisplay(); // On efface ce qu'il y a sur l'écran
    ecranOLED.setTextColor(SSD1306_WHITE); // On configure le texte en blanc
    ecranOLED.setCursor(20, 25); // On place le curseur au milieu
    ecranOLED.print(F("INITIALISATION...")); // On affiche un message d'attente
    ecranOLED.display(); // On envoie l'image à l'écran
// Initialisation des autres composants connectés à l'Arduino // Setup des entrées/sorties
    pinMode(encoderPinA, INPUT_PULLUP); // Pin A en entrée avec résistance de pull-up interne
    pinMode(encoderPinB, INPUT_PULLUP); // Pin B en entrée avec résistance de pull-up interne
    pinMode(Switch, INPUT_PULLUP); // Bouton en entrée avec résistance de pull-up interne
    
    SPI.begin(); // On démarre le bus SPI
    mySerial.begin(baudrate); // On démarre la communication Bluetooth
attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, RISING); // On lie l'interruption sur le front montant de la pin A
    
    delay(1000); // Attente avant de lancer la boucle principale
    Serial.println(F("System Ready")); // Confirmation dans le moniteur série
}

//////////////////////////////////// --- LOOP --- //////////////////////////////////// // Boucle principale
void loop() { // Fonction qui tourne en boucle
    if (mainMenu) { // Si on est dans le menu principal
        displayMenu(); // On affiche les choix du menu
updateSelectedItem(encoderValue, 3); // On met à jour l'index sélectionné selon l'encodeur
        switchButton(); // On vérifie si on a appuyé sur le bouton pour entrer dans un sous-menu
    } else { // Sinon, on est dans l'affichage d'un capteur spécifique
        handleMenuItemSelection(selectMenu); // On affiche les données du capteur choisi
        switchButton(); // On vérifie si on appuie pour revenir au menu
    }
    delay(50); // Petite pause pour ne pas surcharger le processeur
// Plus réactif // Commentaire sur la réactivité
}

////////////////////////////////// --- FONCTIONS DES COMPOSANTS --- ////////////////////////////////// // Détail des fonctions

// --- FONCTIONS AFFICHAGE --- // Gestion de ce qui apparaît sur l'OLED
void displayMenu() { // Fonction pour dessiner le menu
    ecranOLED.clearDisplay(); // On vide l'écran
for (int i = 0; i < 3; i++) { // On boucle sur les 3 éléments du menu
        if (i == selectMenu) { // Si c'est l'élément qu'on a sélectionné
            ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // On inverse les couleurs pour mettre en évidence
// Surlignage // Commentaire sur l'effet visuel
        } else { // Sinon
            ecranOLED.setTextColor(SSD1306_WHITE); // On écrit normalement en blanc sur fond noir
} // Fin du test de sélection
        ecranOLED.setCursor(0, i * 15); // On décale chaque ligne de 15 pixels vers le bas
        ecranOLED.print(menuItems[i]); // On écrit le nom de l'item
    }
    ecranOLED.display(); // On rafraîchit l'affichage physique
} // Fin de la fonction menu

void displayPotentiometer(int val) { // Fonction pour afficher et régler le potentiomètre
    ecranOLED.clearDisplay(); // On efface l'écran
    ecranOLED.setTextColor(SSD1306_WHITE); // Texte en blanc
    SPIWrite(MCP_WRITE, val, ssMCPin); // On envoie la nouvelle valeur au MCP via SPI
    
    ecranOLED.setCursor(0, 0); // On se place en haut
    ecranOLED.print(F("DigiPot Setting")); // Titre de la page
    ecranOLED.setCursor(0, 20); // Un peu plus bas
    ecranOLED.print(F("R2: ")); // Label pour la résistance
ecranOLED.print(R2); ecranOLED.print(F(" Ohm")); // Affichage de la valeur calculée de R2
    
    ecranOLED.setCursor(0, 50); // En bas de l'écran
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // On surligne le bouton retour
    ecranOLED.print(F("< BACK")); // Indication pour revenir au menu
    ecranOLED.display(); // Mise à jour de l'écran
}

void displayFlexSensor() { // Fonction pour l'affichage du capteur de flexion
    ecranOLED.clearDisplay(); // Nettoyage de l'écran
    ecranOLED.setTextColor(SSD1306_WHITE); // Couleur blanche
    flexMeasure(); // On appelle la fonction de mesure
ecranOLED.setCursor(0, 0); // Positionnement du texte
    ecranOLED.print(F("Flex Res: ")); ecranOLED.print(Rflex); // Affichage de la résistance du capteur
    ecranOLED.setCursor(0, 20); // Ligne suivante
    ecranOLED.print(F("Angle: ")); ecranOLED


/////////////////////////////////// --- MESURES --- ///////////////////////////////////


void graphiteMeasure() { // Calcul de la résistance pour le graphite
    float V = analogRead(A2); // Lecture de la valeur brute sur le pin A2
    Vadc = V * 5.0 / 1023.0; // Conversion de la valeur ADC (0-1023) en tension (0-5V) [cite: 29]
    // Formule corrigée avec flottants // On applique la formule physique du montage
    Rc = ((R1 * (1.0 + R3 / R2) * (VCC / Vadc)) - R1 - R5) * range; // Calcul final de la résistance Rc
} // Fin de la mesure graphite [cite: 30]

void flexMeasure() { // Calcul pour le capteur de flexion
    int ADCflex = analogRead(flexPin); // Lecture analogique sur A1
    float Vflex = (ADCflex / 1023.0) * VCC; // Conversion en tension
    Rflex = R_DIV * (VCC / Vflex - 1.0); // Calcul de la résistance du capteur via le pont diviseur [cite: 31]
    angle = map(Rflex, flatResistance, bendResistance, 0, 90.0); // Conversion de la résistance en angle (0 à 90°)
} // Fin de la mesure flex [cite: 32]

void updateEncoder() { // Fonction appelée par l'interruption à chaque cran de l'encodeur
    if (mainMenu) { // Si on est dans le menu
        if (digitalRead(encoderPinB) == LOW) encoderValue++; // On incrémente si on tourne dans un sens [cite: 33]
    else encoderValue--; // On décrémente sinon [cite: 33]
    } else if (selectMenu == 0) { // Si on règle le potentiomètre digital
        if (digitalRead(encoderPinB) == LOW) encoder_for_pot++; // On change la valeur du potard vers le haut [cite: 34]
    else encoder_for_pot--; // On la change vers le bas [cite: 34]
    }
}

void updateSelectedItem(int encVal, int NumberOfItems) { // Gestion de la sélection cyclique
    int val = abs(encVal % NumberOfItems); // On utilise un modulo pour rester dans les bornes du tableau
    if (val != lastEncoderValue) { // Si la valeur a changé [cite: 35]
        selectMenu = val; // On met à jour l'index sélectionné
        lastEncoderValue = val; // On mémorise la nouvelle position
    } // Fin du changement [cite: 36]
}

void updatePotentiometerValue() { // Mise à jour de la valeur numérique du potentiomètre
    if (encoder_for_pot != lastPotValue) { // Si l'encodeur a bougé
        valuePot = constrain(valuePot + (encoder_for_pot > lastPotValue ? 5 : -5), 0, 255); // On ajuste par pas de 5 en restant entre 0 et 255
    lastPotValue = encoder_for_pot; // On stocke la position actuelle pour la suite [cite: 37]
    }
}

void SPIWrite(uint8_t cmd, uint8_t res, uint8_t ssPin) { // Envoi effectif des données au MCP41xxx
    updatePotentiometerValue(); // On calcule d'abord la valeur
    R2 = ((max_value_pot * res) / 256.0) + rWiper; // On calcule la résistance réelle correspondante pour l'affichage [cite: 38]
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0)); // On démarre la transaction SPI à 1MHz
    digitalWrite(ssPin, LOW); // On active le composant (CS bas)
    SPI.transfer(cmd); // On envoie l'octet de commande
    SPI.transfer(res); // On envoie la valeur de résistance (0-255)
    digitalWrite(ssPin, HIGH); // On désactive le composant (CS haut)
    SPI.endTransaction(); // On termine la transaction SPI
} // Fin de l'écriture SPI [cite: 39]

void switchButton() { // Gestion de l'appui sur le bouton avec filtrage
    if (digitalRead(Switch) == LOW && (millis() - lastDebounceTime) >= debounceDelay) { // Si bouton pressé et délai de debounce passé
        lastDebounceTime = millis(); // On reset le timer de debounce
    mainMenu = !mainMenu; // On bascule entre l'état "Menu" et l'état "Capteur" [cite: 40]
        while(digitalRead(Switch) == LOW); // On bloque tant que le bouton est maintenu [cite: 41]
    // Attendre relâchement // Commentaire sur l'attente [cite: 41]
    }
}

void sendBluetooth() { // Envoi des données vers l'application
    /* * On prépare la ligne au format : Potentiometre|Flex|Graphite */ // Explication du format de trame
    /* On utilise directement les variables globales calculées dans les autres fonctions */ // Précision sur les variables

    // 1. On vide le moniteur série PC pour la clarté // Info debug
    //Serial.print(F("Envoi BT: ")); // Ligne commentée pour le debug
    // 2. Envoi de la valeur R2 (Potentiomètre) // Envoi première donnée [cite: 42]
    mySerial.print(R2); // Envoi de R2 au module Bluetooth
    mySerial.print("|"); // Séparateur
    Serial.print(R2); Serial.print("|"); // Affichage identique sur le moniteur série PC
    // 3. Envoi de la valeur Rflex (Flex) // Envoi deuxième donnée [cite: 43]
    mySerial.print(Rflex); // Envoi de la résistance flex
    mySerial.print("|"); // Séparateur
    Serial.print(Rflex); Serial.print("|"); // Debug PC
    // 4. Envoi de la valeur Rc (Graphite) + FIN DE LIGNE (println) // Envoi dernière donnée [cite: 44]
    mySerial.println(Rc); // Envoi final avec retour à la ligne pour le Bluetooth
    Serial.println(Rc); // Retour à la ligne pour le moniteur série
} // Fin de la transmission [cite: 45]

void handleMenuItemSelection(int sel) { // Switch pour choisir quel écran afficher
    switch (sel) { // On teste l'index sélectionné
        case 0: displayPotentiometer(valuePot); // Si 0, on va sur le potentiomètre
    break; // On sort du switch [cite: 46]
        case 1: displayFlexSensor(); break; // Si 1, on va sur le flex sensor
        case 2: displayGraphiteSensor(); break; // Si 2, on va sur le graphite sensor
    }
}