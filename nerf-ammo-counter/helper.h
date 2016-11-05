//yay done! Now just need to test...


class Button {
    public:
        int PIN;
    
    //constructors
    public:
        Button();
        Button(int);
    
    public:
        unsigned long lastDebounceTime;
        unsigned long debounceDelay = 50;
        
        int btnState;
        int lastBtnState = LOW;
    
    public:
        bool isBtnPressed() {
            bool returnVal = false;     //flag so can return at end of method
            
            this -> btnState = digitalRead(this -> PIN);
            
            //delay
            if (micros() >= (this -> lastDebounceTime) + (this -> debounceDelay)) {
                //check if button changes state
                if (this -> btnState != this -> lastBtnState) {
                    //check if btn acutally pressed
                    if (this -> btnState == HIGH) {
                        returnVal = true;
                    }
                }   
                
            }
            
            this -> lastDebounceTime = micros();
            this -> lastBtnState = this -> btnState;
            
            return returnVal;
        }   //method

};

//constructors
Button::Button () {
    PIN = -1;
}

Button::Button (int pin) { 
    PIN = pin;

    pinMode(pin, INPUT);
}


byte magSizeArr[9] = {5, 6, 10, 12, 15, 18, 22, 25, 36};
byte currentMagSize = 5;
byte currentAmmo = magSizeArr[currentMagSize]; 
byte maxAmmo = magSizeArr[currentMagSize];

Button btnArr[5];
void initButtons (int numOfBtns) {
  btnArr [numOfBtns];

  //0 = trigger, 1 = mag release, 2 = toggle mag
  for (int i = 0; i < numOfBtns; i++) {
    btnArr[i] = Button(i);  
  }
  
}

void displayAmmo(){
  String text = (String)currentAmmo;

  byte textSize = 8;
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor( (SCREEN_WIDTH/2) - ((text.length()*2) * (textSize * 1.5)), (SCREEN_HEIGHT/2) - (textSize * 3) );  //center text
  display.print(text);
  display.display();
}

void countAmmo() {
  if (btnArr[0].isBtnPressed()) {
    if (currentAmmo > 0) {
      currentAmmo--;
    }
  }
  displayAmmo();
  
}

void changeMag() {
  if (btnArr[1].isBtnPressed()) {
    currentAmmo = maxAmmo;
    displayAmmo();
  }
  
}

void toggleMags () {
  if (btnArr[2].isBtnPressed()) {
    if (currentMagSize < 8) {
      currentMagSize ++;
    } else {
      currentMagSize = 0;
    }

    maxAmmo = magSizeArr[currentMagSize];
    currentAmmo = maxAmmo;
    displayAmmo();
  }
  
}








