
class T1relays {

    public:

    // kod do styczników
    int relayW = 12;   // numer portu który wysyła sygnał do załączenie stycznika
    int relayE = 11;   // numer portu który wysyła sygnał do załączenie stycznika
    byte balanceAcetable = 3;       // dopuszczalny poziom odchylenia od poziomu, w czasie ustawienia poziomego
    unsigned long relayWonTimer = 0;
    unsigned long relayEonTimer = 0;
    unsigned long relaysTimerOff = 0;

    unsigned long pvAvgTimePreviousM;

    bool relayOn;
    bool windAlarm;

    int accelBalance;

    // **************
    int pvWpin, pvEpin, pvWavgTemp, pvEavgTemp, pvCountAvg;
    int sensorValue, sensorValueW, sensorValueE ;                     //
    bool relaysOn;                          // jak włączone to działają styczniki
    bool nightPos;                          // jeśli true to informacja żeby ustawić tracker w pozycji noc
                                            // TODO może sprawdzić czy po restarcie nie jest noc
    bool settingLevel;

    unsigned long relaysOffTime;    // czas przez który nie pracują styczniki, po ostatnim załączenieu w milisekundach
    unsigned long relayMaxTimeOn;   // maksymalny czas załączenia stycznika w milisekundach
    //unsigned long relayEMaxTimeOn = 3000;   // maksymalny czas załączenia stycznika w milisekundach
    unsigned long relayNightPosTimeM;

    unsigned long relayWindAlarmTimeM;  // ustawienie jak długo ma być poziom po ustaniu wiatru ???

    T1relays::T1relays(){ }

    void T1relays::setup()
    {
        saveTeEpprom(71);
        this->pvWpin = A0;                        // numer portu który odczytuje sygnał z czujnika położenia słońca
        this->pvEpin = A1;                        // numer portu który odczytuje sygnał z czujnika położenia słońca

        pinMode(this->relayW, OUTPUT);
        pinMode(this->relayE, OUTPUT);

        digitalWrite(this->relayW, LOW);
        digitalWrite(this->relayE, LOW);

        this->relayOn = false;
        this->windAlarm = false;                  // TODO to może poprawić, żeby po restarcie nie zaczynał normalnej pracy
                                                // tylko najpierw sprawdził prędkość wiatru

        this->relaysOn = true;                    // jak włączone to działają styczniki
        this->nightPos = false;                   // TODO może sprawdzić czy po restarcie nie jest noc

        this->settingLevel = false;                 // jesle true to ustawia panele do poziomu
        this->pvCountAvg = 0;
        this->pvWavgTemp = 0;
        this->pvEavgTemp = 0;
        this->sensorValueW = 100;
        this->sensorValueE = 100;

        this->pvAvgTimePreviousM = millis();

        Serial.println(F("101 relays setup +"));
        this->setLevel();   // todo czy to tu potrzebne

        this->setRelsysData();

    }

    void T1relays::setRelsysData()
    {
        saveTeEpprom(24);

        unsigned int timeX = 60000;
        if(Settings.testMode) timeX = 1000;

        Serial.println(F(" setRelsysData : "));

        this->relaysOffTime = (long)Settings.ActBreakTimeS * 1000  ;            // TODO   dodać    *60
        this->relayMaxTimeOn = (long)Settings.ActMaxMoveTimeS * 1000 ;          // TODO   dodać    *60 ???
        this->relayNightPosTimeM = (long)Settings.NightPosTimeM * timeX;        // TODO   dodać    *60
        this->relayWindAlarmTimeM = (long)Settings.WindAlarmTimeM * timeX;      // TODO   dodać    *60
    }


    void T1relays::relaysOnF(bool direction)      // East Wschód 0,     West Zachód 1
    {
        saveTeEpprom(72);
        if(direction){
            digitalWrite(this->relayE, LOW);                           // wylacza stycznik
            delay(100);
            digitalWrite(this->relayW, HIGH);                            // włącza stycznik
            delay(100);

        }else{
            digitalWrite(this->relayW, LOW);                           // wylacza stycznik
            delay(100);
            digitalWrite(this->relayE, HIGH);                            // włącza stycznik
            delay(100);
        }
    }

    void T1relays::setLevel()       // na podstawie odczytu z accelerometra poziomuje instalacje
    {
//        saveTeEpprom(73);
        Serial.print(F("124 relays setLevelrelays() = "));
        Serial.println(this->accelBalance);

        if(this->accelBalance > balanceAcetable){                         // poziomowanie instalacji
            this->relaysOnF(true);
        }else if(this->accelBalance < -balanceAcetable){
            this->relaysOnF(false);
        }else{                                    // wypoziomowana, koniec działania
            this->relaysOffF();
            this->relaysBreak();
            this->settingLevel = false;                //  jeśli true to polecenie wypoziomowania instalacji
        }
    }

    void T1relays::pvAvgCount()
    {
//        saveTeEpprom(74);
        unsigned long currentMillis = millis();

        if ((unsigned long)(currentMillis - this->pvAvgTimePreviousM) >= 150) // zmienic n 250
        {

            this->pvWavgTemp = this->pvWavgTemp + analogRead(this->pvWpin);
            this->pvEavgTemp = this->pvEavgTemp + analogRead(this->pvEpin);

            Serial.print(F(" 93 PV values East: "));
            Serial.print( analogRead(this->pvEpin) );
            Serial.print(F("   West: "));
            Serial.println( analogRead(this->pvWpin) );

            this->pvCountAvg++;

            if(this->pvCountAvg > 9)
            {
                this->sensorValueW = this->pvWavgTemp / 10;
                this->sensorValueE = (this->pvEavgTemp / 10) + Settings.SolSenAdj;                                // modyfikacja odczytu z czujnika słońca, pozycja East

                this->pvCountAvg = 0;
                this->pvWavgTemp = 0;
                this->pvEavgTemp = 0;
            }

            this->pvAvgTimePreviousM = millis();
        }
    }

    void  T1relays::relays() // TODO może zmienić
    {
        saveTeEpprom(75);
        this->pvAvgCount();

        if(this->settingLevel){
            this->setLevel();
            return;
        }

        if(!this->relaysOn){                                        // jak wyłączone styczniki to nie przechodzi dalej
            this->relaysOffF();
            this->relaysBreak();
            return;
        }

        unsigned long currentMillis = millis();

        // dotyczy odliczania po włæczaniu pozycji noc
        if(this->nightPos){

            if ((unsigned long)(currentMillis - this->relaysTimerOff) >= this->relayNightPosTimeM) {

                this->relaysTimerOff = 0;
                this->nightPos = false;                               // Serial.println("5 koniec  czasu włączenia ");
                // TODO - MOŻE ZMIENIĆ, NIE POWINNO BYĆ MOŻE PROBLEMU PÓŹNIEJ wyłącza styczniki jeśli nie zakończył się tryb poziomowania
            }
        }

        // dotyczy odliczania po zakończeniu alarmu wiatru
        if(this->windAlarm){

            if ((unsigned long)(currentMillis - this->relaysTimerOff) >= this->relayWindAlarmTimeM) {

                this->relaysTimerOff = 0;
                this->windAlarm = false;                               // Serial.println("5 koniec  czasu włączenia ");

                // TODO - MOŻE ZMIENIĆ, NIE POWINNO BYĆ MOŻE PROBLEMU PÓŹNIEJ wyłącza styczniki jeśli nie zakończył się tryb poziomowania

            } else   return;
        }

        if(this->relaysTimerOff != 0)                                   // znaczy że czas na wyłączenia styczników
        {
            if ((unsigned long)(currentMillis - this->relaysTimerOff) >= this->relaysOffTime) {
                this->relaysTimerOff = 0;                                   // Serial.println("5 koniec  czasu włączenia ");
            } else {                                                      //Serial.println(" przerwa między włączemiami styczników  AAA ");
                return;
            }
        }
        if(this->relayEonTimer != 0)                                    // znaczy że był włączony
        {
            if ((unsigned long)(millis() - this->relayEonTimer) >= this->relayMaxTimeOn)

            {
                this->relaysOffF();                                         // wylącza styczniki
                this->relayEonTimer = 0;                                    // reset czasu włączenia stycznika
                this->relaysBreak();                                        // start odliczania czasu wyłączenia stycznikóœ
            }
        }

        if(this->relayWonTimer != 0)                                    // znaczy że był włączony
        {
            if ((unsigned long)(millis() - this->relayWonTimer) >= this->relayMaxTimeOn)     // jeśli minął maksymalny czas włączenie to wyłącza stycznik
            {
                this->relayWonTimer = 0;                                    // reset czasu włączenia stycznika
                this->relaysBreak();                                        // start odliczania czasu wyłączenia stycznikóœ
            }
        }

        if(( this->sensorValueW + this->sensorValueE) > Settings.SolSenNight) // wykona się jeśli suma odczytów z czujnika słońca będzie większe
        {                                                               // niż ustawienie do pozycji noc....
            if(this->sensorValueW > this->sensorValueE + Settings.SolSenDelta)
            {
                digitalWrite(this->relayW, LOW);                           // wyłącza stycznik

                if(this->relayWonTimer != 0){
                    this->relaysOffF();
                    this->relaysBreak();
                    return;
                }

                if(this->relayEonTimer == 0 && this->relaysTimerOff == 0 && !this->relayOn)
                {
                    this->relayEonTimer = millis();    //      Serial.println("2 ustawia timer włączenia");
                    digitalWrite(this->relayE, HIGH); // włącza stycznik
                    this->relayOn = true;
                }
            }
            else if(this->sensorValueE > this->sensorValueW + Settings.SolSenDelta)
            {
                digitalWrite(this->relayE, LOW);                           // wyłącza stycznik

                if(this->relayEonTimer != 0){
                    this->relaysOffF();
                    this->relaysBreak();
                    return;
                }

                if(this->relayWonTimer == 0 && this->relaysTimerOff == 0  && !this->relayOn)
                {
                    this->relayWonTimer = millis();    //      Serial.println("2 ustawia timer włączenia");
                    digitalWrite(this->relayW, HIGH); // włącza stycznik

                    this->relayOn = true;

                }
            }else{
                this->relaysOffF();
            }

        }else{
            // Serial.println(F(" 226 pozycja noc, "));
            this->nightPos = true;
            this->settingLevel = true;
            // TODO   start odliczania czasu do ustawienia w pozycji noc...
            // zmienna    NightPosTimeM
            // jeśli przez ten czas nie będzie wyższych odczytów - ustawić w pozycji poziom
        }
    }

    void T1relays::relaysOffF()
    {
        saveTeEpprom(76);
        digitalWrite(this->relayW, LOW);                           // wylacza stycznik
        digitalWrite(this->relayE, LOW);                           // wylacza stycznik
        this->relayOn = false;
        delay(20);
    }

    void T1relays::relaysOffFtest()
    {
        saveTeEpprom(77);
        digitalWrite(this->relayW, LOW);                           // wylacza stycznik
        digitalWrite(this->relayE, LOW);                           // wylacza stycznik
        this->relayOn = false;
        delay(20);
    }

    void T1relays::relaysBreak()
    {
//        saveTeEpprom(78);
        this->relaysTimerOff = millis();                            // start odliczania czasu wyłączenia styczników
        this->relayWonTimer = 0;
        this->relayEonTimer = 0;
    }

    void T1relays::windAlarmSet()                               // funkcja uruchamia wind alarm, i ustawia kiedy go wyłączyć
    {
        saveTeEpprom(3);
//        saveTeEpprom(79);
        this->windAlarm = true;
        this->settingLevel = true;
        this->relaysTimerOff = millis();
        saveTeEpprom(4);
    }
};

T1relays Relay;
