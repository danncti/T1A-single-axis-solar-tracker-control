
class T1settings {
  
    public:
      
    /*** stałe i informacje w których komórkach są zapisane ustawienia ***/
    static const byte ROWS = 9; /* four rows */ // todo - check it
    static const byte COLS = 4; 
    static const byte AccAdd = 127;
  
    // value, step, add, memory
  
    byte settingsX[ROWS][COLS] = 
    {
        { 10, 2, 0, 10 },  // prędkość wiatru w km/h od której włącza się alarm
        { 5,  5, 0, 11 },  // czas między wypoziomowaniem panali a próbami powrotu do normalnej pracy
        { 20, 1, 0, 15 },  // maksymalny czas poruszania się siłownika ????? czy potrzebne
        { 5,  1, 0, 16 },  // czas w minutach (sekundach) w których siłownik nie pracuje
        { 10, 2, 0, 20 },  // minimalna róznica w odczytach z czujników nasłonecznienia, żeby nastąpiło przesunięcie instalacji
        { 120,2, 0, 21 },  // poziom odczytu z czujników nasłonecznienia po których następuje ustawienie instalacji na noc
        { 3,  2, 0, 22 },  // czas w minutach po których, jeśli jest ciemno następuje ustawienie w pozycji noc
        { AccAdd, 1, AccAdd, 30 },  // 100 = level 0
        { AccAdd, 1, AccAdd, 32 },  //  100 = 0, poniżej to minus, powyżej to plus
    };

    byte WindSpeedAlarmKm;  // prędkość wiatru w km/h od której włącza się alarm
    byte WindAlarmTimeM;    // czas między wypoziomowaniem panali a próbami powrotu do normalnej pracy
    byte ActMaxMoveTimeS;   // maksymalny czas poruszania się siłownika ????? czy potrzebne
    byte ActBreakTimeS;     // czas w sekundach w których siłownik nie pracuje
    byte SolSenDelta;       // minimalna róznica w odczytach z czujników nasłonecznienia, żeby nastąpiło przesunięcie instalacji
    byte SolSenNight;       // suma z czujników przy której przestaje włączać sterowanie
    byte NightPosTimeM;     // czas w minutach po których, jeśli jest ciemno następuje ustawienie w pozycji noc
    int LevelAdj;           // modyfikacja odczytu z czujnika konta położenia paneli
    int SolSenAdj;          // modyfikacja odczytu z czujnika słońca, pozycja East
  
    bool testMode;
    
    T1settings::T1settings(){ }
    
    void T1settings::setup()
    {
        saveTeEpprom(63);
        // this->memoryReset();  // przywraca ustawienia 'fabryczne'
        this->testMode = true;   // if true, working in test mode (diferent times, etc.)

        for (int i = 0 ; i < this->ROWS ; i++) {

            byte temp = this->read( this->settingsX[i][3] );
            if(temp == 255){
                // Serial.println(" zapis do pamięci " );
                this->set( i, this->settingsX[i][0] ) ;

                this->save(
                    this->settingsX[i][3],
                    byte( this->get(i) + this->settingsX[i][2] )
                );
            }
            else {
                this->set( i, temp );
            }
        }
    }
    
    void T1settings::set(int varible, int value)
    {
        saveTeEpprom(64);
        switch(varible){
            case 0:
                this->WindSpeedAlarmKm = value;
                break;

            case 1:
                this->WindAlarmTimeM = value ;
                break;

            case 2:
                this->ActMaxMoveTimeS = value;
                break;

            case 3:
                this->ActBreakTimeS = value;
                break;

            case 4:
                this->SolSenDelta = value;
                break;

            case 5:
                this->SolSenNight = value;
                break;

            case 6:
                this->NightPosTimeM = value;
                break;

            case 7:
                this->LevelAdj = (value - AccAdd);
                break;
            case 8:
                this->SolSenAdj = (value - AccAdd);
                break;
        }
    }
    
    int T1settings::get(byte name)
    {
        saveTeEpprom(65);
        Serial.print(F(" settings get: "));
        Serial.println(name);

        switch(name){
            case 0:
                return this->WindSpeedAlarmKm;

            case 1:
                return this->WindAlarmTimeM ;

            case 2:
                return this->ActMaxMoveTimeS ;

            case 3:
                return this->ActBreakTimeS ;

            case 4:
                return this->SolSenDelta ;

            case 5:
                return this->SolSenNight ;

            case 6:
                return this->NightPosTimeM;

            case 7:
                return this->LevelAdj;

            case 8:
                return this->SolSenAdj;
        }
    }
    
    String T1settings::getSettingsNames(byte row, byte col)
    {
        saveTeEpprom(66);
        Serial.print(F(" getSettingsNames - row: "));
        Serial.print(row);
        String names[ROWS][2] =
        {           //  "12345123451234512345" // todo - what is it?
            { "WindSpeedAlarmKm", "Alarm ON Wind Speed km/h" },
            { "WindAlarmTimeM",   "Wind alarm time (M)"  },
            { "ActMaxMoveTimeS",  "Max move time (S)" },
            { "ActBreakTimeS",    "Breaks Between Moves (S)"  },
            { "SolSenDelta",    "Solar Sensor Delta" },
            { "SolSenNight",    "Solar Sensor Night"  },
            { "NightPosTimeM",    "Night Position After x (Min.)" },
            { "LevelAdj",     "Level Adjustment" },
            { "SolSenAdj",      "Solar Sensor Adjust-ment East" },
        };

        Serial.print(F("  "));
        Serial.println(names[row][col]);

        return names[row][col];
    }
    
    String T1settings::getDesc(String name)
    {
        saveTeEpprom(67);

        // varibleName, varible Description, change settings STEP
        for (int i = 0 ; i < this->ROWS ; i++) {

            String tempName = this->getSettingsNames(i, 0);
            if(name == tempName){

              return this->getSettingsNames(i, 1);
            }
        }
    }
      
    void T1settings::save(int address, byte value)  // do zapisywania danych do pamięci stałej
    {
        saveTeEpprom(68);
        EEPROM.write(address, value);

        Serial.print(F("Saved adres " ));
        Serial.print( address );
        Serial.print(F("   value "));
        Serial.println(value);
    }
    
    byte T1settings::read(int address)  // do odczytywania danych z pamięci stałej
    {
        saveTeEpprom(69);
        byte value = EEPROM.read(address);

        if(value>=0 && value <=255){
            return value;
        }
        return 255;
    }

    /*** funkcja zapisuje do pamięci ustawienia 'fabryczne' ***/
    void T1settings::memoryReset()
    {
        saveTeEpprom(70);

        for (int i = 0 ; i < EEPROM.length() ; i++) {
            EEPROM.write(i, 255);
        }
    }
};

T1settings Settings;
