//////////////////////////////////////////// LIBRAIRIES UTILISEES ////////////////////////////////////////////



#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SoftwareSerial.h>



//////////////////////////////////////////// DECLARATIONS ET DEFINITIONS DIVERSES ////////////////////////////////////////////



// Définition des pins analogiques pour les capteurs
#define PIN_GRAPH_SENSOR A0 // Capteur graphite sur l'entrée analogique A0
#define PIN_FLEX_SENSOR A1 // Flex sensor sur l'entrée A1

// Définition des pins numériques pour l'encodeur rotatif
#define CLK  2  
#define DT  4  
#define SW  5 

// Définition des broches RXD et TXD du module Bluetooth HC-05
#define BT_TX 9 // Broche TXD du module Bluetooth connectée à la broche 9 d'Arduino
#define BT_RX 8 // Broche RXD du module Bluetooth connectée à la broche 8 d'Arduino

// Déclaration d'un objet de type SoftwareSerial pour la communication avec le module Bluetooth
SoftwareSerial bluetooth(BT_TX, BT_RX);

// Déclaration de l'écran OLED avec la bibliothèque AdaFruit
#define SCREEN_WIDTH 128    // Taille horizontale de l'écran OLED  
#define SCREEN_HEIGHT 64    // Taille verticale 
#define OLED_RESET    -1    // Reset de l'écran est le même que celui de l'Arduino Uno
#define SCREEN_ADDRESS 0x3C // Adresse I2C de l'écran

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Déclaration liées aux menus de l'écran OLED
int selectedItem = 0;
bool inSubMenu = false;
const int numItems = 2;
const char *menuItems[] = {"Afficher les mesures", "Modifier resistance"};

// Déclaration pour le potentiomètre digital
const byte csPin           = 10;      
const int  maxPositions    = 256;     
const long rAB             = 50000.0;    
const byte rWiper          = 125;     
const byte pot0            = 0x11;    // pot0 addr
const byte pot0Shutdown    = 0x21;    // pot0 shutdown
int pos_res_variable = 5; // Fixe la résistance initiale du potentiomètre digital

// Déclaration pour le flex sensor et le capteur graphite
const float VCC = 5.0;
const float flexR_DIV = 39000.0;
const float flexflatResistance = 25000.0;
const float flexbendResistance = 100000.0;
float tension_graphite = 0;
float tension_flex = 0;
long resistance_variable = 0;
float resistance_graphite = 0;
long resistance_flex = 0;
char resultGraphite[15];
char buffer[15];
char resultFlex[15];
char buffer_flex[15];

// Déclaration pour l'encodeur rotatif
volatile unsigned int lastStateSW; // variable qui enregistre l'état du bouton à la boucle précédente



//////////////////////////////////////////// SETUP ////////////////////////////////////////////



void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  
  // Initialisation des pins 
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  pinMode(PIN_GRAPH_SENSOR, INPUT);
  pinMode(PIN_FLEX_SENSOR, INPUT);
    
  lastStateSW = digitalRead(SW); // Vérifie l'état du bouton au lancement du programme

  attachInterrupt(digitalPinToInterrupt(CLK), doEncoder, CHANGE); // Définir les interruptions sur l'encodeur rotatif
 
  // Initialisation de l'écran OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Echec de l'allocation SSD1306"));
    for(;;);
  }
  display.setTextSize(1);
  display.display();
  delay(2000);
  display.clearDisplay();

  // Initialisation de la communication avec le potentiomètre digital
  digitalWrite(csPin, HIGH);        
  pinMode(csPin, OUTPUT);           
  SPI.begin();
}



//////////////////////////////////////////// LOOP ////////////////////////////////////////////



void loop() {
  // Lecture de l'état du bouton poussoir de l'encodeur pour la navigation dans les sous-menus
  int stateSW = digitalRead(SW);
  if (stateSW != lastStateSW && stateSW == LOW) {
    Serial.println(F("Clic!"));
    inSubMenu = !inSubMenu;
  }
  lastStateSW = stateSW;

  setPotWiper(pot0, pos_res_variable); // Modifie la résistance du potentiomètre

  // Mesures des capteurs et de la résistance variable
  tension_graphite = analogRead(PIN_GRAPH_SENSOR) * VCC / 1023.0;
  resistance_variable = ((rAB * pos_res_variable) / maxPositions) + rWiper;
  resistance_graphite = ((1 + (100000.0 / resistance_variable)) * 100000.0 * (VCC / tension_graphite) - 110000.0) / 1000000.0;
  
  tension_flex = analogRead(PIN_FLEX_SENSOR) * VCC / 1023.0;
  resistance_flex = flexR_DIV * (VCC / tension_flex - 1);

  // Gestion des différents menus sur l'écran OLED
  display.clearDisplay();

  if (!inSubMenu) {
    for (int i = 0; i < numItems; ++i) {
      if (i == selectedItem) {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      } else {
        display.setTextColor(SSD1306_WHITE);
      }
      display.setCursor(0, i * 10);
      display.println(menuItems[i]);
    }
  } else {
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    switch (selectedItem) {
      case 0:
        display.println(F("Tension graphite :  "));
        display.print(tension_graphite);
        display.println(F(" V"));
        display.println();
        display.println(F("Resistance graphite :"));
        display.print(resistance_graphite);
        display.println(F(" MOhms"));
        display.println();
        display.println(F("Resistance flex : "));
        display.print(resistance_flex);
        display.println(F(" Ohms"));
        break;

      case 1:             
        display.println(F("Resistance variable :"));
        display.print(resistance_variable);
        display.println(F(" Ohms"));
        display.println();
        display.println(F("Tension graphite :  "));
        display.print(tension_graphite);
        display.println(F(" V"));
        break;

      default:
        display.println(F("Option inconnue"));
        break;
    }
  }

  display.display();

  // Bluetooth
  dtostrf(tension_graphite, 4, 2, buffer); // Transforme la mesure float en une chaîne de caractères    
  sprintf(resultGraphite, "g%s\n", buffer); // Remplit le tableau resultGraphite de la mesure au format g"mesure"\n pour rendre compatible les données avec l'application
  bluetooth.write(resultGraphite); // Envoie le contenu du tableau vers le module Bluetooth
  delay(100);    
  
  dtostrf(tension_flex, 4, 2, buffer_flex);
  sprintf(resultFlex, "f%s\n", buffer_flex);
  bluetooth.write(resultFlex);

  if (bluetooth.available()) {
    // Lit les données reçues depuis le module Bluetooth
    char receivedChar = bluetooth.read();
    // Affiche les données reçues sur le moniteur série
    Serial.print(receivedChar);
  }

  delay(100);
}



//////////////////////////////////////////// FONCTIONS UTILISEES ////////////////////////////////////////////



// Fonction lors de l'interrupt de l'encodeur rotatif
void doEncoder() {
  // Cas du menu principal
  if (!inSubMenu) { 
    if (digitalRead(DT) == HIGH) {
      selectedItem = (selectedItem == 0) ? numItems - 1 : selectedItem - 1; // [Sens horaire] -> du haut vers le bas, revient en haut une fois la fin de la liste atteinte
    } else {
      selectedItem = (selectedItem == numItems - 1) ? 0 : selectedItem + 1; // [Sens anti-horaire] -> du bas vers le haut
    }
  } 
  // Cas du sous-menu de choix du gain
  else {
    if (selectedItem == 1) {
      if (digitalRead(DT) == HIGH) {      
        pos_res_variable++;
        if (pos_res_variable >= 255) {
          pos_res_variable = 255;
        }
      } else {
        pos_res_variable--;
        if (pos_res_variable <= 1) {
          pos_res_variable = 1;
        }
      }
    }
  }
}

// Fonction pour régler la valeur du potentiomètre
void setPotWiper(byte address, int value) {
  digitalWrite(csPin, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(csPin, HIGH);
}
