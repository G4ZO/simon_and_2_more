#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define NUMPIXELS 24 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 50 // Time (in milliseconds) to pause between pixels

//buttons
#define yellow_B A0
#define green_B A1
#define red_B A2
#define blue_B A3
#define mode_B A4

#define beep 9

#define YellowHZ 600
#define GreenHZ 800
#define RedHZ 1000
#define BlueHZ 1200

#define SIZE 64
int ColorSequence[SIZE];
int simonDelay;




bool gameRUN = true;
byte mode = 0; // 0 - simon, 1 - led run, 2 - led

void setup() {
  pixels.begin();
  //pixels.setBrightness(50);
  uint16_t i, j;
  for(j=0; j<256*2; j++) { // 3 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(2);
  }

  pinMode(yellow_B,INPUT_PULLUP);
  pinMode(green_B,INPUT_PULLUP);
  pinMode(red_B,INPUT_PULLUP);
  pinMode(blue_B,INPUT_PULLUP);
  pinMode(mode_B,INPUT_PULLUP);
  pinMode(beep,OUTPUT);
  pinMode(A5,INPUT); //randomseed
}

void loop() {
  if (mode == 0)simon();
  if (mode == 1)ledRun();
  if (mode == 2)led();

}



//mode = 0
void simon(){
  simonSetup();
  delay(200);
  gameRUN = true  ;
  int index = 1; //1-64
  simonDelay = 500;

  
  while(gameRUN && mode == 0){ //game loop 
    DisplaySequence(index);
    for(int i = 0; i < index; i++){
      while(!CheckPressed() && analogRead(mode_B) > 500 && mode == 0){
        delay(5);
      }
      if(CheckButtons() != ColorSequence[i] && mode == 0){ //game over
        tone(beep,300);
        simonColorDisplay(ColorSequence[i], 100);
        simonColorDisplay(ColorSequence[i], 100);
        simonColorDisplay(ColorSequence[i], 100);
        noTone(beep);
        return;
      }
    }
    index++;
    delay(600);
    simonDelay = simonDelay - 30;
    if(simonDelay < 50)simonDelay = 50;
  }
}

void simonSetup(){
  randomSeed(analogRead(A5));
  for(int i = 0; i < 64; i++){ //Get a color sequence
    ColorSequence[i] = random(0,4);
  }
  
  //yeloh
  int R = 160;
  int G = 140;
  int B = 0;
  for(int i = NUMPIXELS; i > -1; i--) { // For each pixel...

    // pixels.Color(); //takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    switch(i){
      case 5:
        R = 0;
        G = 0;
        B = 200;
      break; 
      case 11:
        R = 200;
        G = 0;
        B = 0;
      break; 
      case 17:
        R = 0;
        G = 200;
        B = 0;
      break; 
    }
   pixels.setPixelColor(i, pixels.Color(R, G, B)); //RGB

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
    if(checkMode() == 6)break;
  }
  
  for(int i = NUMPIXELS; i > -1; i--) { // For each pixel...
   if(!(i%3 == 1)) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); //RGB
    pixels.show();   // Send the updated pixel colors to the hardware.
    if(checkMode() == 6)break;
   }
    delay(DELAYVAL); // Pause before next pass through loop
    if(checkMode() == 6)break;
  }
}

void DisplaySequence(int index){
  for(int i = 0; i < index; i++){
    switch(ColorSequence[i]){
      case 0:
        tone(beep,YellowHZ);
        simonColorDisplay(0, simonDelay);
      break;
      case 1:
        tone(beep,GreenHZ);
        simonColorDisplay(1, simonDelay);
      break;
      case 2:
        tone(beep,RedHZ);
        simonColorDisplay(2, simonDelay);
      break;
      case 3:
        tone(beep, BlueHZ);
        simonColorDisplay(3, simonDelay);
      break;
    }
    noTone(beep);
    delay(simonDelay/2);
    
    if(analogRead(mode_B) < 500){ //mode
      tone(beep, 1400);
      pixels.clear(); // Set all pixel colors to 'off'
      pixels.show();
      wait();
      gameRUN = false;
      mode++; 
      noTone(beep);
      return;
    }
  }
}

void simonColorDisplay(int color, int DELAY){ //0 - yellow, 1 - green, 2 - red, 3 blue
  switch(color){
    case 0: //yellow
      for(int i = 18; i <= 23; i++) { // For each pixel...
        pixels.setPixelColor(i, pixels.Color(160, 140, 0)); //RGB
        pixels.show();   // Send the updated pixel colors to the hardware.
      }
    break;
    case 1: //green
      for(int i = 12; i <= 17; i++) { // For each pixel...
        pixels.setPixelColor(i, pixels.Color(0, 200, 0)); //RGB
        pixels.show();   // Send the updated pixel colors to the hardware.
      }
    break;
    case 2: //red
    for(int i = 6; i <= 11; i++) { // For each pixel...
        pixels.setPixelColor(i, pixels.Color(200, 0, 0)); //RGB
        pixels.show();   // Send the updated pixel colors to the hardware.
      }
    break;
    case 3: //blue
    for(int i = 0; i <= 5; i++) { // For each pixel...
        pixels.setPixelColor(i, pixels.Color(0, 0, 200)); //RGB
        pixels.show();   // Send the updated pixel colors to the hardware.
      }
    break;
  }

   if(CheckPressed())wait();
   else delay(DELAY);
   
  //reset
  for(int i = NUMPIXELS; i > -1; i--) { // For each pixel...
    if(!(i%3 == 1)) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); //RGB
      pixels.show();   // Send the updated pixel colors to the hardware.
    }
  }
}

int CheckPressed(){ //return 1 when pressd
  if(analogRead(blue_B) < 500 || analogRead(red_B) < 500 || analogRead(green_B) < 500 || analogRead(yellow_B) < 500 || analogRead(mode_B) < 500){
    return 1;
  }
  else return 0;
}

int CheckButtons(){
  int PressedButton = 7;
  if(mode == 0 || mode == 2){
    if(analogRead(yellow_B) < 500){ //yellow
      if(mode == 0){
        tone(beep, YellowHZ);
        simonColorDisplay(0,100);
      }
      PressedButton = 0;
    }
    else if(analogRead(green_B) < 500){ //green
      if(mode == 0){
        tone(beep, GreenHZ);
        simonColorDisplay(1,100);
      }
      PressedButton = 1;
    }
    else if(analogRead(red_B) < 500){ //red
      if(mode == 0){
        tone(beep, RedHZ);
        simonColorDisplay(2,100);
      }
      PressedButton = 2;
    }
    else if(analogRead(blue_B) < 500){ //blue
      if(mode == 0){
        tone(beep, BlueHZ);
        simonColorDisplay(3,100);
      }
      PressedButton = 3;
    }
  }
   if(analogRead(mode_B) < 500){ //mode
      tone(beep, 1400);
      pixels.clear(); // Set all pixel colors to 'off'
      pixels.show();
      wait();
      gameRUN = false;
      mode++; 
      if(mode == 3)mode = 0;
      PressedButton = 6;
   }
  noTone(beep);
  return(PressedButton);
}

void wait(){
  delay(20);
  while(analogRead(blue_B) < 500 || analogRead(red_B) < 500 || analogRead(green_B) < 500 || analogRead(yellow_B) < 500 || analogRead(mode_B) < 500){
    delay(5);
  }
  delay(20);
}

int checkMode(){
  if(analogRead(mode_B) < 500){ //mode
      tone(beep, 1400);
      pixels.clear(); // Set all pixel colors to 'off'
      pixels.show();
      wait();
      gameRUN = false;
      mode++;
      if(mode == 3)mode = 0;
      noTone(beep);
      return 6;
   }
  
}



//mode = 1
int pix;
int light;
int SecondPixel;
int pixelSequence[SIZE];
int ledDelay;


void ledRun(){  
  int index = 0;
  ledDelay = 280;
  for(int i = 0; i < 64; i++){ //Get a color sequence
    pixelSequence[i] = random(0,24);
  }
  
  gameRUN = true;
  while(gameRUN){  //game loop
    if(CheckButtons() == 6) return;
    for(int i = 0; i < NUMPIXELS; i++) { // For each pixel...
      pix = i;
      light = 250;

      //fade
       fade(index);

      //delay(ledDelay); // Pause before next pass through loop
      unsigned long TIME = millis();
      while(millis() - TIME < ledDelay){
        delay(5);
        if(CheckPressed())break;
      }
      if(CheckButtons() == 6) return;
      if(CheckPressed() && mode == 1){ 
        if(i == pixelSequence[index] || i == SecondPixel){
          index++;
          ledDelay = ledDelay - 15;
          if(ledDelay < 60) ledDelay = 60;
          tone(beep, 800, 80);
          wait();
        }
        
        else if(mode == 1) {        //game over
          tone(beep,300);
          wait();
          for(int j = 0; j < 4; j++){
            pixels.setPixelColor(i, pixels.Color(200, 0, 0));
            pixels.show();
            delay(100);
            pixels.setPixelColor(i, pixels.Color(0, 0, 0)); 
            pixels.show();
            delay(100);
          } 
          noTone(beep);
          pixels.clear();
          pixels.show();
          return;  
        }
      }
    }
  }
}

void fade(int index){
  for(int j = 0; j < 24; j++){
        
        if(pixelSequence[index] > 11)SecondPixel = pixelSequence[index]-12;
        else SecondPixel = 24 + pixelSequence[index]-12;
        
        if(pix == pixelSequence[index] || pix == SecondPixel) pixels.setPixelColor(pix, pixels.Color(0, light/2, light)); //RGB
        else pixels.setPixelColor(pix, pixels.Color(light, 0, light/2)); //RGB
        PIX_minus();
      }

      pixels.show();  
}

void PIX_minus(){
  pix--;
  if(pix < 0)pix = 24 + pix;
  light = light - 14;
  if(light < 5)light = 10;
}



void led(){
  int ledMode = 0;
  while(mode == 2){
    switch(ledMode){
      case 0:  //ranbo
        uint16_t i, j;
        for(j=0; j<256; j++) { // cycles of all colors on wheel
          if(CheckPressed())break;
          for(i=0; i< pixels.numPixels(); i++) {
            pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
            if(CheckPressed())break;
          }
          pixels.show();
          delay(2);
        }
      break;
      case 1:
         for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
          if(CheckPressed())break;
          for (int q=0; q < 3; q++) {
            for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
              pixels.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
              if(CheckPressed())break;
            }
            if(CheckPressed())break;
            pixels.show();
      
            delay(40);
      
            for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
              pixels.setPixelColor(i+q, 0);        //turn every third pixel off
              if(CheckPressed())break;
            }
          }
        }
      break;
      case 2:   
       for (int j=0; j<10; j++) {  //do 10 cycles of chasing
        if(CheckPressed())break;
          for (int q=0; q < 3; q++) {
            if(CheckPressed())break;
            for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
              if(CheckPressed())break;
              pixels.setPixelColor(i+q, pixels.Color(150, 100, 150));    //turn every third pixel on
            }
            pixels.show();
      
            delay(40);
      
            for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
              if(CheckPressed())break;
              pixels.setPixelColor(i+q, 0);        //turn every third pixel off
            }
          }
        }
      
        
      break;
      case 3:
      for(int i = 0; i < NUMPIXELS; i++) { // For each pixel...
        pix = i;
        light = 250;
        for(int j = 0; j < 24; j++){    
          pixels.setPixelColor(pix, pixels.Color(light, light/3, 0)); //RGB
          PIX_minus();
        }
        pixels.show(); 
        delay(10);
        if(CheckPressed())break;
      }
         
      break;
    }
    
    
    if(CheckPressed()){
      int ledMODE = CheckButtons();
      if(ledMODE != 7)ledMode = ledMODE; 
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
