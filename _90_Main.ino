
void loop()
{
    wdt_reset();        // watchdog restart timer
    saveTeEpprom(100);

    // unsigned long currentMillis = millis();            // Get snapshot of time  ********** correct way to use  counters

    if (Screen.refreshScreen())                           // Refresh lcd
    {
        saveTeEpprom(2);
        windSpeedKmh = get_wind_speed();

        if((windSpeedKmh >= Settings.WindSpeedAlarmKm))
        {
            // If the wind speed is greater than the alarm setting, then proceed
            Relay.windAlarmSet();  // the function supports the pause during which sun following is to be disabled after the wind stops
        }

        if(accelClass.accelConnected)
        {
            // todo - can the accelerometer be turned on only when necessary???
            // todo - here was the last save, then the program crashed

            Relay.accelBalance = (accelClass.balancedAcc() /10) + Settings.LevelAdj;
            saveTeEpprom(5);
        }
        else
        {
            accelClass.setup();
        }

        if(Screen.lcdStateOn)
        {
            // saveTeEpprom(45);
            if(Screen.menuLevel == 0 && Screen.position[0] == 0)
            saveTeEpprom(7);
            Screen.menuCurrent(); // todo - temporary solution
            saveTeEpprom(8);
            Screen.lcdOff(false);
        }
        Screen.write_LCD(14, 4, "    ");
        Screen.write_LCD(0, 4, "Last save" );
        Screen.write_LCD(14, 4, (String)(currentAddress - 53));
        saveTeEpprom(46);

        // todo - temporarily turning on the screens permanently - because I don't use the keyboard
        if(!Screen.lcdStateOn)
        {
            Screen.lcdOn();
            saveTeEpprom(11);
        }
    }

    if(Relay.relaysOn) Relay.relays();                    // activating the function that decides whether to turn on the contactors
    saveTeEpprom(47);

    char customKey = customKeypad.getKey();               // to the keyboard
    saveTeEpprom(48);

    if(customKey) keyControl(customKey);
    saveTeEpprom(49);

    inByte = Serial.read();
    if(inByte != -1)
    {
        Screen.write_LCD(0, 6, "Key-" );
        Screen.write_LCD(5, 6, (String)inByte);
        testKeyboard(inByte);
        saveTeEpprom(10);
    }

    saveTeEpprom(50);
}

void setup()
{
    Wire.begin();
    Serial.begin(115200);

    delay(500);
    Serial.println(F(" Setup - main ********** "));
    // todo - add the restartCount function, maybe also deleting ???, or writing to two cells so that there can be a larger number

    wdt_disable();      // watchdog disable

    Settings.setup();   // starting/setup the setting function

    Relay.setup();      // starting the setup function - Relay

    Screen.setup();     // starting the setup function - Screen

    Screen.oled.println(F(" Setup - main ********** "));
    Screen.oled.println(F(" "));

    readEppromTest();

    setupAnemometerInt();
    delay(100);

    i2cScannerSetup();

    // 0x3C - LCD screen Simple I2C 128x64 oled
    bool displayType = false;

    if(i2cScanner(0x3C))
    {
        Screen.displayType = 2;
        displayType = true;
    }
    else if(i2cScanner(0x27))
    {
        Screen.displayType = 1;
    }
  
    Screen.lcdSetup();

    if(displayType)
    {
        Screen.oled.println(F("Oled display found."));
        delay(1000);
    }
  
    accelClass.setup();  // activation of the accelerometer to read the inclination of the installation
  
    pinMode(LED_BUILTIN, OUTPUT);

    // todo - maybe move earlier??? and set a reset after several setup functions
    wdt_enable (WDTO_8S);   // watchdog enabled for 8 seconds
    saveTeEpprom(1);
}
