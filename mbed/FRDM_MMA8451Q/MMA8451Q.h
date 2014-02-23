/* Copyright (c) 2010-2011 mbed.org, MIT License
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef MMA8451Q_H
#define MMA8451Q_H

#include "mbed.h"

/**
* MMA8451Q accelerometer example
*
* @code
* #include "mbed.h"
* #include "MMA8451Q.h"
* 
* #define MMA8451_I2C_ADDRESS (0x1d<<1)
* 
* int main(void) {
* 
* MMA8451Q acc(P_E25, P_E24, MMA8451_I2C_ADDRESS);
* PwmOut rled(LED_RED);
* PwmOut gled(LED_GREEN);
* PwmOut bled(LED_BLUE);
* 
*     while (true) {       
*         rled = 1.0 - abs(acc.getAccX());
*         gled = 1.0 - abs(acc.getAccY());
*         bled = 1.0 - abs(acc.getAccZ());
*         wait(0.1);
*     }
* }
* @endcode
*/

// Z-Lock Threshold Angles
#define Z_LOCKOUT_14    0       // Angle to 14°
#define Z_LOCKOUT_18    1       // Angle to 18°
#define Z_LOCKOUT_21    2       // Angle to 21°
#define Z_LOCKOUT_25    3       // Angle to 25°
#define Z_LOCKOUT_29    4       // Angle to 29°
#define Z_LOCKOUT_33    5       // Angle to 33°
#define Z_LOCKOUT_37    6       // Angle to 37°
#define Z_LOCKOUT_42    7       // Angle to 42°
// Back/Front Orientation Definition
#define Z_BKFR_80       0       // Back and Front trip angle
#define Z_BKFR_75       1       // Back and Front trip angle
#define Z_BKFR_70       2       // Back and Front trip angle
#define Z_BKFR_65       3       // Back and Front trip angle
// Threshold Angle Thresholds Lookup Table
#define PL_THS_15       0x07    // Set Threshold to 15°
#define PL_THS_20       0x09    // Set Threshold to 20°
#define PL_THS_30       0x0C    // Set Threshold to 30°
#define PL_THS_35       0x0D    // Set Threshold to 35°
#define PL_THS_40       0x0F    // Set Threshold to 40°
#define PL_THS_45       0x10    // Set Threshold to 45°
#define PL_THS_55       0x13    // Set Threshold to 55°
#define PL_THS_60       0x14    // Set Threshold to 60°
#define PL_THS_70       0x17    // Set Threshold to 70°
#define PL_THS_75       0x19    // Set Threshold to 75°
// Trip Angles with Hysteresis for 45° Angle
#define PL_HYS_0        0x00    // Set Hysteresis to ±0°    
#define PL_HYS_4        0x01    // Set Hysteresis to ±4°    
#define PL_HYS_7        0x02    // Set Hysteresis to ±7°
#define PL_HYS_11       0x03    // Set Hysteresis to ±11°
#define PL_HYS_14       0x04    // Set Hysteresis to ±14°
#define PL_HYS_17       0x05    // Set Hysteresis to ±17°
#define PL_HYS_21       0x06    // Set Hysteresis to ±21°
#define PL_HYS_24       0x07    // Set Hysteresis to ±24°
// Landscape/Portrait orientation
#define cLAPO_PU        0       // Portrait Up: Equipment standing vertically in the normal orientation
#define cLAPO_PD        1       // Portrait Down: Equipment standing vertically in the inverted orientation
#define cLAPO_LR        2       // Landscape Right: Equipment is in landscape mode to the right
#define cLAPO_LL        3       // Landscape Left: Equipment is in landscape mode to the left.
// System Output Data Rate for acceleration samples
#define cODR_800HZ      0       // 1.25 ms
#define cODR_400HZ      1       // 2.5 ms
#define cODR_200HZ      2       // 5 ms
#define cODR_100HZ      3       // 10 ms
#define cODR_50HZ       4       // 20 ms
#define cODR_12_5HZ     5       // 80 ms
#define cODR_6_25HZ     6       // 160 ms
#define cODR_1_56HZ     7       // 640 ms

class MMA8451Q
{
public:
    /**
    * MMA8451Q constructor
    *
    * @param sda SDA pin
    * @param sdl SCL pin
    * @param addr addr of the I2C peripheral
    */
    MMA8451Q(PinName sda, PinName scl, int addr);
    
    /**
    * MMA8451Q destructor
    */
    ~MMA8451Q();
    
    /**
    * Get the value of the WHO_AM_I register
    *
    * @returns WHO_AM_I value
    */
    uint8_t getWhoAmI();
    
    /**
    * Get X axis acceleration
    *
    * @returns X axis acceleration
    */
    float getAccX();
    
    /**
    * Get Y axis acceleration
    *
    * @returns Y axis acceleration
    */
    float getAccY();
    
    /**
    * Get Z axis acceleration
    *
    * @returns Z axis acceleration
    */
    float getAccZ();
    
    /**
    * Get XYZ axis acceleration
    *
    * @param res array where acceleration data will be stored
    */
    void getAccAllAxis(float * res);

    /**
    * Get raw value for X axis acceleration
    *
    * @returns X axis acceleration
    */
    int16_t getAccRawX( int16_t * res);
    
    /**
    * Get raw value for Y axis acceleration
    *
    * @returns Y axis acceleration
    */
    int16_t getAccRawY( int16_t * res);
    
    /**
    * Get raw value for Z axis acceleration
    *
    * @returns Z axis acceleration
    */
    int16_t getAccRawZ( int16_t * res);

    /**
    * Get raw values for XYZ axis acceleration
    *
    * @param res array where acceleration data will be stored
    */
    unsigned int getAccRawAllAxis(int16_t * res);

    /**
    * Configure the Accelerometere for free fall detection
    *
    * @param pointer to the user function to execute after IRQ assertion
    * @return none
    */
    void FreeFallDetection( void(*fptr)(void));

    /**
    * Configure the Accelerometere for motion detection
    *
    * @param pointer to the user function to execute after IRQ assertion
    * @return none
    */
    void MotionDetection( void(*fptr)(void));
    
    /**
    * Configure the Accelerometere for orientation detection
    *
    * @param pointer to the user function to execute after IRQ assertion
    * @param Z lockout value, Z Backfront, Port/Landscape Thsld, Port/Landscape Hysteresis
    * @return none
    */
    void OrientationDetect( void(*fptr)(void), unsigned int Z_LockOut, unsigned int Z_BkFr, unsigned int PL_Thsld, unsigned int PL_Hyst);
    void OrientationDetect( void(*fptr)(void));
       
    /**
    * Get the orientation state. 
    *
    *    0x10: PL_STATUS Register (Read Only)
    *    Bit 7   Bit 6   Bit 5   Bit 4   Bit 3   Bit 2   Bit 1   Bit 0
    *    NEWLP   LO      —       —       —       LAPO[1] LAPO[0] BAFRO
    *
    *    NEWLP
    *        Landscape/Portrait status change flag. Default value: 0.
    *        0: No change, 1: BAFRO and/or LAPO and/or Z-Tilt lockout value has changed
    *    LO
    *        Z-Tilt Angle Lockout. Default value: 0.
    *        0: Lockout condition has not been detected.
    *        1: Z-Tilt lockout trip angle has been exceeded. Lockout has been detected.
    *    LAPO[1:0](*)
    *        Landscape/Portrait orientation. Default value: 00
    *        00: Portrait Up: Equipment standing vertically in the normal orientation
    *        01: Portrait Down: Equipment standing vertically in the inverted orientation
    *        10: Landscape Right: Equipment is in landscape mode to the right
    *        11: Landscape Left: Equipment is in landscape mode to the left.
    *        (*) The default power up state is BAFRO = 0, LAPO = 0, and LO = 0.        
    *    BAFRO
    *        Back or Front orientation. Default value: 0
    *        0: Front: Equipment is in the front facing orientation.
    *        1: Back: Equipment is in the back facing orientation.
    */
    unsigned char GetOrientationState( void);

    /**
    * Configure the Accelerometer to generate a IRQ when a new sample is available
    *
    * @param pointer to the user function to execute after IRQ assertion
    * @return none
    */
    void DataReady( void(*fptr)(void), unsigned char ODR);
    
    unsigned int isDataAvailable( void);
    
    /**
    * Soft Reset
    * @param none
    * @return none
    */
    void Reset( void);
    
private:
    I2C m_i2c;
    int m_addr;
    void readRegs(int addr, uint8_t * data, int len);
    void writeRegs(uint8_t * data, int len);
    int16_t getAccAxis(uint8_t addr);
    void Standby( void);
    void Active( void);
    void Fall_IRQ( void);
    void Motion_IRQ( void);
    void Orientation_IRQ( void);
    void DataReady_IRQ( void);
    //
    unsigned char OrientationState;
    unsigned char OrientationStateUpdated;
};

#endif
