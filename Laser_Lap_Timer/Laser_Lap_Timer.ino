#include <LiquidCrystal.h>

/*

VERSION 0.3 

-Added Green LED For Best Round

*/


// PIN CONFIGURATION
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int laserPin = 13;
int buttonPin1 = 7;
int buttonPin2 = 8;
int speakerPin = 9;
int ledPin1 = 0; // red 
int ledPin2 = 1; // red
int ledPin3 = 10; // green


/*

 States // The central state the timer is in
 
 0 = pause;
 1 = countdown;
 2 = running;
 
*/


int state = 0;
boolean displayLastRound = true;
boolean running = false;

int lightLevel = 0;
long lastHit = 0; //time of the last hit in the barrier (absolut)
boolean lock; // locks the light barrier while a car is passing. The look is removed as soon the car left the lightbeam and the roundTreshold time has passed

long lastRound = 0; // time of the lastround (absolut)
long bestRound = 0; // time of the bestround (relative)
int roundNr = 0;


int laserTreshold = 10;
int roundTreshold = 300; // minimum time that has to pass before a new round is counted

// Absolut time for pressing buttons
long lastPress1 = 0;
long lastPress2 = 0;


// LED Timer

long timeLEDGreen = 0;

void setup() {
  pinMode(laserPin, OUTPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(speakerPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);


  lcd.setCursor(0, 0);
  lcd.print("Arduino");
  lcd.setCursor(0, 1);
  lcd.print("Laser Lap Timer  ");
  
  delay(1500);
  
  lcd.setCursor(0, 0);
  lcd.print("Christoph &");
  lcd.setCursor(0, 1);
  lcd.print("Peter Halang    ");


  //Serial.begin(9600);
  delay(1500);

}

void loop() {

  if(!digitalRead(buttonPin1)){

    button1Pressed();

  };

  if(!digitalRead(buttonPin2)){

    button2Pressed();

  };



  switch (state){
  case 0: 
    statePaused(); 
    break;
  case 1: 
    stateCountdown(); 
    break;
  case 2: 
    stateRunning(); 
    break;
  }
}

void didFinishRound(){
  if(roundNr > 0)lastRound = millis()-lastHit;
  

  if (bestRound == 0){
    bestRound = lastRound;
  }
  if (lastRound < bestRound){
    bestRound = lastRound;
    timeLEDGreen = millis()+1000;
  }
  lastHit = millis();
  lock = true;  
  ++roundNr;
  displayLastRound = true;  

}

void statePaused(){
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  digitalWrite(laserPin,LOW);
  lcd.setCursor(0, 0);
  lcd.print("Press right   ");
  lcd.setCursor(0, 1);
  lcd.print("button to start");

}

void stateRunning(){

  //switch laser on

  digitalWrite(laserPin,HIGH);


  //mesasure light

  lightLevel = analogRead(0);
  lightLevel = map(lightLevel, 0, 900, 0, 255);
  lightLevel = constrain(lightLevel, 0, 255);

  //Serial.println(lightLevel);
  // react to light

  if(lightLevel < laserTreshold){
    //digitalWrite(ledPin,LOW);
    lock = false;
  }
  else{
    // digitalWrite(ledPin,HIGH);
    if (millis()-lastHit > roundTreshold && lock == false){
      didFinishRound();
    }
  }

  // We have'nt finished the first round
  if(roundNr == 0){

    lcd.setCursor(0,0);
    lcd.print("Standby...      ");
    lcd.setCursor(0,1);
    lcd.print("                ");    

  }
  // Display the time and best / last round
  else{
    displaySecondLine();
    displayFirstLine();

  }
  
  // turn off all LED
  
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);

  if (timeLEDGreen > millis()){
      digitalWrite(ledPin3, HIGH);
  }
  
  else{
      digitalWrite(ledPin3, LOW);
  }
  
  



}

void stateCountdown(){

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Ready             ");
  digitalWrite(ledPin1, HIGH);

  playNote('c',300);

  delay(1000);

  lcd.setCursor(0, 0);
  lcd.print("Set             ");
  digitalWrite(ledPin2, HIGH);

  playNote('c',300);

  delay(1000);

  lcd.setCursor(0, 0);
  lcd.print("Go             ");
  digitalWrite(ledPin3, HIGH);
  digitalWrite(laserPin,HIGH);

  playNote('g',600);

  state = 2;

}



void displayFirstLine(){

  lcd.setCursor(0, 0);


  lcd.print("Rd");
  lcd.print(" ");
  lcd.print(roundNr);
  lcd.print(" ");
  lcd.print(millis()-lastHit);
  lcd.print("ms");

  lcd.print("                       ");



}

void displaySecondLine(){

  lcd.setCursor(0, 1);

  if(displayLastRound){



    lcd.print("Last ");
    lcd.print(lastRound);
    lcd.print("ms");
    lcd.print("                       ");


  }



  else{
    lcd.print("Best ");
    lcd.print(bestRound);
    lcd.print("ms");
    lcd.print("                       ");

  }



}

void button1Pressed(){
  if(millis()-lastPress1 > 800){
    lastPress1 = millis();
    displayLastRound = !displayLastRound;
  }

}

void button2Pressed(){
  if(millis()-lastPress2 > 800){
    lastPress2 = millis();
    if (state == 0){
      state = 1;
    }
    if (state == 2){
      digitalWrite(laserPin,LOW);
      roundNr = 0;
      bestRound = 0;
      lastRound = 0;
      state = 0;
    }    

  }
}

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}
void playNote(char note, int duration) {
  char names[] = { 
    'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'   };
  int tones[] = { 
    1915, 1700, 1519, 1432, 1275, 1136, 1014, 956   };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}





