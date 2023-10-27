
class T1relays {

    public:

    /*** code for relays ***/
    int relayW = 12;           // port number that sends a signal to turn on the relay
    int relayE = 11;           // port number that sends a signal to turn on the relay
    byte balanceAcceptable = 3;  // permissible level of deviation from horizontal when placed horizontally
    unsigned long relayWonTimer = 0;
    unsigned long relayEonTimer = 0;
    unsigned long relaysTimerOff = 0;

    unsigned long pvAvgTimePreviousM;

    bool relayOn;
    bool windAlarm;

    int accelBalance;

    // **************
    int pvWpin, pvEpin, pvWavgTemp, pvEavgTemp, pvCountAvg;
    int sensorValue, sensorValueW, sensorValueE ;
    bool relaysOn;  // when turned on, the relays work
    bool nightPos;  // if true, information to set the tracker to the night position
    // todo - can check if it's not night after the restart
    bool settingLevel;

    unsigned long relaysOffTime;   // time during which the relays do not operate after the last switching on in milliseconds
    unsigned long relayMaxTimeOn;  // maximum relay activation time in milliseconds
    //unsigned long relayEMaxTimeOn = 3000;  // maximum relay activation time in milliseconds
    unsigned long relayNightPosTimeM;

    unsigned long relayWindAlarmTimeM;  // setting how long should the level be after the wind stops???

    T1relays::T1relays(){ }

    void T1relays::setup()
    {
        saveTeEpprom(71);
        this->pvWpin = A0;  // port number that reads the signal from the sun position sensor
        this->pvEpin = A1;  // port number that reads the signal from the sun position sensor

        pinMode(this->relayW, OUTPUT);
        pinMode(this->relayE, OUTPUT);

        digitalWrite(this->relayW, LOW);
        digitalWrite(this->relayE, LOW);

        this->relayOn = false;
        this->windAlarm = false;
        // todo - this may improve it so that after the restart it does not start normal operation but checks the wind speed first

        this->relaysOn = true;   // when turned on, the relays work
        this->nightPos = false;  // todo - can check if it's not night after the restart

        this->settingLevel = false;  // if true, it sets the panels horizontally
        this->pvCountAvg = 0;
        this->pvWavgTemp = 0;
        this->pvEavgTemp = 0;
        this->sensorValueW = 100;
        this->sensorValueE = 100;

        this->pvAvgTimePreviousM = millis();

        Serial.println(F("101 relays setup +"));
        this->setLevel();   // todo - Is it needed here?

        this->setRelaysData();

    }

    void T1relays::setRelaysData()
    {
        saveTeEpprom(24);

        unsigned int timeX = 60000;
        if(Settings.testMode) timeX = 1000;

        Serial.println(F(" setRelaysData : "));

        this->relaysOffTime = (long)Settings.ActBreakTimeS * 1000  ;        // todo -   add    *60    - testing settings shorter
        this->relayMaxTimeOn = (long)Settings.ActMaxMoveTimeS * 1000 ;      // todo -   add    *60 ???
        this->relayNightPosTimeM = (long)Settings.NightPosTimeM * timeX;    // todo -   add    *60
        this->relayWindAlarmTimeM = (long)Settings.WindAlarmTimeM * timeX;  // todo -   add    *60
    }


    void T1relays::relaysOnF(bool direction)  // East 0,     West 1
    {
        saveTeEpprom(72);
        if(direction){
            digitalWrite(this->relayE, LOW);   // turns off the relays
            delay(100);
            digitalWrite(this->relayW, HIGH);  // turns on the relays
            delay(100);

        }else{
            digitalWrite(this->relayW, LOW);   // turns off the relays
            delay(100);
            digitalWrite(this->relayE, HIGH);  // turns on the relays
            delay(100);
        }
    }

    /*** levels the installation based on the reading from the accelerometer ***/
    void T1relays::setLevel()
    {
        // saveTeEpprom(73);
        Serial.print(F("124 relays setLevelrelays() = "));
        Serial.println(this->accelBalance);

        // installation leveling???
        if(this->accelBalance > balanceAcceptable){

            this->relaysOnF(true);
        }else if(this->accelBalance < -balanceAcceptable){
            this->relaysOnF(false);
        }else{
            // is level, end of operation ???
            this->relaysOffF();
            this->relaysBreak();
            this->settingLevel = false;  // if true, the command to level the installation
        }
    }

    void T1relays::pvAvgCount()
    {
        // saveTeEpprom(74);
        unsigned long currentMillis = millis();

        if ((unsigned long)(currentMillis - this->pvAvgTimePreviousM) >= 150) // todo - change to - 250
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
                // modification of the reading from the sun sensor, East position
                this->sensorValueE = (this->pvEavgTemp / 10) + Settings.SolSenAdj;

                this->pvCountAvg = 0;
                this->pvWavgTemp = 0;
                this->pvEavgTemp = 0;
            }
            this->pvAvgTimePreviousM = millis();
        }
    }

    void  T1relays::relays() // todo - maybe change
    {
        saveTeEpprom(75);
        this->pvAvgCount();

        if(this->settingLevel){
            this->setLevel();
            return;
        }

        if(!this->relaysOn)
        {
            // if the relays are turned off, it does not go any further
            this->relaysOffF();
            this->relaysBreak();
            return;
        }

        unsigned long currentMillis = millis();

        // concerns the countdown after turning on the night position
        if(this->nightPos){

            if ((unsigned long)(currentMillis - this->relaysTimerOff) >= this->relayNightPosTimeM) {

                this->relaysTimerOff = 0;
                this->nightPos = false;
                // Serial.println("5 end of switch-on time ");
                // todo - maybe change, THERE SHOULD BE NO PROBLEM LATER - Turns off the relays if the leveling mode has not ended
            }
        }

        // applies to the countdown after the wind alarm ends
        if(this->windAlarm){

            if ((unsigned long)(currentMillis - this->relaysTimerOff) >= this->relayWindAlarmTimeM) {

                this->relaysTimerOff = 0;
                this->windAlarm = false;
                // Serial.println("5 end of switch-on time ");

                // todo - maybe change, THERE SHOULD BE NO PROBLEM LATER - Turns off the relays if the leveling mode has not ended

            } else   return;
        }

        if(this->relaysTimerOff != 0)
        {
            // means it's time to turn off the relays
            if ((unsigned long)(currentMillis - this->relaysTimerOff) >= this->relaysOffTime)
            {
                this->relaysTimerOff = 0;
                // Serial.println("5 end of switch-on time ");
            }
            else
            {
                //Serial.println(" interval between switching on the relays  AAA ");
                return;
            }
        }
        if(this->relayEonTimer != 0)
        {
            // means it was turned on
            if ((unsigned long)(millis() - this->relayEonTimer) >= this->relayMaxTimeOn)
            {
                this->relaysOffF();       // turns off relays
                this->relayEonTimer = 0;  // relays switch-on time reset
                this->relaysBreak();      // start of the relays off time countdown
            }
        }

        if(this->relayWonTimer != 0)
        {
            // means it was turned on
            if ((unsigned long)(millis() - this->relayWonTimer) >= this->relayMaxTimeOn)
            {
                // if the maximum switch-on time has passed, the relays turns off
                this->relayWonTimer = 0;  // relays switch-on time reset
                this->relaysBreak();      // start of the relays off time countdown
            }
        }

        if(( this->sensorValueW + this->sensorValueE) > Settings.SolSenNight)
        {
            // will be executed if the sum of readings from the sun sensor is greater
            // than setting to the night position....
            if(this->sensorValueW > this->sensorValueE + Settings.SolSenDelta)
            {
                digitalWrite(this->relayW, LOW);  // turns off the relay

                if(this->relayWonTimer != 0)
                {
                    this->relaysOffF();
                    this->relaysBreak();
                    return;
                }

                if(this->relayEonTimer == 0 && this->relaysTimerOff == 0 && !this->relayOn)
                {
                    this->relayEonTimer = millis();    // Serial.println("2 sets the on timer");
                    digitalWrite(this->relayE, HIGH);  // turns on the relay
                    this->relayOn = true;
                }
            }
            else if(this->sensorValueE > this->sensorValueW + Settings.SolSenDelta)
            {
                digitalWrite(this->relayE, LOW);  // turns off the relay

                if(this->relayEonTimer != 0)
                {
                    this->relaysOffF();
                    this->relaysBreak();
                    return;
                }

                if(this->relayWonTimer == 0 && this->relaysTimerOff == 0  && !this->relayOn)
                {
                    this->relayWonTimer = millis();    // Serial.println("2 sets the on timer");
                    digitalWrite(this->relayW, HIGH);  // turns on the relay

                    this->relayOn = true;
                }
            }
            else
            {
                this->relaysOffF();
            }

        }
        else
        {
            // Serial.println(F(" 226 night position, "));
            this->nightPos = true;
            this->settingLevel = true;
            // todo - start the time countdown to set to the night position...
            // variable - NightPosTimeM
            // if there are no higher readings during this time - set the level position
        }
    }

    void T1relays::relaysOffF()
    {
        saveTeEpprom(76);
        digitalWrite(this->relayW, LOW);  // turns off the relay
        digitalWrite(this->relayE, LOW);  // turns off the relay
        this->relayOn = false;
        delay(20);
    }

    void T1relays::relaysOffFtest()
    {
        saveTeEpprom(77);
        digitalWrite(this->relayW, LOW);  // turns off the relay
        digitalWrite(this->relayE, LOW);  // turns off the relay
        this->relayOn = false;
        delay(20);
    }

    void T1relays::relaysBreak()
    {
        // saveTeEpprom(78);
        this->relaysTimerOff = millis();  // start of the countdown of the relay switch-off time
        this->relayWonTimer = 0;
        this->relayEonTimer = 0;
    }

    // The function activates the wind alarm, and sets when to turn it off
    void T1relays::windAlarmSet()
    {
        saveTeEpprom(3);
        // saveTeEpprom(79);
        this->windAlarm = true;
        this->settingLevel = true;
        this->relaysTimerOff = millis();
        saveTeEpprom(4);
    }
};

T1relays Relay;
