#include "Arduino.h"
#include <Wire.h>
#include "Arduino.h"
#include <EEPROM.h>

#include <avr/wdt.h>    // watchdog library - for automatic reset / reboot

// ------------------------------------------ for keyboard
#include <Keypad.h>
const byte ROWS = 1;                                              /* four rows */
const byte COLS = 4;                                              /* four columns */
char hexaKeys[ROWS][COLS] = { {'A','3','2','1'} };                /* define the symbols on the buttons of the keypads */
byte rowPins[ROWS] = {10};                                        /* connect to the row pinouts of the keypad */
byte colPins[COLS] = {6, 7, 8, 9};                                /* connect to the column pinouts of the keypad */
Keypad customKeypad ( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

int inByte = 0;                                                   // incoming serial byte

unsigned int windSpeedKmh;                  // current wind speed

/********** FOR I2C SCANER **********/
bool i2cScannerSetup()
{
    saveTeEpprom(17);
    alarmBlink();
    Serial.print(F("_80 i2cScannerSetup()"));

    #if defined (ESP8266) || defined(ESP32)
        uint8_t sda = 21;
        uint8_t scl = 22;
        Wire.begin(sda, scl, 100000);       // ESP32 - change config pins if needed.
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

bool i2cScanner(byte testAddress)
{
    saveTeEpprom(12);
    byte errorResult;                       // error code returned by I2C Wire.endTransmission()

    Serial.print(F("Scanning 0x"));
    if (testAddress < 0x10)  Serial.print(F("0"));  // pad single digit addresses with a leading "0"
    Serial.println(testAddress, HEX);

    Wire.beginTransmission(testAddress);            // initiate communication at current address
    errorResult = Wire.endTransmission();           // if a device is present, it will send an ack and "0" will be returned from function

    Serial.println(F("Scan complete."));

    if (errorResult == 0)                           // "0" means a device at current address has acknowledged the serial communication
    {
        Serial.print(F("I2C device found at 0x"));
        if (testAddress < 0x10) Serial.print(F("0"));  // pad single digit addresses with a leading "0"
        Serial.println(testAddress, HEX);           // display the address on the serial monitor when a device is found
        Serial.println();

        return true;
    }

    Serial.println(F("I2C device NOT found"));
    return false;
}
/******************/


//************************************************************************************************************
// TEST AND DEMO FUNCTIONS - TO DELETE ...

void alarmBlink()
{
    saveTeEpprom(18);
    digitalWrite(LED_BUILTIN, HIGH);    // turn the LED on
    delay(500);                         // wait for half a second
    digitalWrite(LED_BUILTIN, LOW);     // turn the LED off
    delay(500);                         // wait for half a second
}

unsigned long curMillis = millis();
byte saveAddress = 52;
byte currentAddress = 53;
byte lastValue = 250;
bool toSave = false;

void saveTeEpprom(byte value)
{
    // Serial.print(F("  currentAddress "));
    // Serial.println(currentAddress);
    // EEPROM.write(saveAddress, currentAddress);
    if(toSave)
    {
        // start recording use function, to check with was last before freeze
        // if the same value as before, it does not save
        if(value == lastValue) return;

        EEPROM.write(currentAddress, value);
        // Serial.print(F("2 currentAddress "));
        // Serial.println(currentAddress);
        lastValue = value;
    }
    else
    {
        curMillis = millis();
        if (curMillis >= 5000 && !toSave) { // 60000

            // after running, it takes the last used address and increases it by 1
            currentAddress = (EEPROM.read(saveAddress)) + 1;

            if(currentAddress >= 251){ currentAddress = saveAddress + 1; }

            // currentAddress = saveAddress; // temp
            // saves the address it is currently using
            EEPROM.write(saveAddress, currentAddress);

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

    Serial.print(F("Last entry at-"));
    Serial.println(address);
    Serial.println(F(" ") );

    Serial.print(F("address "));
    Serial.print(address);
    Serial.print(F(" value ") );
    Serial.println(EEPROM.read(address));
    Serial.println(F("---------------") );

    // address = 52;
    // while(address < 252)
    // {
    //
    //     address++;
    //     delay(5);
    //     EEPROM.write(address, 200);
    // }
    // EEPROM.write(52, 52);

    address = 51;
    while(address < 252){

        address++;
        Serial.print(F("address "));
        Serial.print(address);
        Serial.print(F(" value ") );
        Serial.println(EEPROM.read(address));
    }

}

void testKeyboard(char inByte)  // test function, for testing keyboard operation by entering commands via serial monitor
{
    if(inByte == 49){ keyControl('1'); } // 1 on the keyboard - for testing
    if(inByte == 50){ keyControl('2'); } // 2 on the keyboard - for testing
    if(inByte == 51){ keyControl('3'); } // 3 on the keyboard - for testing
    if(inByte == 46){ keyControl('A'); } // . on the keyboard - for testing
}
