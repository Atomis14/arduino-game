#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

class Renderer {
  private:
    const byte heart[8] = {
      0b00000,
      0b01010,
      0b11111,
      0b11111,
      0b01110,
      0b00100,
      0b00000,
      0b00000
    };
    
  public:
    static const byte CHAR_HEART = byte(0);

    Renderer() {
      lcd.createChar(0, this->heart);
      lcd.begin(16, 2);
    }

    void screenWelcome() {
      lcd.clear();
      
      lcd.setCursor(0,1);
      lcd.print("starting game...");
      
      lcd.setCursor(0,0);
      lcd.print("   W E L C O M E");
      delay(100);
      lcd.setCursor(0,0);
      lcd.print("  W E L C O M E ");
      delay(100);
      lcd.setCursor(0,0);
      lcd.print(" W E L C O M E  ");
      delay(100);
      lcd.setCursor(0,0);
      lcd.print("W E L C O M E   ");
      delay(100);
      lcd.setCursor(0,0);
      lcd.print("WE L C O M E    ");
      delay(100);
      lcd.setCursor(0,0);
      lcd.print("WEL C O M E     ");
      delay(100);
      lcd.setCursor(0,0);
      lcd.print("WELC O M E      ");
      delay(100);
      lcd.setCursor(0,0);
      lcd.print("WELCO M E       ");
      delay(100);
      lcd.setCursor(0,0);
      lcd.print("WELCOM E        ");
      delay(100);
      lcd.setCursor(0,0);
      lcd.print("WELCOME         ");
      
      delay(1500);
      lcd.clear();
    }

    void screenGameplay(int progress, int lives, int level, bool isTargetChar, bool updateChar) { 
      // show play field
      lcd.setCursor(0, 0);
      lcd.print("|");
      for (int i = 0; i < 12; i++) {
        if (i < progress) {
          lcd.print("~");
        } else {
          lcd.print(" ");  
        }
      }
      lcd.print("|");

      // show changing characters
      if (updateChar) {
        const char characters[] = {"QWERTYUIOPASDFGHJKLZCVBNM"};
        lcd.setCursor(15, 0);
        if (isTargetChar == 1) {
          lcd.print("X");
        } else {
          lcd.print(characters[random(0, 25)]);  
        }
      }
  
      // show level
      lcd.setCursor(0, 1);
      lcd.print("Level: ");
      lcd.print(level);

      // show lives
      lcd.setCursor(12, 1);
      lcd.print("    ");
      for (int i = 0; i < lives; i++) {
        lcd.setCursor(15 - i, 1);
        lcd.write(this->CHAR_HEART);
      }
    }

    void levelUp() {
      for (int i = 0; i < 3; i++) {
        lcd.setCursor(0, 0);
        lcd.print("|            |");
        delay(300);
        lcd.setCursor(1, 0);
        for (int j=0; j<12; j++) {
          lcd.print("~");
          delay(30);
        }
        delay(200);
      }
    }

    void screenGameover(int points) {
      lcd.clear();
      lcd.print("       GAME OVER");
      lcd.setCursor(0, 1);
      lcd.print("Your Score: ");
      lcd.print(points);
    }
};
Renderer renderer;

class Game {
  private:
    int maxLives = 3;
    int probabilityForX = 50;
    int gameSpeed = 1000;

    unsigned long timestamp = 0;

    int currentLives;
    bool lost;
    int currentLevel;
    int progress;
    int points;

    bool buttonPressed = false;
    bool isX = false;

  public:
    void startGame(int delayTime = 0) {
      this->lost = false;
      this->points = 0;
      this->gameSpeed = 1000;
      
      this->progress = 0;
      this->currentLives = this->maxLives;
      this->currentLevel = 1;

      renderer.screenGameplay(this->progress, this->currentLives, this->currentLevel, this->isX, true);
      delay(delayTime);
    }

    void updateGame() {
      if (!this->lost) {
        // check for button press
        if (digitalRead(6) == HIGH && this->buttonPressed == false) { // only consider button press once per cycle
          buttonPressed = true;
          if (this->isX) {
            this->points++;
            this->progress++;
            if(this->progress == 12) {
              this->progress = 0;
              this->currentLevel++;
              if (this->gameSpeed*0.9 > 0) {
                this->gameSpeed *= 0.9; 
              }
              renderer.levelUp();
            } else {
              renderer.screenGameplay(this->progress, this->currentLives, this->currentLevel, this->isX, true); 
            }
          } else {
            this->currentLives--;
            if (this->currentLives == 0) {
              renderer.screenGameover(this->points);
              this->lost = true;
              return;
            }
            renderer.screenGameplay(this->progress, this->currentLives, this->currentLevel, this->isX, false);
          }
        }
        
        if (this->timestamp == 0 || millis() - this->timestamp >= this->gameSpeed) {  // only update according to game speed
          // choose random if X or not
          if (random(0, 100) > this->probabilityForX && !this->isX) {  // avoid two times X in a row
            this->isX = true;
          } else {
            this->isX = false;  
          }

          // render
          renderer.screenGameplay(this->progress, this->currentLives, this->currentLevel, this->isX, true);
          this->timestamp = millis();
          this->buttonPressed = false;
        } 
      } else {
        if (millis() - this->timestamp > 2000 && digitalRead(6)) {
          lcd.clear();
          startGame(1000);
        }
      }
    }
};
Game game;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);   // Onboard Light
  pinMode(6, INPUT);              // mainButton

  renderer.screenWelcome();
  game.startGame();
}

void loop() {
  game.updateGame();
}
