#include "Arduino.h"
#include "SmartBlaster.h"

using namespace SmartBlaster;

SmartBlaster (bool[] modes, byte[] IOPins, byte[] buttons, init[] magSizes) {
	lastVoltageCheckTime = 0;
	delayTime = 500;

	chronoToPrint = "";
    ammoToPrint = "";
    voltageToPrint = "";
    firingModeToPrint = "";

    fireMode = 0;	//0 = safe = SF, 1 = semi-automatic = SA, 2 = 3 round burst = 3b, 3 = fully automatic = AM

    IR_MAP_TRIP_VAL = 95;
    DART_LEGNTH_FEET = 0.2362208333;

    R1 = 100000.0 
    R2 = 10000.0

	initModes().initIOPins().initButtons();
}

SmartBlaster& initModes (bool[] modes) {
	_isIRGate = modes[0];
	_isChrono = modes[1];
	_isVoltmeter = modes[2];
	_isSelectFire = modes[3];

	return *this;
}

SmartBlaster& initIOPins (byte[] pins) {
	_AMMO_COUNTING_INPUT_PIN = pins[0];
	_MAG_INSERTION_DETECTION_PIN = pins[1];
	_MAG_SIZE_TOGGLE_INPUT_PIN = pins[2];

	if (_isVoltmeter) {
		_VOLTMETER_INPUT_PIN = pins[3];
	}

	if (_isSelectFire) {
		_TOGGLE_SELECT_FIRE_INPUT_PIN = pins[4];
		_SELECT_FIRE_OUTPUT_PIN = pins[5];
		pinMode(_SELECT_FIRE_OUTPUT_PIN, OUTPUT);
	}

	_I2C-SDA-Pin = 4;
    _I2C-SCL-Pin = 5;

    return *this;	
}

//buttons using the Buttons library
SmartBlaster& initButtons () {
	if (!_isIRGate) {
		ammoCountingButton = Button(_AMMO_COUNTING_INPUT_PIN, false, false, 25);
	} else {
		ammoCountingButton = 0;
	}

	magInsertionDetectionButton = Button(_MAG_INSERTION_DETECTION_PIN, false, false, 25);
	magSizeToggleButton = Button(_MAG_SIZE_TOGGLE_INPUT_PIN, false, false, 25);


	if (_isSelectFire) {
		toggleSelectFireButton = Button(_TOGGLE_SELECT_FIRE_INPUT_PIN, false, false, 25);
	} else {
		buttonArr[3] = 0;
	}


	return *this;
}

SmartBlaster& initMagSizes(int[] magSizes) {
	for (int i = 0; i < ((sizeof(magSizes)/sizeof(magSizes[0])) - 1), i++) {
		magSizeArr[i] = magSizes[i];
	}
    currentMagSize = 0;
    maxAmmo = magSizeArr[currentMagSize];
    curretAmmo = maxAmmo;

	return *this;
}

SmartBlaster& initDisplay(Adafruit_SSD1306 displayArg) {
	display = displayArg;

	return *this;
}

void displayValues () {
	//display ammo
	display.clearDisplay(); //clear the display, so the stuff that was here before is no longer here
	display.setTextSize(6);  //set the size of the text
	display.setTextColor(WHITE);    //set the color of text text
	//tell the display where to draw the text
	display.setCursor( (SCREEN_WIDTH/2) - ((ammoToPrint.length()*2) * 9) , (SCREEN_HEIGHT/2) - 30 );  //center text
	display.print(ammoToPrint);    //print the text

	display.setTextSize(1);

	//display chrono values
	if (_isChrono) {
		display.setCursor(0, 50);  
		display.print(chronoToPrint);
	}

	//display voltage values
	if (_isVoltmeter) {
		display.setCursor(60, 50);
		display.print(voltageToPrint);
	}

	//display fire mode
	if (_isSelectFire) {
		display.setCursor(80, 50);  
		display.print(firingModeToPrint);
	}	
  
	display.display(); //display the text
}

void initDisplayVoltage (Double voltage) {
	voltageToPrint = (String)voltage + " v";
	displayValues();
}

void initDisplayAmmo () {
	String textToDisplay = "00";    //create something to store what to print. This is empty now
    //if the ammo to print, current ammo, is less that 10, make it like '01' or '04'  
    if (currentAmmo < 10) {
      textToDisplay = "0" + (String)currentAmmo; //fill the thing we used to store what to print
    } else {    //if not, leave it as is
      textToDisplay = (String)currentAmmo;   //fill the thing we used to store what to print
    }

    ammoToPrint = textToDisplay;  //display the text, the ammo

	displayValues();
}

void initDisplayChrono (double fps) {
	if (fps == -1) {
	        chronoToPrint = ("ERR");
	    } else if (fps == -2) {
	        chronoToPrint = ("NO FPS");
	    } else {
	        chronoToPrint = ( (String)(fps)  + " fps" );
	}

	displayValues();	
}

void initDisplayFireMode() {
	//print text based on mode: 0 == S == Safety, 1 == SS == Single Shot, 2 == 3B == 3 Round Burst, 3 == A == Automatic
	if (fireMode == 0) {
	    modeToPrint = "S";
	} else if (fireMode == 1) {
	    modeToPrint = "SS";
	} else if (fireMode == 2) {
	    modeToPrint = "3B";
	} else if (fireMode == 3) {
	    modeToPrint = "A";
	}	

	displayValues();
}

void resetChronoVals() {
	tripTime = -10;
	exitTime = -10;
}

double calculateChronoReadings(double firstTime, double secondTime) {
	if ( (tripTime > -10) && (exitTime > -10) ) {
        resetChronoVals();
        return (DART_LEGNTH_FEET) / ((secondTime-firstTime)/1000000.0);
	}
}

void chrono() {
    //when tripped and expecting first value
    if ((map(analogRead(_AMMO_COUNTING_INPUT_PIN), 0, 1023, 0, 100) > IR_MAP_TRIP_VAL) && (tripTime == -10) ) { 
        tripTime = micros();
    //when tripped and expecting second value
    } else if ( (tripTime != -10) && (exitTime == -10) && (map(analogRead(_AMMO_COUNTING_INPUT_PIN), 0, 1023, 0, 100) < IR_MAP_TRIP_VAL) )  {
        exitTime = micros();
        initDisplayChronoValues(calculateChronoReadings(tripTime, exitTime));

        countAmmo();  
        
    //when no second value within 1 second
    } else if ( (tripTime != -10) && (tripTime + 2000000) < micros() ) {
        resetChronoVals();
        initDisplayChronoValues(-1);
    } 
}

void changeMags() {
	//make sure the magazine insertion detection button is pressed from not being pressed
    if (magInsertionDetectionButton.isPressed()) {   //when the magazine is inserted
        currentAmmo = maxAmmo;  //set current ammo to the max amount of ammo
        displayAmmo();  //display ammo
	}
}

void toggleMags() {
	//check if the magazine toggle button is pressed
  if (magSizeToggleButton.isPressed()) {
      //make sure the value doesn't overflow:
      //if the we're trying to access the 10th element of the array, but there are only 9 elements, the program will break
        //must keep the value trying to access is within the amount of values there are. 
        if (currentMagSize < ((sizeof(magSizeArr)/sizeof(magSizeArr[0])) - 1) ) {
            currentMagSize ++;  //change current magazine size
        } else {  
            currentMagSize = 0;
        }

        //there's a new max ammo, because there's a new magazine size
        maxAmmo = magSizeArr[currentMagSize];
        currentAmmo = maxAmmo;

        displayAmmo();    //display the maxAmmo

	} 
}

void ammoCounter () {
	if (!_isIRGate) {
		if (ammoCountingButton.isPressed()) {
			countAmmo();
		}
	} else {
		if (map(analogRead(_AMMO_COUNTING_INPUT_PIN), 0, 1023, 0, 100) > IR_MAP_TRIP_VAL) {
			countAmmo();
		}
	}
}

void countAmmo () {
	//count ammo stuff
    //make sure that the ammo is less than 99 so it doesnt overflow the display
    //make sure it's in increment mode
    if ( (magSizeArr[currentMagSize] == 0) && (currentAmmo < 99) ) {
        currentAmmo++;    //increment ammo
    
    //make sure that the ammo is more than 0 so no negative numbers are displayed
    //make sure it's in increment mode
    } else if ( (currentAmmo > 0) && (magSizeArr[currentMagSize] != 0) ){
        currentAmmo--;    //decrement ammo
    }

    displayAmmo();    //display the ammo  
}

void voltMeter () {
	//make sure only prints every .5 sec
    if (millis() >= lastVoltageCheckTime + delayTime) {
        //calculate voltage
        float voltageIn = ((analogRead(_VOLTMETER_INPUT_PIN) * 5.0) / 1024.0) / (R2/ (R1 + R2));
    
        //make sure voltage is above 0.03, since it could be an error
        if (voltageIn < 0.5) {
            voltageIn = 0; 
        }

        displayVoltage(voltageIn);

        lastVoltageCheckTime = millis();
	}
}

void fireModeMotorControl() {
	boolean canShoot = false, wasDartFired = false;    //flags enabling shooting    
    //check trigger switch was pressed
    if (toggleSelectFireButton.isPressed()) {
        wasDartFired = true;
    }
    
    if ((fireMode == 1 || fireMode == 2) && wasDartFired == true) {    // if in burst mode or single shot mode, and if trigger pressed
        //based on the fire mode (single shot or burst), we will only fire a certain number if shots
        byte modeAmmoIndex;
        if (fireMode == 1) {
            modeAmmoIndex = 1;
        } else {
            modeAmmoIndex = 3;
        }

        //make sure haven't fired more than 1 or 3 shots, depending on the fire mode
        if ((currentAmmo - modeAmmoIndex) < (lastAmmo - 1)) {   
            //if haven't fired more than 1/3 shot, depending on fireMode, still can shoot more
            canShoot = true;
            lastAmmo = currentAmmo;
        } else {
            //if fired more than 1 or 3 shots, depending on mode, can't shoot anymore, and wait for next time trigger pressed
            canShoot = false;
            wasDartFired = false;
        }
    } else if (fireMode == 3) { //if in full auto
        //make sure trigger switch pressed/blaster fired
        if (_isIRGate) {
        	if (map(analogRead(_AMMO_COUNTING_INPUT_PIN), 0, 1023, 0, 100) > IR_MAP_TRIP_VAL) {
				canShoot = true;
			}
		} else {
			if (ammoCountingButton.isPressed()) {
            	canShoot = true;
        	}
		}
        
    } else {    //if not in fully auto, single shot, or burst
        //can't shoot
        canShoot = false;
    }

    //using the logic above, determine whether to shoot
    if (canShoot) {
        digitalWrite(_SELECT_FIRE_OUTPUT_PIN, HIGH);
    } else {
        digitalWrite(_SELECT_FIRE_OUTPUT_PIN, LOW);
	}
}

void toggleFireModeControl () {
	if (toggleSelectFireButton.isPressed()) {
		if (fireMode < 3) {
			fireMode++;
		} else {
			fireMode = 0;
		}

		initDisplayFireMode();

	}
}

void smartMyBlaster () {
	toggleMags();
	changeMags();

	if (!_isChrono) {
		ammoCounter();
	} else {
		chrono();
	}

	if (_isVoltmeter) {
		voltMeter();
	}

	if (_isSelectFire) {
		fireModeMotorControl();
		toggleFireModeControl();

	}
}


