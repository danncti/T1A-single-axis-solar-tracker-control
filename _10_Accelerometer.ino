
class T1accelerometer {

    public:

    long acc_x;
    long acc_x_bal;
  
    byte toAverage;
    long accAverage;
    
    int lastAccX;
    byte accReset;

    bool accelConnected = false;  // true - if i2cScanner(0x68) will find the device

    bool setup()
    {
        saveTeEpprom(44);
        this->accelConnected = i2cScanner(0x68);
        if(this->accelConnected) {

            // todo - set delay before sensor start, or checking procedure if (already) working
            this->setup_mpu_6050_registers();

            this->reset();
        }
        saveTeEpprom(6);
    }

    void reset()
    {
        this->lastAccX = 0;    
        this->accReset = 10;
    }
    
    void setup_mpu_6050_registers()
    {
        saveTeEpprom(58);

        // Activate the MPU-6050
        Wire.beginTransmission(0x68);  // Start communicating with the MPU-6050
        Wire.write(0x6B);              // Send the requested starting register
        Wire.write(0x00);              // Set the requested starting register
        Wire.endTransmission();        // End the transmission

        // Configure the accelerometer (+/-8g)
        Wire.beginTransmission(0x68);  // Start communicating with the MPU-6050
        Wire.write(0x1C);              // Send the requested starting register
        Wire.write(0x10);              // Set the requested starting register
        Wire.endTransmission();        // End the transmission

        // Configure the gyro (500dps full scale)
        Wire.beginTransmission(0x68);  // Start communicating with the MPU-6050
        Wire.write(0x1B);              // Send the requested starting register
        Wire.write(0x08);              // Set the requested starting register
        Wire.endTransmission();        // End the transmission

        saveTeEpprom(59);
    }

    /*** setting for accelerometer ***/
    bool readAcc()
    {                                  // Subroutine for reading the raw gyro and accelerometer data
        Wire.beginTransmission(0x68);  // Start communicating with the MPU-6050
        Wire.write(0x3B);              // Send the requested starting register

        byte busStatus = Wire.endTransmission();  // End the transmission - false if no reading

        if(busStatus != 0x00)
        {
            Serial.println("Transmission Error.... accelerometer not working!");  // transmission error wait here for ever

            // todo - set in main loop delay for another accelerometer setup
            // todo - app still from time to time is stop working when sensor are disconnected, eg. plug disconnected

           return false;
        }
        Wire.requestFrom(0x68,14);                 // Request 14 bytes from the MPU-6050

        while(Wire.available() < 14);              // Wait until all the bytes are received
        this->acc_x = Wire.read()<<8|Wire.read();  // Add the low and high byte to the acc_x variable
    }
    
    void balanceCount()
    {
        // limits the range of loaded data to the normal tilted position

        if( (this->acc_x > -5000) && (this->acc_x < 5000) )
        {
            // without taking into account the values resulting from the acceleration caused by shocks
            // cuts out rapid changes, shocks 1 degree is about 44.5
            if( ( (unsigned int)((this->lastAccX - this->acc_x )+50) ) < 100 )
            {
                this->accAverage = this->accAverage + this->acc_x;
                this->lastAccX = this->acc_x;
                this->toAverage++;
            }
            else
            {
                // resets shock cutting protection
                if(this->accReset>10)
                {
                    this->lastAccX = this->acc_x;
                    this->accReset = 0;
                }
                this->accReset++;
            }
        }
    }

    /*** stabilization of the reading of the installation inclination account ***/

    // returns a result in the range -500 to 500, 0 being horizontal
    int balancedAcc()
    {
        saveTeEpprom(60);

        while(this->toAverage < 50)
        {
            this->readAcc();
            this->balanceCount();
        }

        // after X runs it calculates the average
        // takes 1/10 of the average, which cuts out minor fluctuations
        this->acc_x_bal = (this->accAverage/500);

        this->accAverage = 0;
        this->toAverage = 1;

        saveTeEpprom(61);

        return this->acc_x_bal;
    }
};

/*** class for handling level sensor, accelerometer ***/
T1accelerometer accelClass;
