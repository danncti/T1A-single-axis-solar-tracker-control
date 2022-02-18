#include "Arduino.h"
#include <Wire.h>
#include "Arduino.h"
#include <EEPROM.h>

#include <avr/wdt.h>    // watchdog libary - for automatic reset

#include <Keypad.h>                                               // dla klawiatury
                                                                  // ------------------------------------------dla klawiatury
const byte ROWS = 1;                                              /* four rows */
const byte COLS = 4;                                              /* four columns */
char hexaKeys[ROWS][COLS] = { {'A','3','2','1'} };                /* define the symbols on the buttons of the keypads */
byte rowPins[ROWS] = {10};                                        /* connect to the row pinouts of the keypad */
byte colPins[COLS] = {6, 7, 8, 9};                                /* connect to the column pinouts of the keypad */
Keypad customKeypad ( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

int inByte = 0;                                                   // incoming serial byte

unsigned int windSpeedKmh;                        // aktualna predkosc wiatru

/********** FOR I2C SCANER **********/
bool i2cScanerSetup()
{
    saveTeEpprom(17);
    alarmBlink();
    Serial.print(F("_80 i2cScanerSetup()"));

    #if defined (ESP8266) || defined(ESP32)
        uint8_t sda = 21;
        uint8_t scl = 22;
        Wire.begin(sda, scl, 100000);  // ESP32 - change config pins if needed.
    #else
        Wire.begin();
    #endif

    #if defined WIRE_IMPLEMENT_WIRE1 || WIRE_INTERFACES_COUNT > 1
        Wire1.begin();
        wirePortCount++;
    #endif

    #if defined WIRE_IMPLEMENT_WIRE2 || WIRE_INTERFACES_COUNT > 2
        Wire2.begin();
        wirePortCount++;
    #endif

    #if defined WIRE_IMPLEMENT_WIRE3 || WIRE_INTERFACES_COUNT > 3
        Wire3.begin();
        wirePortCount++;
    #endif
}

bool i2cScaner(byte testAddress)
{
    saveTeEpprom(12);
    byte errorResult;           // error code returned by I2C Wire.endTransmission()

    Serial.print(F("Scanning 0x"));
    if (testAddress < 0x10)  Serial.print(F("0"));                // pad single digit addresses with a leading "0"
    Serial.println(testAddress, HEX);

    Wire.beginTransmission(testAddress);                          // initiate communication at current address
    errorResult = Wire.endTransmission();                         // if a device is present, it will send an ack and "0" will be returned from function

    Serial.println(F("Scan complete."));

    if (errorResult == 0)                                           // "0" means a device at current address has acknowledged the serial communication
    {
        Serial.print(F("I2C device found at 0x"));
        if (testAddress < 0x10) Serial.print(F("0"));                                // pad single digit addresses with a leading "0"
        Serial.println(testAddress, HEX);                       // display the address on the serial monitor when a device is found
        Serial.println();

        return true;
    }

    Serial.println(F("I2C device NOT found"));
    return false;
}
/******************/


//*****************************************************************************************************************************
// FUNKCJE TESTOWE, POKAZOWE, MOŻNA BĘDZIE USUŃĄĆ, ...
//
void alarmBlink()
{
    saveTeEpprom(18);
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on
    delay(500);                       // wait for half a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off
    delay(500);                       // wait for half a second
}

unsigned long curMillis = millis();
byte saveAddres = 52;
byte currentAddress = 53;
byte lastValue = 250;
bool toSave = false;

void saveTeEpprom(byte value)
{
//Serial.print(F("  currentAddress "));
//Serial.println(currentAddress);
//EEPROM.write(saveAddres, currentAddress);
    if(toSave)
    {
        // start recording usef function, to check with was last before freeze
        // jeśli ta sama wartość co porzednio to nie zapisuje
        if(value == lastValue) return;

        EEPROM.write(currentAddress, value);
//        Serial.print(F("2 currentAddress "));
//        Serial.println(currentAddress);
        lastValue = value;

    }else{

        curMillis = millis();
        if (curMillis >= 5000 && !toSave) { // 60000

            // po uruchomieniu pobiera ostatnio wykożystany adres i zwiększa go o 1
            currentAddress = (EEPROM.read(saveAddres)) + 1;

            if(currentAddress >= 251){ currentAddress = saveAddres + 1; }

            //currentAddress = saveAddres; // temp
            // zapisuje adres z którego obecnie kożysta
            EEPROM.write(saveAddres, currentAddress);

            toSave = true;
        }
    }
}

void readEppromTest()
{
    Serial.println(F(" readEpprom "));

    byte address = 51;

    delay(500);

    address = EEPROM.read(52);

    Serial.print(F("Ostatni zapis pod adresem-"));
    Serial.println(address);
    Serial.println(F(" ") );

    Serial.print(F("addres "));
    Serial.print(address);
    Serial.print(F(" value ") );
    Serial.println(EEPROM.read(address));
    Serial.println(F("---------------") );
//
//    address = 52;
//    while(address < 252){
//
//        address++;
//        delay(5);
//        EEPROM.write(address, 200);
//    }
//    EEPROM.write(52, 52);

    address = 51;
    while(address < 252){

        address++;
        Serial.print(F("addres "));
        Serial.print(address);
        Serial.print(F(" value ") );
        Serial.println(EEPROM.read(address));
    }

}

void testKeyboard(char inByte)  // funkcja testowa, do testowania działania klawiatury, przez wpisywanie poleceń przez serial monitor
{
  if(inByte == 49){ keyControll('1'); } // 1 na klawiaturze - do testów
  if(inByte == 50){ keyControll('2'); } // 2 na klawiaturze - do testów
  if(inByte == 51){ keyControll('3'); } // 3 na klawiaturze - do testów
  if(inByte == 46){ keyControll('A'); } // . na klawiaturze - do testów
}
