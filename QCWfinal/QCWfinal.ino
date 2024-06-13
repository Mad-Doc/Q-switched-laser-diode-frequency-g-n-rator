#include <Arduino.h>
#include <U8x8lib.h>
#include <Encoder.h>

// Définition des pins
#define pwmPin B00000010 // Définit la pin 9 pour la sortie PWM
#define encoderPinA 2    // Pin A de l'encodeur rotatif
#define encoderPinB 3    // Pin B de l'encodeur rotatif
#define encoderButtonPin 4 // Pin du bouton de l'encodeur rotatif
#define interL 5 // pin d'interlock

U8X8_SH1106_128X64_NONAME_HW_I2C display(U8X8_PIN_NONE);
Encoder encoder(encoderPinA, encoderPinB); // Initialise l'encodeur rotatif
bool buttonState = false; // État actuel du bouton de l'encodeur
//bool State = false; // État actuel de l'entrée activation externe

// Variables pour contrôler le PWM
volatile unsigned long pulseWidth = 300;  // Largeur d'impulsion en microsecondes (300 par défaut)
volatile unsigned long frequence = 100;    // Fréquence en Hz (1Hz par défaut)
volatile unsigned long period = 100000; // Période en microsecondes
volatile unsigned long pwComp = 0.15;
volatile unsigned long perComp = 0.6;

// Duty cycle maximum (20 %)
const float dutyCycleMax = 0.20;

// État du signal
volatile bool signalOn = false;
volatile bool lock = false;

void setup() {
  display.begin();
  
  //display.clearDisplay();  
  display.setPowerSave(0);
   display.setFlipMode(0);

  DDRB = DDRB | pwmPin; 

  pinMode(encoderButtonPin, INPUT_PULLUP); // Bouton de l'encodeur rotatif
  pinMode(encoderPinA, INPUT_PULLUP);      // Pin A de l'encodeur rotatif
  pinMode(encoderPinB, INPUT_PULLUP);      // Pin B de l'encodeur rotatif
  pinMode(interL, INPUT_PULLUP); //entrée interlock
  
  Serial.begin(9600);
  while (!Serial) continue;
  Serial.println("Prêt à recevoir vos commandes.");
  Serial.println(" ");
  Serial.println("envoyer laser help pour listé les commandes!");
  Serial.println(" ");
  delay(1000);
  lcdBoot();
  delay(1000);
  updateDisplay();
}
void lcdBoot() {
   
   display.setFont(u8x8_font_8x13B_1x2_f);
   display.clear();
   display.setCursor(0,0);
   display.println("  pulsed laser ");
   display.println("   controller  ");
   display.println("---------------");
   display.setFont( u8x8_font_pressstart2p_f);
   display.println("rev 1.5        ");
   display.println("        by Doc ");
   delay(1500);
}

void updateDisplay() {

  display.setFont( u8x8_font_pressstart2p_f);
  display.clear();
  display.setCursor(1,0);
  display.print(F("Emission:")); 
  display.println(signalOn ? "ON" : "OFF");
  display.setCursor(1,1);
  display.print(F("Freq: "));
  display.print(frequence);
  display.print(F("Hz"));
  display.setCursor(1,2);
  display.print(F("Pulse: "));
  display.print(pulseWidth);
  display.print(F("us"));
  display.setCursor(1,3);
  display.print(F("Duty:"));
  display.print((float)pulseWidth / (float)period * 100.0, 2);  // affiche avec 2 décimales
  display.print(F("%"));
  display.setCursor(1,4);
  display.print(F("Interlock: "));
  display.print(lock ? "OK" : "Not");
  
}
void button(){
   // Lecture de l'encodeur rotatif
long newPosition = encoder.read();
static long oldPosition = newPosition;
if (newPosition != oldPosition) {
  if (newPosition > oldPosition && frequence < 20000) {
    frequence += 1; // Augmentation de la fréquence de 1 Hz
  } else if (newPosition < oldPosition && frequence > 0) {
    frequence -= 1; // Diminution de la fréquence de 1 Hz
  }
  period = 1000000 / max(min(frequence, 20000UL), 1UL); // Limite à 20000 Hz
  oldPosition = newPosition;
  pulseWidth = min((unsigned long)(period * dutyCycleMax), 300UL);
  pulseWidth = max(pulseWidth, 1UL);
  updateDisplay();
}

  // Lecture de l'état du bouton de l'encodeur rotatif
  bool newButtonState = digitalRead(encoderButtonPin);
  if (newButtonState != buttonState) {
    if (newButtonState == LOW) {
      signalOn = !signalOn; // Inversion de l'état du signal
    }
    buttonState = newButtonState;
    delay(200); // Anti-rebond
    updateDisplay();
  }
}
void external(){
    bool extState = digitalRead(interL);
  if (extState = LOW) {
    lock = !lock; // Inversion de l'état du signal
    //signalOn = false;
    delay(200); // Anti-rebond
    //updateDisplay();
  }else{
   signalOn = false;
   delay(200);
   //updateDisplay(); 
  }
  //updateDisplay();
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    handleLaserCommand(command);
    updateDisplay();
  } else {
   button();
   //external(); //entrée interlock
}

  if (signalOn) {
    //add calcul de correction variable 0.15ms at 300ms , et 0.6ms at 100hz !   
     //pwComp = 0.15;
     //perComp = 0.6;
    
    if (period <= 16382 ) {
      PORTB |= pwmPin;
      delayMicroseconds(pulseWidth);
      PORTB &= ~pwmPin;
      delayMicroseconds(period - pulseWidth);
    } else if (period >= 16382 ) {
      PORTB |= pwmPin;
      delayMicroseconds(pulseWidth);
      PORTB &= ~pwmPin;
      delay(period / 1000 );
    }
  }
}

void handleLaserCommand(String command) {
  command.trim();
  if (command.startsWith("on")) {
    signalOn = true;
    Serial.println("Signal activé");
  } else if (command.startsWith("off")) {
    signalOn = false;
    Serial.println("Signal désactivé");

  } else if (command.startsWith("frequence ")) {
    int spaceIndex1 = command.indexOf(' ');
    if (spaceIndex1 != -1) {
      String valueStr1 = command.substring(spaceIndex1 + 1);
    unsigned long newfrequence = valueStr1.toInt();
      if (newfrequence >= 1 && newfrequence <= 20000) { //665 hz a 300µS on . 20000 hz a 10µs on pour 20% de dutycycle max
        frequence = newfrequence;
        period = 1000000 / frequence;
        pulseWidth = min((unsigned long)(period * dutyCycleMax), 300UL);
        pulseWidth = max(pulseWidth, 1UL);
        Serial.print("Fréquence mise à ");
        Serial.print(frequence);
        Serial.println(" hz");
        Serial.print("Duty Cycle: ");
        Serial.print((float)pulseWidth / (float)period * 100.0, 2);
        Serial.println("%");
    } else {
        Serial.println("La valeur de la fréquence dépasse la consigne!");
    }
    } 
  } else if (command.startsWith("pulse ")) {
    int spaceIndex = command.indexOf(' ');
    if (spaceIndex != -1) {
      String valueStr = command.substring(spaceIndex + 1);
     unsigned long newPulseWidth = valueStr.toInt();
      if (newPulseWidth >= 1 && newPulseWidth <= 300) {
        pulseWidth = newPulseWidth;
        unsigned long newPeriod = (pulseWidth / dutyCycleMax);
        frequence = 1000000 / newPeriod;
        period = newPeriod;
        Serial.print("Largeur d'impulsion mise à ");
        Serial.print(pulseWidth);
        Serial.println(" ms");
        Serial.print("Duty Cycle: ");
        Serial.print((float)pulseWidth / (float)period * 100.0, 2);
        Serial.println("%");
    } else {
        Serial.println("La valeur de la largeur d'impulsion dépasse la consigne!");
    }
   } 
  } else if (command.startsWith("help")) {
    Serial.println("Liste des commandes :");
    Serial.println("on = sortie pulse active");
    Serial.println("");
    Serial.println("off = sortie pulse inactive");
    Serial.println(" ");
    Serial.println("frequence [Hz] = fréquence de répétition du pulse en Hz, 0 à 600 Hz max !");
    Serial.println("commande en fréquence , la largeur de pulse s'ajuste automatiquement !");
    Serial.println("");
    Serial.println("pulse [ms] = largeur du pulse en ms, 0.10µs à 300µs max !");
    Serial.println("QCW pulser code version 2.0 by Doc");
  } else {
    Serial.println("Commande incorrecte! Taper help pour les commandes disponible.");
  }
}
