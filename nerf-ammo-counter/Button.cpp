//for Button class

#include <iostream>
using namespace std;



class Button {
    public:
        Button();
        Button(int);
    
    public:
        unsigned long lastDebounceTime;
        unsigned long debounceDelay = 50;
        
        int btnState;
        int lastBtnState = 0;
    
    public:
        void initPin (int pin) {
            const int PIN = pin;
        }

};

Button::Button () {
    cout << "instantiated" << endl;
}

Button::Button (int pin) { 
    cout << pin<< endl;
}


int main () {
    Button btnArr[5];
    
}

