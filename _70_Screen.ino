
#include <LiquidCrystal_I2C.h>

// Simple I2C test for ebay 128x64 oled.
// Use smaller faster AvrI2c class in place of Wire.
// Edit AVRI2C_FASTMODE in SSD1306Ascii.h to change the default I2C frequency.

#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1


// class for menu and LCD display operation
class T1screen {

    public:

    bool lcdStateOn;                        // true if lcd is on
    bool lcdConnected;
    byte displayType;
    unsigned long previousMillisScreenOff, currentMillisScreenOff;
    // unsigned int refreshLcd;
    byte menuMItems;
    byte menuLevel, position2, position3;
    bool enterPressed, confirm;
    byte position[3] = {0, 0, 0};
    int newValue;                           // new value that will be saved to memory
    // todo - can change type to - byte.

    byte changeStep;

    #define Interval   120000               // time in milliseconds after which the LCD backlight will turn off

                                            // variables to functions refreshScreen()
    #define RefreshScreen 1000              // time in milliseconds after which the LCD refreshes
    unsigned long previousMillisScreenRefresh = 0;
    // ----------

    LiquidCrystal_I2C lcd;

    SSD1306AsciiAvrI2c oled;

    T1screen::T1screen():lcd(0x27,20,4){ }

    void T1screen::setup()
    {
        saveTeEpprom(21);

        this->displayType = 0;
        this->lcdStateOn = true;
        Serial.println(F(" 8 setup() "));

        this->menuReset();

        this->menuMItems = sizeof(position) - 1;  // a variable that indicates the number of items in the main menu
    }

    void T1screen::lcdSetup()
    {
        saveTeEpprom(22);

        Serial.println(F("101 lcdSetup "));

        switch(this->displayType){
            case 0:

            break;

            case 1: // LCD 20 x 4
                Serial.println(F("109 1 lcdSetup "));
                this->lcd.init();           // initialize the lcd
                this->lcdOn();
            break;

            case 2: // oled
                Serial.println(F("109 1 lcdSetup "));
                #if RST_PIN >= 0
                oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
                #else // RST_PIN >= 0
                oled.begin(&Adafruit128x64, I2C_ADDRESS);
                #endif // RST_PIN >= 0
                // Call oled.setI2cClock(frequency) to change from the default frequency.

                oled.setFont(System5x7);
                oled.clear();
                oled.print("Setup.");

                delay(300);
                oled.print(F("."));
                delay(300);
                oled.print(F("."));
                delay(300);
                oled.print(F("."));
            break;
        }
    }

    // the function returns true if there is time to refresh the screen
    bool T1screen::refreshScreen()
    {
        if(this->lcdStateOn)
        {
            unsigned long currentMillis = millis();  // Get snapshot of time  ********** correct way to use counters

            if ((unsigned long)(currentMillis - this->previousMillisScreenRefresh) >= RefreshScreen)  // Refresh lcd
            {
                this->previousMillisScreenRefresh = currentMillis;
                return true;
            }
        }

        return false;
    }

    void T1screen::menuSettings()
    {
        saveTeEpprom(26);

        switch(this->menuLevel){
            case 0:
                this->lcdClear();
                this->write_LCD(0,1, F("  Settings") );
                this->downMenuLine();
            break;

            case 1:  // list of available settings
                this->settingsL2();
            break;

            case 2:
                this->settingsL3();
            break;
        }

    }

    void T1screen::enter()  // function performed after pressing the Enter button
    {
        saveTeEpprom(27);

        switch(this->position[0]){
            case 0:
            break;

            case 1:
                if(this->menuLevel == 1) this->enterPressed = true;
                this->menuLevel = 1;
                // Serial.print(F(" enter ")); Serial.println(this->menuLevel);
                // Serial.print(F(" value ")); Serial.println(value);
            break;

            case 2:
                if(this->menuLevel < 2) {
                    this->menuLevel++;
                    this->position[this->menuLevel] = 0;

                    if(this->menuLevel == 2){
                        // entering the settings change menu - downloading current data and change step
                        this->newValue = Settings.get(this->position[1]);
                        this->changeStep = Settings.settingsX[this->position[1]][1];
                    }
                }else{
                    Settings.save(
                    Settings.settingsX[this->position[1] ][3],
                    this->  newValue  + Settings.settingsX[this->position[1] ][2]
                    );

                    Settings.set(
                        this->position[1],
                        this->newValue  + Settings.settingsX[this->position[1]][2]
                    );

                    Relay.setRelaysData();
                    // todo - can you add a confirmation of the save????
                }
            break;
        }
        this->printMenu();
    }

    void T1screen::exit()
    {
        saveTeEpprom(28);

        this->newValue = 0;
        this->changeStep = 0;

        if(this->menuLevel > 0){this->menuLevel--;} else this->position[0] = 0;

        switch(this->position[0]){
            case 0:
            break;

            case 1:
                this->position[0] = 0;
            break;

            case 2:
                this->position[1] = 0;
            break;
        }
        this->printMenu();
    }

    void T1screen::controlMenu(byte direct)
    {
        saveTeEpprom(29);

        this->previousMillisScreenOff = millis();

        if(direct)                          // menu down, next  **********
        {
            switch(this->menuLevel){
                case 0:  // level
                    this->position[0] ++;
                    if(this->position[0] > this->menuMItems){ this->position[0] = 0; }
                break;

                case 1:
                    this->position[this->menuLevel]++;

                    if(this->position[0] == 2){
                        if(this->position[this->menuLevel] > Settings.ROWS - 1){ this->position[this->menuLevel] = 0;}
                    }else{
                        if(this->position[this->menuLevel] > 2){ this->position[this->menuLevel] = 0;}
                    }

                    Serial.print(F(" 224 down ")); Serial.println(this->position[this->menuLevel]);
                break;

                case 2:
                    Serial.print(F(" 223 settings get ")); Serial.println(254 - Settings.settingsX[this->position[1]][2]);

                    if ((this->newValue + this->changeStep) < 254 - Settings.settingsX[this->position[1]][2] ){
                        this->newValue += this->changeStep;
                    }
                break;
            }
        }else{
            switch(this->menuLevel){        // next menu position  **********
                case 0:

                    if(this->position[0] > 0){
                        this->position[0]--;
                    }else{
                        this->position[0] = this->menuMItems;
                    }
                break;

                case 1:
                    if(this->position[0] == 2){
                        if(this->position[this->menuLevel] == 0){ this->position[this->menuLevel] = Settings.ROWS - 1;}
                        else if(this->position[this->menuLevel] > 0) this->position[this->menuLevel]--;
                    }else{
                        if(this->position[this->menuLevel] == 0) { this->position[this->menuLevel] = 2;}
                        else if(this->position[this->menuLevel] > 0) this->position[this->menuLevel]--;
                    }

                    Serial.print(F(" 254 down ")); Serial.println(this->position[this->menuLevel]);
                break;

                case 2:
                    Serial.print(F(" 255 settings get ")); Serial.println(254 - Settings.settingsX[this->position[1]][2]);

                    this->newValue -= this->changeStep;
                    if(this->newValue < 0 - Settings.settingsX[this->position[1]][2]){
                        this->newValue = Settings.settingsX[this->position[1]][2];
                    }
                break;
            }
        }
        this->printMenu();
    }

    void T1screen::menuOther()
    {
        saveTeEpprom(3);

        // Serial.println(F(" 268 menuOther : "));
        if(this->menuLevel == 0){
            this->write_LCD(0,1, F("Other settings") );
            this->downMenuLine();
        }else{
            if(this->position[1] > 2) this->position[1] = 0;

            String menus[3] =
            {
                { "Memory Reset" },
                { "Set Panels Level" },
                { "Test Mode" },
            };

            switch(this->position[1]){                        // After entering the selected MENU item, a confirmation message is displayed
                case 0:
                    this->write_LCD(0,1, menus[0] );

                    if(this->confirm && this->enterPressed){  // after confirmation, this code is executed

                        Settings.memoryReset();
                        this->confirm = false;
                        this->enterPressed = false;
                    }
                break;

                case 1:
                    this->write_LCD(0,1, menus[1] );

                    if(this->confirm && this->enterPressed){  // after confirmation, this code is executed

                        Serial.println(F(" set level confirmed "));

                        Relay.settingLevel = true;            // the installation is adjusted to the level
                        this->confirm = false;
                        this->enterPressed = false;

                        this->position[1] = 0;
                        this->position[0] = 0;
                        this->menuLevel = 0;
                        this->printMenu();
                        return;
                    }
                break;

                case 2:
                    this->write_LCD(0,1, menus[2] );

                    if(Settings.testMode){
                        this->write_LCD(10,1, F("ON") );
                    }else{
                        this->write_LCD(10,1, F("OFF") );
                    }

                    if(this->confirm && this->enterPressed){  // after confirmation, this code is executed

                        this->confirm = false;
                        this->enterPressed = false;

                        if(Settings.testMode){
                            Settings.testMode = false;        // test mode is activated, i.e., among other things, different function operation times
                        }else{
                            Settings.testMode = true;         // test mode is activated, i.e., among other things, different function operation times
                        }

                        Relay.setRelaysData();
                        this->printMenu();
                    }
                break;
            }

            this->downMenuLine();

            if(this->enterPressed){
                this->write_LCD(3,2, F("Enter to Confirm") );
                this->confirm = true;
            }
            this->enterPressed = false;
        }
    }

    void T1screen::printMenu()
    {
        saveTeEpprom(31);
        this->lcdClear();
        Serial.println(F("373 printMenu clear() "));

        delay(100);

        this->previousMillisScreenOff = millis();
        switch(this->position[0]){
            case 0:
                this->menuCurrent();
            break;

            case 1:
                this->menuOther();
            break;

            case 2:
                this->menuSettings();
            break;
        }
    }

    void T1screen::lcdOn()  // turning on the LCD backlight
    {
        saveTeEpprom(32);
        switch(this->displayType){
            case 0:
            break;

            case 1: // LCD 20 x 4
                this->lcd.backlight();
            break;

            case 2: // oled
            break;
        }

        this->lcdClear();
        this->lcdStateOn = true;
        this->previousMillisScreenOff = millis();
        this->currentMillisScreenOff = millis();
    }

    void T1screen::lcdClear()    // turning on the LCD backlight
    {
        saveTeEpprom(33);
        switch(this->displayType){
            case 0:
            break;

            case 1: // LCD 20 x 4
                this->lcd.clear();
            break;

            case 2: // oled
                this->oled.clear();
            break;
        }
    }

    void T1screen::lcdTurnOff()    // turning on the LCD backlight
    {
        saveTeEpprom(34);
        switch(this->displayType){
            case 0:

            break;

            case 1: // LCD 20 x 4
                this->lcd.noBacklight();
            break;

            case 2: // oled
            break;
        }
    }

    void T1screen::lcdOff(bool force)  // turning off the LCD backlight
    {
        saveTeEpprom(35);
        if(force)                      // if true - forced LCD off
        {
            this->lcdTurnOff();
            this->lcdClear();
            this->lcdStateOn = false;
            this->menuReset();
        }

        this->currentMillisScreenOff = millis();  // update with every loop

        if (this->currentMillisScreenOff - this->previousMillisScreenOff >= Interval)
        {
            this->previousMillisScreenOff = this->currentMillisScreenOff;  // new start point
            this->lcdTurnOff();
            this->lcdClear();
            this->lcdStateOn = false;

            this->menuReset();
        }
    }

    void T1screen::menuReset()
    {
        saveTeEpprom(36);
        this->changeStep = 0;
        this->newValue = 0;
        this->position[0] = 0;
        this->position[1] = 0;
        this->position[2] = 0;

        this->menuLevel = 0;
        this->position2 = 1;
        this->position3 = 1;
        this->enterPressed = false;
        this->confirm = false;
    }

    void T1screen::write_LCD(int column, int row, String text )
    {
        // saveTeEpprom(37);
        switch(this->displayType){
            case 0:{
                break;
            }

            case 1:{ // LCD 20 x 4

                this->lcd.setCursor(column, row);
                int x = 1;

                while(text.length() ){
                    this->lcd.print(text.substring(0,1));
                    text.remove(0,1);

                    delay(2);
                    x++;
                    if(x>20){
                        row++;
                        if (row > 3) row = 0;
                        this->lcd.setCursor(0, row );
                        x = 1;
                    }
                }
                break;}

            case 2:{ // oled
                // Serial.println(F("453 OLED 128x64 "));
                this->oled.setCursor((column * 6), row);

                if(text.length() > 20){
                    int x = 1;
                    while(text.length() ){

                        this->oled.print(text.substring(0,1));
                        text.remove(0,1);

                        delay(2);
                        x++;
                        if(x>20){
                            row++;
                            if (row > 7) row = 0;
                            this->oled.setCursor(0, row );
                            x = 1;
                        }
                    }
                }else this->oled.print(text);
            break;}
        }
    }

    void T1screen::menuCurrent()
    {
        saveTeEpprom(38);
        if(!accelClass.accelConnected){
            this->write_LCD(0,1,F("AccError"));
        } else {
            this->write_LCD(0,1,F("        "));
            this->write_LCD(0,1,String( Relay.accelBalance) );
        }

        if(Settings.testMode)  this->write_LCD(8,1, F("  Test Mode") );

        this->write_LCD(0,0,F("East"));
        this->write_LCD(5,0,F("      "));
        this->write_LCD(5,0,String( (Relay.sensorValueE )));
        this->write_LCD(9,0,F("West"));
        this->write_LCD(15,0,F("      "));
        this->write_LCD(15,0,String( ( Relay.sensorValueW )));

        this->write_LCD(0,5,F("East"));
        this->write_LCD(5,5,F("      "));
        this->write_LCD(5,5,String( analogRead(Relay.pvEpin) ));
        this->write_LCD(9,5,F("West"));
        this->write_LCD(15,5,F("      "));
        this->write_LCD(15,5,String( analogRead(Relay.pvWpin) ));

        if(Relay.windAlarm){
            this->write_LCD(0,2, F("WindAlarm>(km/h)") );
            this->write_LCD(17, 2, (String)(Settings.WindSpeedAlarmKm));
            this->write_LCD(0,6, F("Set Level") );
        } else if(Relay.nightPos){
            this->write_LCD(0,2, F("Night Position-(set)") );
        } else if(Relay.settingLevel){
            this->write_LCD(0,2, F("Set Level") );
        } else {
            this->write_LCD(0,2, F("                    ") );
            this->write_LCD(0,6, F("                    ") );
        }

        this->write_LCD(0,3,String(windSpeedKmh, DEC).substring(0,5) + F(" km/h"));
    }

    void T1screen::settingsL2()
    {
        saveTeEpprom(39);
        this->lcdClear();

        this->write_LCD(0,0, Settings.getSettingsNames(this->position[1], 1));
        this->write_LCD(0,2, F("Value:") );

        this->write_LCD(7,2, String( Settings.get(this->position[1]) , DEC ) ) ;

        this->downMenuLine();
    }

    void T1screen::settingsL3()
    {
        saveTeEpprom(40);
        this->lcdClear();

        this->write_LCD(0,0, Settings.getSettingsNames(this->position[1], 1) );

        this->write_LCD(0,1, F("Cur. Value:") );
        this->write_LCD(12,1, String( Settings.get(this->position[1]) , DEC ) ) ;

        this->write_LCD(0,2, F("New Value:") );
        this->write_LCD(12,2, String( this->newValue , DEC ) ) ;

        this->write_LCD(1,3, String( this->changeStep , DEC ) ) ;
        this->write_LCD(6,3, String( this->changeStep , DEC ) ) ;

        this->downMenuButtons();
        this->downMenuLine();
    }

    void T1screen::downMenuLine()
    {
        saveTeEpprom(41);
        this->write_LCD(10,3, F("Enter") );
        this->write_LCD(16,3, F("Exit") );
    }

    void T1screen::downMenuButtons()
    {
        saveTeEpprom(42);
        this->write_LCD(0,3, F("+") );
        this->write_LCD(5,3, F("-") );
    }

};

T1screen Screen;
