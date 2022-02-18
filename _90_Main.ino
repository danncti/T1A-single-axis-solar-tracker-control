
void loop()
{
    wdt_reset();        // watchdog restar timer
    saveTeEpprom(100);

    //    unsigned long currentMillis = millis();                     // Get snapshot of time  ********** correct way to use  counters

    if (Screen.refreshScreen())      // Refresh lcd
    {
        saveTeEpprom(2);
        windSpeedKmh = get_wind_speed();

        if((windSpeedKmh >= Settings.WindSpeedAlarmKm)){        // jeśli prędkość wiatru większa niż ustalona do alarmu, to wykonać to

            Relay.windAlarmSet();                                     // funkcja obsługuje przerwę przez którą mają być wyłączone podążanie za słońcem po ustaniu wiatru
        }

        if(accelClass.accelConnected)
        {
            // todo może accelerometer uruchamiać dopiero jak jest potrzeba???
            // todo tutaj był ostatni zapis, potem program się zawiesił

            Relay.accelBalance = (accelClass.balancedAcc() /10) + Settings.LevelAdj;

            saveTeEpprom(5);
        }else{

            accelClass.setup();
        }

        if(Screen.lcdStateOn){
//            saveTeEpprom(45);
            if(Screen.menuLevel == 0 && Screen.position[0] == 0)
            saveTeEpprom(7);
            Screen.menuCurrent(); // TODO tymczasowe rozwiązanie
            saveTeEpprom(8);
            Screen.lcdOff(false);
        }
        Screen.write_LCD(14, 4, "    ");
        Screen.write_LCD(0, 4, "Ostatni zapis" );
        Screen.write_LCD(14, 4, (String)(currentAddress - 53));
        saveTeEpprom(46);

        // todo tymczasowe włączenie na stałe ekrany - bo nie używam klawiatury
        if(!Screen.lcdStateOn)
        {
            Screen.lcdOn();
            saveTeEpprom(11);
        }
    }

    if(Relay.relaysOn) Relay.relays();                          // uruchomienie funkcji która decyduje czy włączyć styczniki
    saveTeEpprom(47);

    char customKey = customKeypad.getKey();                           // do klawiatury
    saveTeEpprom(48);

    if(customKey) keyControll(customKey);
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
    // todo dodać funkcję restartCount, może też kasowanie ???, albo zapis do dwu komórek, żeby mogła być większa liczba

    wdt_disable();      // watchdog disable

    Settings.setup();   // uruchomienie / setup funcji setting

    Relay.setup();      // uruchomienie funkcji setup - Relay

    Screen.setup();     // uruchomienie funkcji setup - Screen

    Screen.oled.println(F(" Setup - main ********** "));
    Screen.oled.println(F(" "));

    readEppromTest();




    setupAnemometerInt();
    delay(100);

    i2cScanerSetup();

    // 0x3C - LCD screen Simple I2C 128x64 oled
    bool displayType = false;

    if(i2cScaner(0x3C))
    {
        Screen.displayType = 2;
        displayType = true;
    }
    else if(i2cScaner(0x27)) { Screen.displayType = 1; }
  
    Screen.lcdSetup();

    if(displayType){
        Screen.oled.println(F("Oled display found."));
        delay(1000);
    }
  
    accelClass.setup(); // uruchomienie accelerometru, do odczytu pochylenia instalacji
  
    pinMode(LED_BUILTIN, OUTPUT);

    // todo może przenieść wcześniej??? i ustawić reset po kilku funkcjach setupu
    wdt_enable (WDTO_8S);   // watchdog enabled for 8 seconds
    saveTeEpprom(1);

}
