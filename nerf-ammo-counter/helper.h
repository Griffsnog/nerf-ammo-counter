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

Button::Button () {
    PIN = 0;
}

Button::Button (int pin) { 
    PIN = pin;
}




