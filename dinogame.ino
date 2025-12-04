#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

// ------------------ CUSTOM CHARACTERS ------------------
byte dinoBody[8]    = {0b00100,0b01110,0b11111,0b11111,0b01110,0b00100,0b00000,0b00000};
byte legForward[8]  = {0b00100,0b01100,0b01010,0b01110,0b00100,0b00000,0b00000,0b00000};
byte legBackward[8] = {0b00100,0b01010,0b01100,0b01110,0b00100,0b00000,0b00000,0b00000};
byte legJump[8]     = {0b00010,0b00110,0b00011,0b01110,0b00100,0b00000,0b00000,0b00000};
byte cactus[8]      = {0b00100,0b01110,0b01110,0b01110,0b11111,0b00100,0b00100,0b00000};

// ------------------ GAME VARIABLES ------------------
int dinoCol = 2;
int dinoRow = 1; 
int dinoState = 0; 
bool isJumping = false;
int jumpFrame = 0;

int obsCol = 15;
int score = 0;
bool gameOver = false;
bool gameStarted = false;

unsigned long lastFrameTime = 0;
const int frameDelay = 200;
const int jumpDistance = 4;
const int jumpDurationFrames = 3;

// ------------------ SETUP ------------------
void setup() {
  lcd.init();
  lcd.backlight();

  lcd.createChar(0,dinoBody);
  lcd.createChar(1,legForward);
  lcd.createChar(2,legBackward);
  lcd.createChar(3,legJump);
  lcd.createChar(4,cactus);

  Serial.begin(9600);
  lcd.clear();

  // Intro Screen
  lcd.setCursor(1,0); lcd.print("Dino by Rupam");
  lcd.setCursor(0,1); lcd.print("Press S to start");
}

// ------------------ LOOP ------------------
void loop() {
  // Wait until game starts
  if(!gameStarted){
    if(Serial.available() > 0){
      char c = Serial.read();
      if(c == 'S' || c == 's'){
        gameStarted = true;
        lcd.clear();
      }
    }
    return;
  }

  if(millis() - lastFrameTime < frameDelay) return;
  lastFrameTime = millis();

  // ------------------ CHECK SERIAL INPUT ------------------
  if(Serial.available() > 0){
    char c = Serial.read();
    if((c == ' ' || c == 'J' || c == 'j') && !isJumping && !gameOver){
      isJumping = true;
      jumpFrame = 0;
    }
    if((c == 'R' || c == 'r') && gameOver){
      // Restart game
      score = 0;
      obsCol = 15;
      isJumping = false;
      jumpFrame = 0;
      gameOver = false;
      lcd.clear();
      return;
    }
  }

  if(!gameOver){
    lcd.clear();

    // ------------------ DINO ------------------
    if(isJumping){
      lcd.setCursor(dinoCol,dinoRow-1);
      lcd.write(byte(0));
      lcd.setCursor(dinoCol,dinoRow);
      lcd.write(byte(3));
      jumpFrame++;
      if(jumpFrame >= jumpDurationFrames) isJumping=false;
    } else {
      lcd.setCursor(dinoCol,dinoRow);
      lcd.write(byte(0));
      if(dinoState==0){
        lcd.setCursor(dinoCol,dinoRow);
        lcd.write(byte(1));
        dinoState=1;
      } else {
        lcd.setCursor(dinoCol,dinoRow);
        lcd.write(byte(2));
        dinoState=0;
      }
    }

    // ------------------ OBSTACLE ------------------
    lcd.setCursor(obsCol,dinoRow);
    lcd.write(byte(4));

    // ------------------ COLLISION ------------------
    if(obsCol==dinoCol && !isJumping){
      gameOver = true;
      lcd.clear();
      lcd.setCursor(4,0); lcd.print("GAME OVER");
      lcd.setCursor(0,1); lcd.print("Score:");
      lcd.setCursor(6,1); lcd.print(score);
      lcd.setCursor(11,1); lcd.print("R=Restart");
      return;
    }

    // ------------------ MOVE OBSTACLE ------------------
    obsCol--;
    if(obsCol<0){
      obsCol=15;
      score++;
    }

    // ------------------ DISPLAY SCORE ------------------
    lcd.setCursor(12,0);
    lcd.print(score);
  } else {
    // Keep score updated during game over
    lcd.setCursor(6,1);
    lcd.print(score);
  }
}
