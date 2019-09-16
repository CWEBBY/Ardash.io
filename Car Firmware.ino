#include <RTClib.h>
#include <LiquidCrystal.h>
#include <Wire.h>
//Pins
const int pinRPM = 0, pinIllumination = 1, pinModeButton = 2; //Inputs
const int pinContrast = 7, pinBacklight = 6, pinLCDE = 50, pinLCDRS = 52, pinLCDD4 = 42, pinLCDD5 = 44, pinLCDD6 = 46, pinLCDD7 = 48; //Screen
const int pinRGBLights = 8, pinLightsR = 3, pinLightsG = 4, pinLightsB = 5; //RGB
const int pinRTCPos = 19, pinRTCNeg = 18; //RGB Relay
//Vars
//General
bool bScreenOff = false;
bool bPreviousButtonState = false;
int iMode = 0;
int iModeHoldCount = 0;
bool bButtonContact = false;
int iIntro = 0;
//Clock
int clockCount = 0;
int iMic = 0;
int iHr = 0;
int iMin = 0;
int iSec = 0;
int iDay = 0;
int iMon = 0;
int iYear = 0;
String sCount = "";
String sMer = "am";
String sHr = "";
String sMin = "";
String sSec = "";
String sDay = "";
String sMon = "";
String sYear = "";
//Tachometer
String sRPMString = "";
volatile byte half_revolutions;
unsigned int rpm;
unsigned long timeold;
byte zero[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte one[8] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
};
byte two[8] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
};
byte three[8] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
};
byte four[8] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
};
byte five[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte six[8] = {
  B01010,
  B01010,
  B11111,
  B01010,
  B01010,
  B00010,
  B00100,
  B01000
};
byte seven[8] = {
  B01010,
  B01010,
  B01010,
  B01010,
  B01010,
  B00010,
  B00100,
  B01000
};
byte eight[8] = {
  B00000,
  B00000,
  B00000,
  B10000,
  B11111,
  B00000,
  B00000,
  B00000
};
//Lights
int iLightsMode = 0;
int iLightsTimer = 0, iLightsSeq = 0;

LiquidCrystal lcd(pinLCDRS, pinLCDE, pinLCDD4, pinLCDD5, pinLCDD6, pinLCDD7);
RTC_DS1307 rtc;

void checkMode() {
  if (digitalRead(pinModeButton) == HIGH) {
    if (bPreviousButtonState == false) {
      iModeHoldCount = millis();
      bPreviousButtonState = true;
    }
    else {
      
    }
  }
  else {
    if (bPreviousButtonState == true) {
      iModeHoldCount = millis() - iModeHoldCount;
      if (iModeHoldCount >= 100 && iModeHoldCount < 500) {
        //Do things in different modes
        if (iMode == 0) {
          
        }
        else if (iMode == 1) {
          
        }
        else if (iMode == 2) {
          //Change Light Mode
          if (iLightsMode < 8) {
            iLightsMode++;
          }
          else {
            iLightsMode = 0;
          }
        }
        else if (iMode == 3) {
          iMode = 0;
        }
      }
      else if (iModeHoldCount >= 500) {
        //Different States
        if (iMode == 0) {
          iMode = 1;
          showIntro(iMode);
        }
        else if (iMode == 1) {
          iMode = 2;
          showIntro(iMode);
        }
        else if (iMode == 2) {
          iMode = 3;
          showIntro(iMode);
        }
        else if (iMode == 3) {
          iMode = 0;
          showIntro(iMode);
        }
      }
      bPreviousButtonState = false;
    }
  }  
}

void showIntro(int i) {
  if (i == 0) { //Time
    lcd.clear();
    //
    lcd.setCursor(0, 0);
    lcd.print("    Time        ");
    lcd.setCursor(0, 1);
    lcd.print("        Date    ");
    //
    delay(1000);
    lcd.clear();
  }
  else if (i == 1) { //RPM
    lcd.clear();
    //
    lcd.setCursor(3, 0);
    lcd.print("Tachometer");
    //
    delay(1000);
    lcd.clear();
  }
  else if (i == 2) { //Lights
    lcd.clear();
    //
    lcd.setCursor(0, 0);
    lcd.print(" RGB. Eyelights ");
    //
    delay(1000);
    lcd.clear();
  }
  else if (i == 3) { //Off
    lcd.clear();
    //
    lcd.setCursor(0, 0);
    lcd.print("    Stand-by     ");
    delay(625);
    delay(125);
    analogWrite(pinBacklight, 127);
    delay(125);
    analogWrite(pinBacklight, 63);
    delay(125);
    analogWrite(pinBacklight, 0);
    //
    lcd.clear();
  }
}

void showMode() {
  //Different Modes
  if (iMode == 0) {
    modeClock();
  }
  else if (iMode == 1) {
    modeRPM();
  }
  else if (iMode == 2) {
    modeRGB();
  }
  else if (iMode == 3) {
    modeStandby();
  }
}

void modeRGB () {
  if (iLightsMode == 0) {//OFF BUT WHITE
    digitalWrite(pinRGBLights, HIGH);
    digitalWrite(pinLightsR, HIGH);
    digitalWrite(pinLightsG, HIGH);
    digitalWrite(pinLightsB, HIGH);    
  }
  else if (iLightsMode == 1) {//WHITE
    digitalWrite(pinRGBLights, LOW);
    digitalWrite(pinLightsR, HIGH);
    digitalWrite(pinLightsG, HIGH);
    digitalWrite(pinLightsB, HIGH);    
  }
  else if (iLightsMode == 2) {//RED
    digitalWrite(pinRGBLights, LOW);
    digitalWrite(pinLightsR, HIGH);
    digitalWrite(pinLightsG, LOW);
    digitalWrite(pinLightsB, LOW);    
  }
  else if (iLightsMode == 3) {//GREEN
    digitalWrite(pinRGBLights, LOW);
    digitalWrite(pinLightsR, LOW);
    digitalWrite(pinLightsG, HIGH);
    digitalWrite(pinLightsB, LOW);    
  }
  else if (iLightsMode == 4) {//BLUE
    digitalWrite(pinRGBLights, LOW);
    digitalWrite(pinLightsR, LOW);
    digitalWrite(pinLightsG, LOW);
    digitalWrite(pinLightsB, HIGH);    
  }
  else if (iLightsMode == 5) {//PURPLE
    digitalWrite(pinRGBLights, LOW);
    digitalWrite(pinLightsR, HIGH);
    digitalWrite(pinLightsG, LOW);
    digitalWrite(pinLightsB, HIGH);    
  }
  else if (iLightsMode == 6) {//YELLOW
    digitalWrite(pinRGBLights, LOW);
    digitalWrite(pinLightsR, HIGH);
    digitalWrite(pinLightsG, HIGH);
    digitalWrite(pinLightsB, LOW);    
  }
  else if (iLightsMode == 7) {//AQUA
    digitalWrite(pinRGBLights, LOW);
    digitalWrite(pinLightsR, LOW);
    digitalWrite(pinLightsG, HIGH);
    digitalWrite(pinLightsB, HIGH);    
  }
  else if (iLightsMode == 8) {//Disco
    if (iLightsTimer < 50) {
      iLightsTimer++;
    }
    else {
      if (iLightsSeq < 7) {
        iLightsSeq++;
      }
      else {
        iLightsSeq = 0;
      }
      iLightsTimer = 0;
    }

    if (iLightsSeq == 0) {
      digitalWrite(pinRGBLights, LOW);
      digitalWrite(pinLightsR, HIGH);
      digitalWrite(pinLightsG, HIGH);
      digitalWrite(pinLightsB, HIGH);    
    }
    else if (iLightsSeq == 1) {
      digitalWrite(pinRGBLights, LOW);
      digitalWrite(pinLightsR, HIGH);
      digitalWrite(pinLightsG, LOW);
      digitalWrite(pinLightsB, LOW);    
    }
    else if (iLightsSeq == 2) {
      digitalWrite(pinRGBLights, LOW);
      digitalWrite(pinLightsR, LOW);
      digitalWrite(pinLightsG, HIGH);
      digitalWrite(pinLightsB, LOW); 
    }
    else if (iLightsSeq == 3) {
      digitalWrite(pinRGBLights, LOW);
      digitalWrite(pinLightsR, LOW);
      digitalWrite(pinLightsG, LOW);
      digitalWrite(pinLightsB, HIGH);
    }
    else if (iLightsSeq == 4) {
      digitalWrite(pinRGBLights, LOW);
      digitalWrite(pinLightsR, HIGH);
      digitalWrite(pinLightsG, LOW);
      digitalWrite(pinLightsB, HIGH); 
    }
    else if (iLightsSeq == 5) {
      digitalWrite(pinRGBLights, LOW);
      digitalWrite(pinLightsR, HIGH);
      digitalWrite(pinLightsG, HIGH);
      digitalWrite(pinLightsB, LOW); 
    }
    else if (iLightsSeq == 6) {
      digitalWrite(pinRGBLights, LOW);
      digitalWrite(pinLightsR, LOW);
      digitalWrite(pinLightsG, HIGH);
      digitalWrite(pinLightsB, HIGH); 
    }
    else if (iLightsSeq == 7) {
      digitalWrite(pinRGBLights, HIGH);
      digitalWrite(pinLightsR, HIGH);
      digitalWrite(pinLightsG, HIGH);
      digitalWrite(pinLightsB, HIGH); 
    }
  }
  lcd.setCursor(0,0);
  lcd.print("RGB Mode:");
  lcd.setCursor(0,1);
  if (iLightsMode == 0) {
    lcd.print("Off             ");
  }
  else if (iLightsMode == 1) {
    lcd.print("White           ");
  }
  else if (iLightsMode == 2) {
    lcd.print("Red             ");
  }
  else if (iLightsMode == 3) {
    lcd.print("Green           ");
  }
  else if (iLightsMode == 4) {
    lcd.print("Blue            ");
  }
  else if (iLightsMode == 5) {
    lcd.print("Purple          ");
  }
  else if (iLightsMode == 6) {
    lcd.print("Yellow          ");
  }
  else if (iLightsMode == 7) {
    lcd.print("Aqua            ");
  }
  else if (iLightsMode == 8) {
    lcd.print("Rave (^m^)      ");
  }
}

void modeStandby () {//IF iMode CHANGES, CHECK ILLUMINATION NEEDS TO CHANGE
  digitalWrite(pinBacklight, LOW);
}

void modeClock() {
  //Stuff
  String zero = "0";
  String space = " ";
  //AM/PM
  if (iHr < 12) {
    sMer = "am";
  }
  else {
    sMer = "pm";
  }
  //
  //12 Hours Formattting
  if (iHr == 0) {
    iHr = 12;
  }
  if (iHr > 12) {
    iHr = iHr - 12;    
  }
  //Meridian needed.
  //
  //Seconds formatting.
  if (iSec < 10) {
    sSec = zero + iSec;
  }
  else {
    sSec = iSec;
  }
  //
  //Minutes formatting
  if (iMin < 10) {
    sMin = zero + iMin;
  }
  else {
    sMin = iMin;
  }
  //
  //Hours formatting
  if (iHr < 10) {
    sHr = space + iHr;    
  }
  else {
    sHr = iHr;
  }
  //

  //Day formatting
  if (iDay < 10) {
    sDay = zero + iDay;
  }
  else {
    sDay = iDay;
  }

  //Month formatting
  if (iMon == 1) {
    sMon = "Jan";
  }
  else if (iMon == 2) {
    sMon = "Feb";
  }
  else if (iMon == 3) {
    sMon = "Mar";
  }
  else if (iMon == 4) {
    sMon = "Apr";
  }
  else if (iMon == 5) {
    sMon = "May";
  }
  else if (iMon == 6) {
    sMon = "Jun";
  }
  else if (iMon == 7) {
    sMon = "Jul";
  }
  else if (iMon == 8) {
    sMon = "Aug";
  }
  else if (iMon == 9) {
    sMon = "Sep";
  }
  else if (iMon == 10) {
    sMon = "Oct";
  }
  else if (iMon == 11) {
    sMon = "Nov";
  }
  else if (iMon == 12) {
    sMon = "Dec";
  }
  //

  if (clockCount < 100) { //Meant to be 1000 but delay somewhere effects it.
    clockCount++;
  }
  else {
    clockCount = 0;
  }

  if (clockCount < 50) {
      sCount = ":";
    }
    else {
      sCount = " ";
    }

  //Year formatting
  sYear = iYear;
  //
  lcd.setCursor(0, 0);
  String timeStr = String("    ") + String(sHr) + sCount + String(sMin) + space + String(sMer) + String("    ");
  String dateStr = String("  ") + String(sDay) + String(" ") + String(sMon) + String(", ") + String(sYear) + String("  ");
  lcd.print(timeStr);
  lcd.setCursor(0, 1);
  lcd.print(dateStr);
}

void backgroundClock() {
  DateTime now = rtc.now();
  iSec = now.second();
  iMin = now.minute();
  iHr = now.hour();
  iDay = now.day();
  iMon = now.month();
  iYear = now.year();
}

void checkIllumination() {
  if (iMode != 3) {
    if (bScreenOff == false) {
      if (iHr > 18 || iHr <= 6) {
        analogWrite(pinBacklight, 15);
      }
      else {
        analogWrite(pinBacklight, 255);
      }
    }
    else {
      analogWrite(pinBacklight, 0);
    }
  }
}

void addRPM() {
  half_revolutions++;
}

void modeRPM() {
  if (half_revolutions >= 5) { 
     //Update RPM every 20 counts, increase this for better RPM resolution,
     //decrease for faster update
     rpm = (30*1000/(millis() - timeold)*half_revolutions);
     timeold = millis();
     half_revolutions = 0;
     //LCD Print
     lcd.setCursor(4, 0);
     if (rpm > 8000) {
      sRPMString = String("!!!!") + String(" rpm");
     }
     else if (rpm < 1000) {
      sRPMString = String(" ") + String(rpm) + String(" rpm");
     }
     else if (rpm < 100) {
      sRPMString = String("  ") + String(rpm) + String(" rpm");
     }
     else if (rpm < 10) {
      sRPMString = String("   ") + String(rpm) + String(" rpm");
     }
     else if (rpm == 0) {
      sRPMString = String("----") + String(" rpm");
     }
     else {
      sRPMString = String(rpm) + " rpm";
     }
     lcd.print(sRPMString);
     lcd.setCursor(0, 1);
     if (rpm > 0 && rpm < 100) {
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 100 && rpm < 200) {
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 200 && rpm < 300) {
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 300 && rpm < 400) {
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 400 && rpm < 500) {
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 500 && rpm < 600) {
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 600 && rpm < 700) {
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 700 && rpm < 800) {
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 800 && rpm < 900) {
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 900 && rpm < 1000) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 1000 && rpm < 1100) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 1100 && rpm < 1200) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 1200 && rpm < 1300) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 1300 && rpm < 1400) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 1400 && rpm < 1500) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 1500 && rpm < 1600) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 1600 && rpm < 1700) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 1700 && rpm < 1800) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 1800 && rpm < 1900) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 1900 && rpm < 2000) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 2000 && rpm < 2100) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 2100 && rpm < 2200) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 2200 && rpm < 2300) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 2300 && rpm < 2400) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 2400 && rpm < 2500) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 2500 && rpm < 2600) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 2600 && rpm < 2700) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 2700 && rpm < 2800) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 2800 && rpm < 2900) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 2900 && rpm < 3000) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 3000 && rpm < 3100) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 3100 && rpm < 3200) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 3200 && rpm < 3300) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 3300 && rpm < 3400) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 3400 && rpm < 3500) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 3500 && rpm < 3600) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 3600 && rpm < 3700) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 3700 && rpm < 3800) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 3800 && rpm < 3900) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 3900 && rpm < 4000) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 4000 && rpm < 4100) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 4100 && rpm < 4200) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 4200 && rpm < 4300) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 4300 && rpm < 4400) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 4400 && rpm < 4500) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 4500 && rpm < 4600) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 4600 && rpm < 4700) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 4700 && rpm < 4800) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 4800 && rpm < 4900) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 4900 && rpm < 5000) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 5000 && rpm < 5100) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 5100 && rpm < 5200) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 5200 && rpm < 5300) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 5300 && rpm < 5400) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 5400 && rpm < 5500) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 4500 && rpm < 5600) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 5600 && rpm < 5700) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 5700 && rpm < 5800) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 5800 && rpm < 5900) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 5900 && rpm < 6000) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 6000 && rpm < 6100) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 6100 && rpm < 6200) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 6200 && rpm < 6300) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 6300 && rpm < 6400) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 6400 && rpm < 6500) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 6500 && rpm < 6600) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 6600 && rpm < 6700) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 6700 && rpm < 6800) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 6800 && rpm < 6900) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 6900 && rpm < 7000) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 7000 && rpm < 7100) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 7100 && rpm < 7200) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 7200 && rpm < 7300) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 7300 && rpm < 7400) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 7400 && rpm < 7500) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
     else if (rpm > 7500 && rpm < 7600) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(1));
      lcd.print("               ");
     }
     else if (rpm >= 7600 && rpm < 7700) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(2));
      lcd.print("              ");
     }
     else if (rpm >= 7700 && rpm < 7800) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(3));
      lcd.print("             ");
     }
     else if (rpm >= 7800 && rpm < 7900) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(4));
      lcd.print("            ");
     }
     else if (rpm >= 7900 && rpm < 8000) {
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.print("           ");
     }
   } 
}

void setup() {
  Wire.begin();
  rtc.begin();
  pinMode(pinRTCPos, OUTPUT);
  pinMode(pinRTCNeg, OUTPUT);
  pinMode(pinBacklight, OUTPUT);
  pinMode(pinContrast, OUTPUT);
  pinMode(pinRelayPos, OUTPUT);
  pinMode(pinRGBLights, OUTPUT);
  pinMode(pinLightsR, OUTPUT);
  pinMode(pinLightsG, OUTPUT);
  pinMode(pinLightsB, OUTPUT);
  pinMode(pinLightsSwitch, INPUT);
  pinMode(pinRPM, INPUT);
  pinMode(pinModeButton, INPUT);
  pinMode(pinIllumination, INPUT);
  analogWrite(pinBacklight, 0);
  analogWrite(pinContrast, 127);
  digitalWrite(pinRelayPos, HIGH);
  digitalWrite(pinRGBLights, LOW); //LOW MEANS ON IN THIS CASE,HIGH MEANS OFF
  digitalWrite(pinLightsR, HIGH);//HIGH MEANS ON, LOW MEANS OFF!
  digitalWrite(pinLightsB, LOW);  
  digitalWrite(pinLightsB, LOW);  
  digitalWrite(pinRTCPos, HIGH);
  digitalWrite(pinRTCNeg, LOW);
  attachInterrupt(digitalPinToInterrupt(3), addRPM, RISING);
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
  half_revolutions = 0;
  rpm = 0;
  timeold = 0;
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("99 J-Spec 4G93-R");
  lcd.setCursor(0,1);
  lcd.print("");
  lcd.write(char(0xBB));
  lcd.write(char(0xD8));
  lcd.write(char(0xB0));
  //lcd.print("");
  //Fade in
  analogWrite(pinBacklight, 7);
  delay(125);
  analogWrite(pinBacklight, 15);
  delay(125);
  analogWrite(pinBacklight, 23);
  delay(125);
  analogWrite(pinBacklight, 31);
  
  delay(125);
  analogWrite(pinBacklight, 39);
  delay(125);
  analogWrite(pinBacklight, 47);
  delay(125);
  analogWrite(pinBacklight, 55);
  delay(125);
  analogWrite(pinBacklight, 63);
  delay(125);
  
  analogWrite(pinBacklight, 71);
  delay(125);
  analogWrite(pinBacklight, 79);
  delay(125);
  analogWrite(pinBacklight, 87);
  delay(125);
  analogWrite(pinBacklight, 95);
  
  delay(125);
  analogWrite(pinBacklight, 103);
  delay(125);
  analogWrite(pinBacklight, 111);
  delay(125);
  analogWrite(pinBacklight, 129);
  delay(125);
  analogWrite(pinBacklight, 127);
  
  delay(125);
  analogWrite(pinBacklight, 135);
  delay(125);
  analogWrite(pinBacklight, 143);
  delay(125);
  analogWrite(pinBacklight, 151);
  delay(125);
  analogWrite(pinBacklight, 159);
  
  delay(125);
  analogWrite(pinBacklight, 167);
  delay(125);
  analogWrite(pinBacklight, 175);
  delay(125);
  analogWrite(pinBacklight, 183);
  delay(125);
  analogWrite(pinBacklight, 191);
  
  delay(125);
  analogWrite(pinBacklight, 199);
  delay(125);
  analogWrite(pinBacklight, 207);
  delay(125);
  analogWrite(pinBacklight, 215);
  delay(125);
  analogWrite(pinBacklight, 223);
  
  delay(125);
  analogWrite(pinBacklight, 231);
  delay(125);
  analogWrite(pinBacklight, 239);
  delay(125);
  analogWrite(pinBacklight, 247);
  delay(125);
  analogWrite(pinBacklight, 255);
  delay(125);
  //
  digitalWrite(pinRGBLights, HIGH);
  //Set To white light mode here
  lcd.clear();
}

void loop() {
  backgroundClock();
  checkIllumination();
  checkMode();
  showMode();
}

