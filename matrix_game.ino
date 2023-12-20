#include "LedControl.h"
#include "LiquidCrystal.h"
#include <EEPROM.h>

// ===== MATRIX ===== //  

const byte DIN_PIN = 12; 
const int CLK_PIN = 11;
const int LOAD_PIN = 10;
const byte matrixSize = 8;
LedControl lc = LedControl(DIN_PIN, CLK_PIN, LOAD_PIN, 1);
byte matrixBrightness = 2;
int matrixBrightnessLevel = 5;
const int matrixBrightnessAddress=20;

// ===== JOYSTICK ===== //
const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;
int xValue = 0;
int yValue = 0;
byte swState = LOW;

int playerX = 7;
int playerY = 0;

// ===== PLAYER BLINK AT DIFFERENT RATES ===== //

bool playerVisible = true;
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 200;

// ===== WALLS ===== //

int wallCount = 18;
int walls[18][2] = {{1,1}, {2,1}, {3,1}, {4,1}, {4,2}, {6,1}, {6,2}, {6,3}, {5,4}, {3,4}, {2,4}, {2,5}, {2,6}, {4,6}, {5,6}, {6,6}, {7,6}, {7,5}};


// ===== BOMB ===== //

bool bombPlaced = false;
int bombX, bombY;
unsigned long bombPlacedTime = 0;
const unsigned long bombDuration = 3000;
bool bombExploded = false;
unsigned long explosionTime = 0;
const unsigned long explosionDuration = 1000;

// ===== ENEMY ===== //

int enemyX = 0, enemyY = 0;
int enemyDirection = 1;
unsigned long lastEnemyMoveTime = 0;
const unsigned long enemyMoveInterval = 500;
bool enemyAlive = true;
bool enemy2Alive = true;

// ===== ENEMY2 ===== //

int enemy2X = 5;
int enemy2Y = 3;
int enemy2TargetX = 5;
int enemy2TargetY = 6;
int enemy2Direction = 1;
unsigned long lastEnemy2MoveTime = 0;
const unsigned long enemy2MoveInterval = 500;
int enemiesAlive = 2;

// ===== LCD ====== //

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 3;
const byte d7 = 4;
const byte v0 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
String currentLCDMessage = "";
unsigned long lastLCDUpdate = 0;
const unsigned long lcdUpdateInterval = 500;

// ===== GAME LOGIC ===== //

int score = 0;
bool gameOver = false;
bool gameOn = false;
bool okMessage=false;
int highscore=0;
const int maxHighscores = 10;
int highscores[maxHighscores] = {0};
unsigned long gameStartTime;
int lives=2;

// ===== MENU ===== //

const char* menuItems[] = {"Start Game", "Highscore", "Settings", "About", "Exit"};
const int menuSize = 5;
int currentMenuItem = 0;

// ===== SETTINGS SUBMENU ===== //

bool inSettingsMenu = false;
const char* settingsItems[] = {"LCD Brightness", "Matrix Brightness", "Sounds", "Back"};
const int settingsSize = 4;
int currentSettingsItem = 0;

// ===== ABOUT ===== //

const char* infoItems[] = {"Game: BOMBERMAN", "Author: Vect0r", "GitHub: link_here", "Back"};
const int infoSize = 4;
int currentInfoItem = 0;
bool inInfoMenu=false;

//lcd brightness

int lcdBrightnessLevel = 5;



void setup() {
  pinMode(pinSW, INPUT_PULLUP);
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
  lcd.begin(16, 2);
  lcd.clear();
  analogWrite(3,128);
  analogWrite(5,100);
  loadHighscores();
  Serial.begin(9600);
  lives=2;
  EEPROM.update(matrixBrightnessAddress, 0);
}


void loop() {

  swState = digitalRead(pinSW);
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  readJoystickSettings();
  if (inSettingsMenu) {
    showSettingsMenu();
  } else if(!gameOn){
    if(!okMessage){
      displayScrollingMessage();
      okMessage=!okMessage;
    }
    readJoystick();
    showMenu();
    if (digitalRead(pinSW) == LOW) {  // Button pressed
      switch (currentMenuItem) {
        case 0:
          gameOn=true;
          gameStartTime = millis();
          break;
        case 1:
          displayHighscores();
          break;
        case 2:
          inSettingsMenu = true;
          break;
        case 3:
          inInfoMenu=true;
          inSettingsMenu = false;
          break;
        case 4:
          // Exit
          break;
      }
    }
  if (inInfoMenu) {
    readJoystickForInfo();
    showInfo();
  } 

  delay(100);

  }else{
    
    lcd.clear();
    if (!gameOver) {
      // input of the joystick
      if (xValue < 200) {
        movePlayer(-1, 0);
      } else if (xValue > 800) {
        movePlayer(1, 0);
      }

      if (yValue < 200) {
        movePlayer(0, -1);
      } else if (yValue > 800) {
        movePlayer(0, 1);
      }

      //enemies
      unsigned long currentTimeEnemy = millis();
      if (currentTimeEnemy - lastEnemyMoveTime >= enemyMoveInterval) {
        enemyX += enemyDirection;
        if (enemyX <= 0 || enemyX >= matrixSize - 1) {
          enemyDirection = -enemyDirection;
        }
        lastEnemyMoveTime = currentTimeEnemy;
      }

      if (currentTimeEnemy - lastEnemy2MoveTime >= enemy2MoveInterval) {
        enemy2Y += enemy2Direction;
        if (enemy2Y == enemy2TargetY || enemy2Y == 3) {
          enemy2Direction = -enemy2Direction;
        }
        lastEnemy2MoveTime = currentTimeEnemy;
      }
        //check for enemy

      if (enemyAlive && playerX == enemyX && playerY == enemyY) {
        lives--; // <== CHANGED: Decrement lives instead of score
        respawnPlayer();
        if (lives == 0) { // <== ADDED: Check if lives are depleted
          gameOver = true;
          lightUpMatrix();
          return;
        }
      }
      if (enemy2Alive && playerX == enemy2X && playerY == enemy2Y) {
        lives--; // <== CHANGED: Decrement lives instead of score
        respawnPlayer();
        if (lives == 0) { // <== ADDED: Check if lives are depleted
          gameOver = true;
          lightUpMatrix();
          return;
        }
      }

      //bomb
      if (swState == LOW && !bombPlaced) {
        bombPlaced = true;
        bombX = playerX;
        bombY = playerY;
        bombPlacedTime = millis();
      }
      if (bombPlaced && millis() - bombPlacedTime > bombDuration) {
        handleBombExplosion();
        bombPlaced = false;
        bombExploded = true;
        explosionTime = millis();
      }
      if (bombExploded && millis() - explosionTime > explosionDuration) {
        bombExploded = false;
      }

      //player
      lc.clearDisplay(0);
      unsigned long currentTime = millis();
      if (currentTime - lastBlinkTime >= blinkInterval) {
        playerVisible = !playerVisible;
        lastBlinkTime = currentTime;
      }

      //here are all the conditions for the game to end (without the loose condition) 

      if (score <= -7 && !gameOver) {
        gameOver = true;
        lightUpMatrix();
        return;
      }

      //update highscore
      if (score > highscore) {
        highscore = score;
      }
      //if all the enemies and walls are gone, than the game is finished
      if(wallCount == 0 && enemiesAlive == 0){
        addHighscore(highscore);
        gameOn=false;
        lightUpMatrix();
      }
      //you have maximum 25 seconds to finish the game
      if (millis() - gameStartTime > 25000) {
        addHighscore(highscore);
        gameOn=false;
        lightUpMatrix();
      }
      drawGameElements();
      delay(100);
    }
  }
  if(gameOn){
    updateGameInfoDisplay();
  }
  // lcd about section
  if (inInfoMenu) {
    readJoystickForInfo();
    showInfo();
  }
}

void drawGameElements() {
  lc.clearDisplay(0);

  drawWalls();
  drawPlayer();
  drawEnemies();
  drawBomb();
}

void drawWalls() {
  for (int i = 0; i < wallCount; i++) {
    lc.setLed(0, walls[i][0], walls[i][1], true);
  }
}

void drawPlayer() {
  if (playerVisible) {
    lc.setLed(0, playerY, playerX, true);
  }
}

void drawEnemies() {
  if (enemyAlive) {
        lc.setLed(0, 0, enemyX, true);
    }
    if (enemy2Alive) {
        lc.setLed(0, enemy2Y, enemy2X, true);
    }
}

void drawBomb() {
  if (bombExploded) {
    for (int i = 0; i < matrixSize; i++) {
      lc.setLed(0, bombY, i, true);
      lc.setLed(0, i, bombX, true);
    }
  } else if (bombPlaced) {
    lc.setLed(0, bombY, bombX, true);
  }
}

void movePlayer(int dx, int dy) {
  int newX = playerX + dx;
  int newY = playerY + dy;

  // check for wall
  for (int i = 0; i < wallCount; i++) {
    if (newX == walls[i][1] && newY == walls[i][0]) {
      return;
    }
  }

  // update player position if no collision

  playerX = newX;
  playerY = newY;

  // boundaries -> the player cannot move if it's on a marginal row or collum

  if (playerX < 0) playerX = 0;
  if (playerY < 0) playerY = 0;
  if (playerX >= matrixSize) playerX = matrixSize - 1;
  if (playerY >= matrixSize) playerY = matrixSize - 1;
}

void respawnPlayer() {
    playerX = 7;
    playerY = 7;
}

// ====== BOMB LOGIC ===== //

void handleBombExplosion() {

    // how bomb explosion affects walls
    for (int i = 0; i < wallCount; i++) {
        if (walls[i][0] == bombY || walls[i][1] == bombX) {
            score += 1;
            removeWall(i);
            i--;
        }
    }
    if ((enemyX == bombX && enemyY == bombY) || (enemy2X == bombX && enemy2Y == bombY)) {
        score += 2;
    }

    // how bomb explosion affects enemies - i used 2 boolean flags
    if ((enemyX == bombX || enemyY == bombY) && enemyAlive) {
        score += 2;
        enemyAlive = false;
        enemiesAlive--;
    }

    if ((enemy2X == bombX || enemy2Y == bombY) && enemy2Alive) {
        score += 2;
        enemy2Alive = false;
        enemiesAlive--;
    }
}

void removeWall(int index) {
    for (int j = index; j < wallCount - 1; j++) {
        walls[j][0] = walls[j + 1][0];
        walls[j][1] = walls[j + 1][1];
    }
    wallCount--;
}

// ===== LIGHT UP THE MATRIX IF THE GAME IS OVER ===== //

void lightUpMatrix() {
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            lc.setLed(0, row, col, true); // Turn on every LED in the matrix
        }
    }
    // while the matrix is full on, the game restarts it's parameters, so that when you press start game it's brand new
    score=0;
    displayScrollingMessageOver();
    gameOn=false;
    enemyAlive = true;
    enemy2Alive = true;
    wallCount = 18;
    enemiesAlive = 2;
    lives = 2;
    resetWalls();
}

void resetWalls() {
    int initialWalls[18][2] = {{1,1}, {2,1}, {3,1}, {4,1}, {4,2}, {6,1}, {6,2}, {6,3}, {5,4}, {3,4}, {2,4}, {2,5}, {2,6}, {4,6}, {5,6}, {6,6}, {7,6}, {7,5}};
    
    for (int i = 0; i < 18; i++) {
        walls[i][0] = initialWalls[i][0];
        walls[i][1] = initialWalls[i][1];
    }
}

// ===== LCD ===== //

void displayScrollingMessage() {

  const char *message= "Welcome to BOMBERMAN by Vect0r          ";
  int lcdWidth = 16;
  int messageLength = strlen(message);

  // Iterate through the message and scroll it on the LCD
  for (int i = 0; i < messageLength + lcdWidth; ++i) {
    //lcd.clear();
    lcd.setCursor(0, 0);

    // Display a portion of the message based on the current iteration
    for (int j = 0; j < lcdWidth; ++j) {
      int index = (i + j) % messageLength;
      lcd.write(message[index]);
    }

    delay(200);
  }

  delay(1000);
}

void displayScrollingMessageOver() {
  
  const char *message= "Game over, give it one more try!          ";
  int lcdWidth = 16;
  int messageLength = strlen(message);

  // Iterate through the message and scroll it on the LCD
  for (int i = 0; i < messageLength + lcdWidth; ++i) {
    //lcd.clear();
    lcd.setCursor(0, 0);

    // Display a portion of the message based on the current iteration
    for (int j = 0; j < lcdWidth; ++j) {
      int index = (i + j) % messageLength;
      lcd.write(message[index]);
    }

    delay(200);
  }
  gameOver=false;
  delay(1000);
}

void readJoystick() {
  int xValue = analogRead(pinX);
  int yValue = analogRead(pinY);

  if (yValue < 200) {
    currentMenuItem--;
    if (currentMenuItem < 0) currentMenuItem = menuSize - 1;
  } else if (yValue > 800) {
    currentMenuItem++;
    if (currentMenuItem >= menuSize) currentMenuItem = 0;
  }
}

void showMenu(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menuItems[currentMenuItem]);
}

// ===== HIGHSCORES ===== //

void addHighscore(int newScore) {
    // add the new score at the end of the array
    highscores[maxHighscores - 1] = newScore;

    // simple sort
    for (int i = maxHighscores - 1; i > 0; i--) {
        if (highscores[i] > highscores[i - 1]) {
            // Swap
            int temp = highscores[i];
            highscores[i] = highscores[i - 1];
            highscores[i - 1] = temp;
        } else {
            break;
        }
    }
    saveHighscores();
}

void displayHighscores() {
    lcd.clear();
    lcd.setCursor(0, 0);
    int i=0;
    lcd.setCursor(0, 1);
    lcd.print(String(highscores[i]) + " ");
    i++;
    lcd.setCursor(4, 1);
    lcd.print(String(highscores[i]) + " ");
    i++;
    lcd.setCursor(8, 1);
    lcd.print(String(highscores[i]) + " ");
    // for (int i = 0; i < 3; i++) {
    //     lcd.setCursor(i+1, i);
    //     lcd.print(String(highscores[i]) + " ");
    // }
}

void saveHighscores() {
    int addr = 0;  // Start address in EEPROM
    for (int i = 0; i < maxHighscores; i++) {
        EEPROM.put(addr, highscores[i]);
        addr += sizeof(int);  // Move to the next address
    }
}

void loadHighscores() {
    int addr = 0;  // Start address in EEPROM
    for (int i = 0; i < maxHighscores; i++) {
        EEPROM.get(addr, highscores[i]);
        addr += sizeof(int);  // Move to the next address
    }
}

// ===== SETTINGS SUBMENU ===== //

void readJoystickSettings() {
  int xValue = analogRead(pinX);
  int yValue = analogRead(pinY);

  if (inSettingsMenu) {
    // Navigation logic for settings submenu
    if (yValue < 200) {
      currentSettingsItem--;
      if (currentSettingsItem < 0) currentSettingsItem = settingsSize - 1;
      delay(100);
    } else if (yValue > 800) {
      currentSettingsItem++;
      if (currentSettingsItem >= settingsSize) currentSettingsItem = 0;
      delay(100);
    }

    if (currentSettingsItem == 0 && digitalRead(pinSW) == LOW) {
      adjustLCDBrightness();
      inSettingsMenu = false;
    }

    if (digitalRead(pinSW) == LOW) {
      switch (currentSettingsItem) {
        case 0:
          inSettingsMenu = false;
          delay(500);
          adjustLCDBrightness(); 
          break;
        case 1:
          inSettingsMenu = false;
          delay(500);
          adjustMatrixBrightness();
          break;
        case 2:
          // Toggle Sounds
          lcd.clear();
          lcd.print("workzz!!");
          delay(500);
          lcd.clear();
          inSettingsMenu = false;
          break;
        case 3:
          delay(500);
          inSettingsMenu = false;
          break;
      }
    }
  }
}

void adjustLCDBrightness() {
    lcd.clear();
    while (digitalRead(pinSW) == HIGH) {
        lcd.setCursor(0, 0);
        lcd.print("Brightness: ");
        lcd.print(lcdBrightnessLevel);

        int yValue = analogRead(pinY);
        if (yValue < 200) {
            lcdBrightnessLevel--;
            delay(200);
        } else if (yValue > 800) {
            lcdBrightnessLevel++;
            delay(200);
        }

        // this function constraints the variable between 1 and 10 (aka puts inferior and superior limits)
        lcdBrightnessLevel = constrain(lcdBrightnessLevel, 1, 10);

        // map brightness level to pmw
        int pwmValue = map(lcdBrightnessLevel, 1, 10, 0, 255);

        analogWrite(v0, pwmValue);
    }
}

void adjustMatrixBrightness() {

    bool settingBrightness = true;
    lcd.clear();
    lcd.print("Matrix Brightness");
    lcd.setCursor(0, 1);
    lcd.print(String(matrixBrightnessLevel));

    while (settingBrightness) {
        int joystickY = analogRead(pinY);
        if (joystickY < 200) {
            matrixBrightnessLevel++;
            delay(200);
        }
        else if (joystickY > 800) {
            matrixBrightnessLevel--;
            delay(200);
        }

        // used it at lcd brightness
        matrixBrightnessLevel = constrain(matrixBrightnessLevel, 1, 10);

        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(String(matrixBrightnessLevel));

        if (digitalRead(pinSW) == LOW) {
            settingBrightness = false;
        }
    }

    int actualBrightness = map(matrixBrightnessLevel, 1, 10, 0, 15);
    lc.setIntensity(0, actualBrightness);


    EEPROM.put(matrixBrightnessAddress, matrixBrightnessLevel);

    lcd.clear();
}

void showSettingsMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(settingsItems[currentSettingsItem]);
}

// ===== SHOW LIVES, SCORE AND TIME IN REAL TIME ===== //

void updateGameInfoDisplay() {
    //lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Lives: ");
    lcd.print(lives);

    lcd.setCursor(9, 0);
    lcd.print("Score: ");
    lcd.print(score);
    
    unsigned long currentTime = millis();
    int timeElapsed = (currentTime - gameStartTime) / 1000;
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(timeElapsed);
}

// ===== ABOUT SECTION - SAME SCROLL LOGIC WITH BACK BUTTON ===== //

void readJoystickForInfo() {
    int yValue = analogRead(pinY);

    if (yValue < 200) {
        currentInfoItem--;
        if (currentInfoItem < 0) currentInfoItem = infoSize - 1;
    } else if (yValue > 800) {
        currentInfoItem++;
        if (currentInfoItem >= infoSize) currentInfoItem = 0;
    }

}

void showInfo() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(infoItems[currentInfoItem]);
    if (currentInfoItem == 3 && digitalRead(pinSW) == LOW) {
        inInfoMenu = false;
    }
}
 
  
