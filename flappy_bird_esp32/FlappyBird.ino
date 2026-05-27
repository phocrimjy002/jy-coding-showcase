#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BTN_JUMP 2
#define BTN_RESTART 4
#define SDA_PIN 23
#define SCL_PIN 22

const float GRAVITY = 0.4;
const float JUMP_STRENGTH = -2.5;
const int BIRD_X = 25;
const int PIPE_WIDTH = 12;
const int PIPE_GAP = 28;

float birdY = 32.0;
float velocity = 0.0;
int score = 0;
bool gameOver = false;

struct Pipe {
  int x;
  int gapY;
  bool passed;
};

Pipe pipes[2];

void setup() {
  pinMode(BTN_JUMP, INPUT_PULLUP);
  pinMode(BTN_RESTART, INPUT_PULLUP);

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;); 
  }

  resetGame();
}

void loop() {
  if (gameOver) {
    drawGameOver();
    if (digitalRead(BTN_RESTART) == LOW) {
      resetGame();
      delay(300); 
    }
    return;
  }

  handleInput();
  updatePhysics();
  updatePipes();
  checkCollisions();
  drawFrame();

  delay(20);
}

void handleInput() {
  if (digitalRead(BTN_JUMP) == LOW) {
    velocity = JUMP_STRENGTH;
  }
}

void updatePhysics() {
  velocity += GRAVITY;
  birdY += velocity;
}

void updatePipes() {
  for (int i = 0; i < 2; i++) {
    pipes[i].x -= 3;

    if (pipes[i].x < -PIPE_WIDTH) {
      pipes[i].x = 128;
      pipes[i].gapY = random(10, SCREEN_HEIGHT - PIPE_GAP - 10);
      pipes[i].passed = false;
    }

    if (!pipes[i].passed && pipes[i].x < BIRD_X) {
      score++;
      pipes[i].passed = true;
    }
  }
}

void checkCollisions() {
  if (birdY < 0 || birdY > SCREEN_HEIGHT - 4) {
    gameOver = true;
  }

  for (int i = 0; i < 2; i++) {
    if (BIRD_X + 4 > pipes[i].x && BIRD_X < pipes[i].x + PIPE_WIDTH) {
      if (birdY < pipes[i].gapY || birdY + 4 > pipes[i].gapY + PIPE_GAP) {
        gameOver = true;
      }
    }
  }
}

void drawFrame() {
  display.clearDisplay();

  for (int i = 0; i < 2; i++) {
    display.fillRect(pipes[i].x, 0, PIPE_WIDTH, pipes[i].gapY, SSD1306_WHITE);
    display.fillRect(pipes[i].x, pipes[i].gapY + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  }

  display.fillRect(BIRD_X, (int)birdY, 5, 5, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Score: ");
  display.print(score);

  display.display();
}

void drawGameOver() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.println("GAME OVER");
  
  display.setTextSize(1);
  display.setCursor(10, 35);
  display.print("Score: ");
  display.println(score);
  display.setCursor(10, 50);
  display.println("Press Restart");
  display.display();
}

void resetGame() {
  birdY = 32.0;
  velocity = 0.0;
  score = 0;
  gameOver = false;
  
  pipes[0] = {128, random(10, 30), false};
  pipes[1] = {128 + 70, random(10, 30), false};
}