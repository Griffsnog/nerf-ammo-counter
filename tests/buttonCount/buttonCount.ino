//libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//init stuff for display
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const byte BTN_PIN = 2;
int btnState, lastBtnState = LOW, cntr = 0; 

unsigned long lastDebounceTime = 0, debounceDelay = 50;

void setup() {
  // put your setup code here, to run once:
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  pinMode(BTN_PIN, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  btnState = digitalRead(BTN_PIN);

  if (btnState != lastBtnState) {
    if (btnState == HIGH) {
      cntr++;
      printToScreen(0, 10, "Pressed!")
    } else {
      printToScreen(0, 10, "Not Pressed!")      
    }
    
  }
  
  printToScreen(0, 0, String(cntr));
  
  lastBtnState = btnState;
}

void printToScreen (int x, int y, String text, bool toClearScreen) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.print(text);
  display.display();
}

