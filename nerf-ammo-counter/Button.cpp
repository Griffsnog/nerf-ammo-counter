//for Button class

class Button {
    public:
        unsigned long lastDebounceTime;
        unsigned long debounceDelay = 50;
        
        int btnState;
        int lastBtnState = LOW;
    
    public:
        void initPin (int pin) {
            const int PIN = pin;
        }
    

}