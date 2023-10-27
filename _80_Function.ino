
// ---------------------------- to the keyboard
void keyControl(char customKey){

    saveTeEpprom(13);
  if(!Screen.lcdStateOn){ Screen.lcdOn(); return; }
    
  if(customKey == '1'){ Serial.println(F("Keyboard [1]")); Screen.controlMenu(1); } /*  up    */
  if(customKey == '2'){ Serial.println(F("Keyboard [2]")); Screen.controlMenu(0); } /*  down  */
  if(customKey == '3'){ Serial.println(F("Keyboard [3]")); Screen.enter(); }        /*  enter */
  if(customKey == 'A'){ Serial.println(F("Keyboard [A]")); Screen.lcdOn();          /*  exit  */
    if(Screen.lcdStateOn){ Screen.exit(); }
  }
}
//******************************************************

// ---------------------------- to the wind speed sensor
#define WIND_FACTOR 2.4                                     // for km/h  - #define WIND_FACTOR 1.492 // for mp/h
#define ANEMOMETER_PIN 3
#define ANEMOMETER_INT 2
 
volatile unsigned long anem_count=0;
volatile unsigned long anem_last = 0;
volatile unsigned long anem_min = 0xffffffff;
float lastWindCheck;
 
void setupAnemometerInt()
{
    // saveTeEpprom(14);
    delayMicroseconds(100);   //50
    pinMode(ANEMOMETER_INT,INPUT_PULLUP);                   // setting the pin that will count impulses
                                                            // setting which function will be activated to count impulses
    attachInterrupt(digitalPinToInterrupt(ANEMOMETER_INT),anemometerClick,RISING);
                                                            // setting which function will be activated to count impulses
    //  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_INT),anemometerClick,FALLING);
    interrupts();                                           // starting the interuptor operation
}

int get_wind_speed()
{
    saveTeEpprom(15);
    float deltaTime = (millis() - lastWindCheck) / 1000;    // time since the last speed check, and converts to seconds
    float windSpeed = (float)anem_count / deltaTime;        // calculating anemometer clicks in a specific period of time (per second)
    anem_count = 0;                                         // resets the number of clicks counted
    //  windSpeed = (int)(windSpeed * WIND_FACTOR);         // wind speed calculation
    lastWindCheck = millis();

    saveTeEpprom(20);

    return((int)(windSpeed * WIND_FACTOR));                 // the function returns the wind speed in the selected unit (km/h)
}
 
void anemometerClick()
{
    // saveTeEpprom(16);
    // todo - improve the calculation method for thisTime
    long thisTime = micros() - anem_last;
    anem_last = micros();

    if(thisTime>500){                                       // lock so that it does not run the counting several times
        anem_count++;
        if(thisTime < anem_min) { anem_min = thisTime; }    // todo - check if it is needed
    }
}
