//By Monty C

/*Here's how it will work:
 * It's simple. When the trigger is pulled, count down 1 from the ammo. When a magazine is inserted, refill the ammo. A button can be 
 * used to toggle through the different magazine sizes: 5, 6, 10, 12, 15, 18, 22, 25, and 36. The last mode is counts up, starting from 0, 
 * which can be accessed from the toggling the magazine sizes. The magazine sizes will increment, with the press of the button, from 0, 
 * 5, 6, 12 … 36 and will not decrement, for the sake of simplicity.  
 * 
 * 
 * I am aware that the code isn't that good. It's not my best, not my neatest, and it's not too efficient for working with micro controllers, but it should be fine.
 */

//libraries, so the display can work
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Button.h>

//set the height and width of the screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//initialize stuff for display
#define OLED_RESET 4
#define TEXT_SIZE 8

//pins for button
#define TRIGGER_BTN_PIN 1
#define RELOAD_BTN_PIN 3
#define MAG_SZ_TOG_BTN_PIN 5

//parameters for buttons
#define INVERT true
#define PULLUP true
#define DEBOUNCE_MS 20

Adafruit_SSD1306 display(OLED_RESET);     //display

Button triggerBtn (TRIGGER_BTN_PIN, INVERT, PULLUP, DEBOUNCE_MS);       //trigger button
Button reloadBtn (RELOAD_BTN_PIN, INVERT, PULLUP, DEBOUNCE_MS);         //reloading button
Button magSzTogBtn (MAG_SZ_TOG_BTN_PIN, INVERT, PULLUP, DEBOUNCE_MS);   //magazine size toggle button

//stuff to help keep track of magazine stuff
//
//
//
//if you want to add/remove magazine sizes, do that here. Remember, they go in order when you toggle between them, from left to right in this.
//
//when you change the value, the "currentMagSize"(several lines below) has to be less than the number of different magazines sizes, 
//the number of different magazine size values in the "magSizeArr" you can change "currentMagSize" to whatever you want. 
//When it program first starts, when the microcontroller turns on, the 5th element of "magSizeArr" is the current magazine size,
//starting from 0. 
//Ex: byte array = {1(0), 2(1), 3(2), 4(3)} - the numbers without parenthesis are the values this array/list 
//stores, and the number between the parenthesis indicates which place they are, their "index", where they are in the list/array. 
//If I want to access the value 1, which is the first value of the array/list, which the computer sees as the 
//"zeroith" value, I would do array[0]. If I want to access the value 3, the third value of the array, I would do array[2]
byte magSizeArr[] = {5, 6, 10, 12, 15, 18, 20, 22, 25, 36, 0};  //keep track of the magazine sizes
byte currentMagSize = 0;  //keep track of the current magazine size
byte currentAmmo = magSizeArr[currentMagSize];    //keep track of how much ammo there currently is
byte maxAmmo = magSizeArr[currentMagSize];    //keep track of what the max ammo is, for use when reloading 

//this code will run when the Arduino turns on
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    //begin stuff for the display
  initDisplayAmmo();     //show the ammo
}

//this code loops many times a second
void loop() {
  //count ammo, constantly check for the trigger switch to be pressed to count
  countAmmo();
  //toggle between magazine sizes, constanly check for the magazine toggle switch to be pressed
  toggleMags();
  //change magazine, constantly check for the magazine switch to be pressed/not pressed
  changeMag();
}

void displayAmmo (String ammoToDisplay) {
  display.clearDisplay(); //clear the display, so the stuff that was here before is no longer here
  display.setTextSize(TEXT_SIZE);  //set the size of the text
  display.setTextColor(WHITE);    //set the color of text text
  //tell the display where to draw the text
  display.setCursor( (SCREEN_WIDTH/2) - ((text.length()*2) * (textSize * 1.5)), (SCREEN_HEIGHT/2) - (textSize * 3) );  //center text
  display.print(ammoToDisplay);    //print the text
  display.display();    //display the text
}

void initDisplayAmmo () {
  //if the ammo to print, current ammo, is less that 10, make it like '01' or '04'  
  String ammoToDisplay = currentAmmo < 10 ? ("0" + (String)currentAmmo) : (String)currentAmmo;
  displayAmmo(ammoToDisplay);  //display the text, the ammo
}
