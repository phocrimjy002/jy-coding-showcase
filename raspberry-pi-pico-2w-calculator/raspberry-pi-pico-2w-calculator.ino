#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <LittleFS.h>
#include <math.h>

// ===================== PINS =====================
#define TFT_CS     17
#define TFT_DC     14
#define TFT_RST    15
#define TOUCH_CS   16   // T_CS
#define BUZZER_PIN 6

// ===================== COLORS =====================
#define C_BLACK   0x0000
#define C_GREEN   0x07E0
#define C_BLUE    0x07FF
#define C_PINK    0xF81F
#define C_WHITE   0xFFFF
#define C_GREY    0x4208
#define C_RED     0xF800
#define C_YELLOW  0xFFE0

struct Point { int x; int y; bool touched; };
struct GuessResult { String val; uint16_t colors[4]; };

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
SPISettings touchSPI(500000, MSBFIRST, SPI_MODE0);

enum Mode { CALC, CODE, EDITOR, FILES };
Mode currentMode = CALC;
String inputStr = "";
uint32_t lastInteraction = 0;
int targetCode[4], attempts = 0;
GuessResult history[6];
String currentGuess = "";

const char* calcLabels[7][4] = {
  {"CLR", "DEL", "(", ")"}, {"7","8","9","/"}, {"4","5","6","*"},
  {"1","2","3","-"}, {"0",".","x","+"}, {"sqrt","=","^","pi"}, {" "," "," ","SOLVE"}
};

// Layout grid for the game mode numeric pad
const char* gameLabels[4][3] = {
  {"1", "2", "3"}, {"4", "5", "6"}, {"7", "8", "9"}, {"CLR", "0", "GUESS"}
};

void beep(int freq, int dur) { tone(BUZZER_PIN, freq, dur); }

// ======================================================
// RECOVERY TOUCH ENGINE
// ======================================================
uint16_t readRaw(uint8_t cmd) {
  SPI1.beginTransaction(touchSPI);
  digitalWrite(TOUCH_CS, LOW);
  SPI1.transfer(cmd);
  uint16_t val = SPI1.transfer16(0x00) >> 3;
  digitalWrite(TOUCH_CS, HIGH);
  SPI1.endTransaction();
  return val & 0xFFF;
}

Point getTouch() {
  uint16_t z = readRaw(0xB1); 
  if (z < 25) return {0, 0, false}; 

  const int SAMPLES = 5;
  uint32_t tx = 0, ty = 0;
  int valid = 0;

  for (int i = 0; i < SAMPLES; i++) {
    uint16_t rx = readRaw(0xD1); 
    uint16_t ry = readRaw(0x91);
    if (rx > 100 && rx < 4000 && ry > 100 && ry < 4000) {
      tx += rx; ty += ry; valid++;
    }
  }

  if (valid < 3) return {0, 0, false};

  int avgX = tx / valid;
  int avgY = ty / valid;

  int sx = map(avgY, 350, 3850, 240, 0); 
  int sy = map(avgX, 350, 3850, 0, 320);

  return { constrain(sx, 0, 239), constrain(sy, 0, 319), true };
}

// ===================== MATH ENGINE =====================
double parseExpr(const char** p);
double parseAtom(const char** p) {
  while (**p == ' ') (*p)++;
  if (**p == '(') { (*p)++; double res = parseExpr(p); if (**p == ')') (*p)++; return res; }
  if (isdigit(**p) || **p == '.') { char* end; double res = strtod(*p, &end); *p = end; return res; }
  if (strncmp(*p, "pi", 2) == 0) { (*p) += 2; return 3.14159; }
  if (strncmp(*p, "sqrt", 4) == 0) { (*p) += 4; return sqrt(parseAtom(p)); }
  return 0;
}
double parseFactor(const char** p) {
  double res = parseAtom(p);
  while (**p == '^') { (*p)++; res = pow(res, parseAtom(p)); }
  return res;
}
double parseTerm(const char** p) {
  double res = parseFactor(p);
  while (**p == '*' || **p == '/' || **p == 'x') {
    char op = **p; (*p)++;
    if (op == '/') { double d = parseFactor(p); res = (d != 0) ? res / d : 0; }
    else res *= parseFactor(p);
  }
  return res;
}
double parseExpr(const char** p) {
  double res = parseTerm(p);
  while (**p == '+' || **p == '-') {
    char op = **p; (*p)++;
    if (op == '+') res += parseTerm(p); else res -= parseTerm(p);
  }
  return res;
}

void solveMaster() {
  const char* pstr = inputStr.c_str();
  double result = parseExpr(&pstr);
  inputStr = (result == (long)result) ? String((long)result) : String(result, 2);
}

// ===================== GAME LOGIC ENGINE =====================
void processGuess() {
  if (currentGuess.length() != 4 || attempts >= 6) return;

  GuessResult res;
  res.val = currentGuess;
  bool targetUsed[4] = {false, false, false, false};
  bool guessUsed[4]  = {false, false, false, false};

  // Default all feedback blocks to Grey
  for (int i = 0; i < 4; i++) res.colors[i] = C_GREY;

  // First Pass: Match exact positions (Green / Bulls)
  for (int i = 0; i < 4; i++) {
    int gDigit = currentGuess[i] - '0';
    if (gDigit == targetCode[i]) {
      res.colors[i] = C_GREEN;
      targetUsed[i] = true;
      guessUsed[i] = true;
    }
  }

  // Second Pass: Match wrong positions (Yellow / Cows)
  for (int i = 0; i < 4; i++) {
    if (guessUsed[i]) continue;
    int gDigit = currentGuess[i] - '0';
    for (int j = 0; j < 4; j++) {
      if (!targetUsed[j] && gDigit == targetCode[j]) {
        res.colors[i] = C_YELLOW;
        targetUsed[j] = true;
        break;
      }
    }
  }

  history[attempts] = res;
  attempts++;
  currentGuess = ""; // Clear current string for next guess
}

// ===================== UI DRAWING =====================
void drawUI() {
  tft.setRotation(0);
  tft.fillScreen(C_BLACK);
  
  // Navigation Menu Bar
  const char* labs[] = { "CALC", "CODE", "EDIT", "FILE" };
  for (int i = 0; i < 4; i++) {
    tft.fillRect(i * 60, 0, 59, 30, (i == (int)currentMode) ? C_BLUE : C_GREY);
    tft.setCursor(i * 60 + 10, 10); tft.setTextColor(C_WHITE); tft.print(labs[i]);
  }
  
  // CALC MODE UI
  if (currentMode == CALC) {
    tft.drawRect(5, 35, 230, 45, C_GREEN);
    tft.setCursor(10, 50); tft.setTextColor(C_PINK); tft.setTextSize(2); tft.print(inputStr); tft.setTextSize(1);
    for (int r = 0; r < 7; r++) {
      for (int c = 0; c < 4; c++) {
        int bx = 6 + c * 58, by = 85 + r * 33;
        tft.drawRoundRect(bx, by, 56, 31, 4, C_BLUE);
        tft.setCursor(bx + 10, by + 12); tft.setTextColor(C_WHITE); tft.print(calcLabels[r][c]);
      }
    }
  } 
  // CODE GAME MODE UI
  else if (currentMode == CODE) {
    // Display current player active entry block
    tft.drawRect(5, 35, 230, 35, C_PINK);
    tft.setCursor(15, 45); tft.setTextColor(C_WHITE); tft.setTextSize(2);
    tft.print("Guess: " + currentGuess); tft.setTextSize(1);

    // Draw the historical logs (Up to 6 attempts)
    for (int i = 0; i < attempts; i++) {
      int entryY = 75 + (i * 20);
      tft.setCursor(10, entryY + 4);
      tft.setTextColor(C_WHITE);
      tft.print("Try " + String(i + 1) + ": ");
      
      // Print digits with matching color responses
      for (int j = 0; j < 4; j++) {
        tft.fillRect(65 + (j * 16), entryY, 12, 14, history[i].colors[j]);
        tft.setCursor(68 + (j * 16), entryY + 3);
        tft.setTextColor(C_BLACK);
        tft.print(history[i].val[j]);
      }
    }

    // Win condition text indicator
    if (attempts > 0 && history[attempts - 1].colors[0] == C_GREEN && history[attempts - 1].colors[1] == C_GREEN && history[attempts - 1].colors[2] == C_GREEN && history[attempts - 1].colors[3] == C_GREEN) {
      tft.setCursor(140, 120); tft.setTextColor(C_GREEN); tft.setTextSize(2); tft.print("WIN!"); tft.setTextSize(1);
    } else if (attempts >= 6) {
      tft.setCursor(140, 120); tft.setTextColor(C_RED); tft.setTextSize(2); tft.print("LOSE"); tft.setTextSize(1);
    }

    // Draw localized Game Keyboard (Bottom Half)
    for (int r = 0; r < 4; r++) {
      for (int c = 0; c < 3; c++) {
        int bx = 5 + c * 76, by = 195 + r * 30;
        tft.drawRoundRect(bx, by, 72, 26, 4, C_BLUE);
        tft.setCursor(bx + 12, by + 9); tft.setTextColor(C_WHITE); tft.print(gameLabels[r][c]);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TFT_RST, OUTPUT); digitalWrite(TFT_RST, LOW); delay(50); digitalWrite(TFT_RST, HIGH);
  
  SPI.setRX(16); SPI.setTX(19); SPI.setSCK(18); SPI.begin();
  SPI1.setRX(8); SPI1.setTX(11); SPI1.setSCK(10); SPI1.begin();
  
  pinMode(8, INPUT); 

  pinMode(TFT_CS, OUTPUT); digitalWrite(TFT_CS, HIGH);
  pinMode(TOUCH_CS, OUTPUT); digitalWrite(TOUCH_CS, HIGH);

  tft.begin();
  LittleFS.begin();
  
  randomSeed(analogRead(26));
  for (int i = 0; i < 4; i++) targetCode[i] = random(0, 10);
  
  drawUI();
}

void loop() {
  Point p = getTouch();
  if (p.touched && (millis() - lastInteraction > 250)) { 
    lastInteraction = millis();
    beep(1200, 20);
    
    // Top Bar Tab Navigation
    if (p.y < 35) {
      currentMode = (Mode)(p.x / 60);
      drawUI();
    } 
    // Handle Calculator Screen Key Taps
    else if (currentMode == CALC && p.y > 80) {
      int c = (p.x - 6) / 58; int r = (p.y - 85) / 33;
      if (c >= 0 && c < 4 && r >= 0 && r < 7) {
        String cmd = calcLabels[r][c];
        if (cmd == "CLR") inputStr = "";
        else if (cmd == "DEL") { if (inputStr.length() > 0) inputStr.remove(inputStr.length() - 1); }
        else if (cmd == "SOLVE") solveMaster();
        else if (cmd != " ") inputStr += cmd;
        drawUI();
      }
    } 
    // Handle Game Screen Key Taps
    else if (currentMode == CODE && p.y >= 195) {
      int c = (p.x - 5) / 76; int r = (p.y - 195) / 30;
      if (c >= 0 && c < 3 && r >= 0 && r < 4) {
        String cmd = gameLabels[r][c];
        if (cmd == "CLR") {
          currentGuess = "";
        } else if (cmd == "GUESS") {
          processGuess();
        } else {
          if (currentGuess.length() < 4) { // Limits user selection to a max of 4 digits
            currentGuess += cmd;
          }
        }
        drawUI();
      }
    }
  }
}