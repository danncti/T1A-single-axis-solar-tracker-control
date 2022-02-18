


// ---------------------------- do klawiatury
void keyControll(char customKey){

    saveTeEpprom(13);
  if(!Screen.lcdStateOn){ Screen.lcdOn(); return; }
    
  if(customKey == '1'){ Serial.println(F("Keyboard [1]")); Screen.controlMenu(1); } /*  up    */
  if(customKey == '2'){ Serial.println(F("Keyboard [2]")); Screen.controlMenu(0); } /*  down  */
  if(customKey == '3'){ Serial.println(F("Keyboard [3]")); Screen.enter(); }        /*  enter */
  if(customKey == 'A'){ Serial.println(F("Keyboard [A]")); Screen.lcdOn();          /*  exit  */
    if(Screen.lcdStateOn){ Screen.exit(); }
  }
}
//*********************************************************************************

// -------------------------------------------- do czujnika prędkości wiatru
#define WIND_FACTOR 2.4                                                     // for km/h  - #define WIND_FACTOR 1.492 // for mp/h
#define ANEMOMETER_PIN 3
#define ANEMOMETER_INT 2
 
volatile unsigned long anem_count=0;
volatile unsigned long anem_last = 0;
volatile unsigned long anem_min = 0xffffffff;
float lastWindCheck;
 
void setupAnemometerInt()
{
//    saveTeEpprom(14);
  delayMicroseconds(100);   //50
  pinMode(ANEMOMETER_INT,INPUT_PULLUP);                                     // ustawienie pinu który będzie zliczał impulzy
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_INT),anemometerClick,RISING); // ustawienie która funkcja będzie uruchamiana do zliczania impulsów
//  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_INT),anemometerClick,FALLING); // ustawienie która funkcja będzie uruchamiana do zliczania impulsów
  interrupts();                                                             // uruchomienie działania interuptora
}

int get_wind_speed()
{
    saveTeEpprom(15);
    float deltaTime = (millis() - lastWindCheck) / 1000;                      // czas od ostatniego sprawdzenia prędkości, i konwertuje na sekundy
    float windSpeed = (float)anem_count / deltaTime;                          // wyliczanie kliknięć anemometru w określonym odcinku czasu (na sekundę)
    anem_count = 0;                                                           // resetuje ilość zliczonych kliknięć
    //  windSpeed = (int)(windSpeed * WIND_FACTOR);                                                 // wyliczanie prędkości wiatru
    lastWindCheck = millis();

    saveTeEpprom(20);

    return((int)(windSpeed * WIND_FACTOR));                                                        // funkcja zwraca prędkość wiatru w wybranej jednostce (km/h)
}
 
void anemometerClick()
{
//    saveTeEpprom(16);
    // todo poprawic sposob wyliczania thisTime
    long thisTime = micros() - anem_last;
    anem_last = micros();

    if(thisTime>500){                                                        // blokada do tego żeby nie uruchomiało liczenia kilka razy
        anem_count++;
        if(thisTime < anem_min) { anem_min = thisTime; }                      // TODO   sprawdzić czy to potrzebne
    }
}
