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

#include "MMA8451Q.h"

#define REG_STATUS        0x00
#define REG_WHO_AM_I      0x0D
#define REG_CTRL_REG_1    0x2A
#define REG_CTRL_REG_2    0x2B
#define REG_CTRL_REG_4    0x2D
#define REG_CTRL_REG_5    0x2E
#define REG_INT_SRC       0x0C
#define REG_FF_MT_CFG     0x15
#define REG_FF_MT_SRC     0x16
#define REG_FF_MT_THS     0x17
#define REG_FF_MT_CNT     0x18
#define REG_DBCNTM        0x11
#define REG_DBNCE         0x12
#define REG_BKFR          0x13
#define REG_P_L_THS       0x14
#define REG_PL_STATUS     0x10

//
#define REG_OUT_X_MSB     0x01
#define REG_OUT_Y_MSB     0x03
#define REG_OUT_Z_MSB     0x05

#define UINT14_MAX        16383

//
#define ZYXDR           0x08
#define ZDR             0x04
#define YDR             0x02
#define XDR             0x01

/** Interrupt schema
*
* :: The FreeFall and Motion detection share the same IRQ2. 
* 
*   FreeFall --+                             +-- Fall_IRQ -----+
*               \                           /                   \
*                +-- MMA8451Q_Int2.fall ---+                     +--- MMA8451Q_usr2_fptr
*               /                           \                   /
*   Motion ----+                             +-- Motion_IRQ ---+
*   
* :: The Orientation Detect use the IRQ1
* 
*   Orientation Detect -- MMA8451Q_Int1.fall --- Orientation_IRQ --- MMA8451Q_usr1_fptr
*
*
* :: The data ready use the IRQ2
*
*   Data Ready -- MMA8451Q_Int2.fall --- DataReady_IRQ --- usr2_fptr
*
*/
void (*MMA8451Q_usr2_fptr)(void);               // Pointers to user function called after
void (*MMA8451Q_usr1_fptr)(void);               // IRQ assertion.

//
InterruptIn MMA8451Q_Int1( PTA14);      // INT1
InterruptIn MMA8451Q_Int2( PTA15);      // INT2

MMA8451Q::MMA8451Q(PinName sda, PinName scl, int addr) : m_i2c(sda, scl), m_addr(addr) {
    
    MMA8451Q_Int1.fall( NULL);
    MMA8451Q_Int2.fall( NULL);
    MMA8451Q_usr2_fptr = NULL;
    MMA8451Q_usr1_fptr = NULL;    

    Reset();    
    Active();    
}

MMA8451Q::~MMA8451Q() 
{
     MMA8451Q_Int1.fall( NULL);
     MMA8451Q_Int2.fall( NULL);
     MMA8451Q_usr2_fptr = NULL;
     MMA8451Q_usr1_fptr = NULL;
}

void MMA8451Q::Reset( void)
{
    // Soft reset
    uint8_t data[2] = {REG_CTRL_REG_2, 0x40};
    writeRegs(data, 2);
    wait( 0.1);
}

void MMA8451Q::FreeFallDetection( void(*fptr)(void))
{
    // Soft Reset
    Reset();
    
    // Example Steps for Configuring Linear Freefall Detection
    // X AND Y AND Z < 0.2g using MFF Function, 50 Hz ODR
    // Step 1: Put the device in Standby Mode: Register 0x2A CTRL_REG1
    unsigned char data[2] = {REG_CTRL_REG_1, 0x20};
    writeRegs(data, 2);
    
    // Step 2: Configuration Register set for Freefall Detection enabling “AND” condition, OAE = 0, Enabling X,
    // Y, Z and the Latch
    data[0] = REG_FF_MT_CFG;
    data[1] = 0x01;
    writeRegs(data, 2);

    // Step 3: Threshold Setting Value for the resulting acceleration < 0.2g
    // Note: The step count is 0.063g/count
    // • 0.2g/0.063g = 3.17 counts //Round to 3 counts
    data[0] = REG_FF_MT_THS;
    data[1] = 0x03;
    writeRegs(data, 2);

    // Step 4: Set the debounce counter to eliminate false positive readings for 50Hz sample rate with a
    // requirement of 120 ms timer, assuming Normal Mode.
    // Note: 120 ms/20 ms (steps) = 6 counts
    data[0] = REG_FF_MT_CNT;
    data[1] = 0x06;
    writeRegs(data, 2);

    // Step 5: Enable Motion/Freefall Interrupt Function in the System (CTRL_REG4)
    data[0] = REG_CTRL_REG_4;
    data[1] = 0x04;
    writeRegs(data, 2);

    // Step 6: Route the Motion/Freefall Interrupt Function to INT2 hardware pin (CTRL_REG5)
    data[0] = REG_CTRL_REG_5;
    data[1] = 0x00;
    writeRegs(data, 2);
    
    // Step 7: Put the device in Active Mode, 50 Hz
    data[0] = REG_CTRL_REG_1;
    data[1] = 0x21;
    writeRegs(data, 2);
    
    MMA8451Q_usr2_fptr = fptr;
    MMA8451Q_Int2.fall( this, &MMA8451Q::Fall_IRQ);
}

void MMA8451Q::Fall_IRQ( void)
{
    unsigned char t;
    
    // Determine source of the interrupt by first reading the system interrupt
    readRegs( REG_INT_SRC, &t, 1);
    //
    if ( (t & 0x04) == 0x04) {
        // Read the Motion/Freefall Function to clear the interrupt
        readRegs( REG_FF_MT_SRC, &t, 1);
        // Run the user supplied function
        MMA8451Q_usr2_fptr();
    }
}

void MMA8451Q::MotionDetection( void(*fptr)(void))
{
    // Soft Reset
    Reset();
    
    // 6.1 Example Steps for Configuring Motion Detection
    // X or Y > 3g using MFF Function 4g, 100 Hz ODR, Normal Mode
    // Step 1: Put the device into Standby Mode: Register 0x2A CTRL_REG1
    unsigned char data[2] = {REG_CTRL_REG_1, 0x18}; // Set the device in 100 Hz ODR, Standby
    writeRegs(data, 2);

    
    // Step 2: Set Configuration Register for Motion Detection by setting the “OR” condition OAE = 1, enabling
    // X, Y, and the latch
    data[0] = REG_FF_MT_CFG;
    data[1] = 0xD8;
    writeRegs(data, 2);

    // Step 3: Threshold Setting Value for the Motion detection of > 2g
    // Note: The step count is 0.063g/ count
    // • 1g/0.063g = 15.8; //Round up to 16
    data[0] = REG_FF_MT_THS;
    data[1] = 0x10;
    writeRegs(data, 2);
    
    // Step 4: Set the debounce counter to eliminate false readings for 100 Hz sample rate with a requirement
    // of 100 ms timer.
    // Note: 100 ms/10 ms (steps) = 10 counts
    data[0] = REG_FF_MT_CNT;
    data[1] = 0x0A;
    writeRegs(data, 2);
    
    // Step 5: Enable Motion/Freefall Interrupt Function in the System (CTRL_REG4)
    data[0] = REG_CTRL_REG_4;
    data[1] = 0x04;
    writeRegs(data, 2);
    
    // Step 6: Route the Motion/Freefall Interrupt Function to INT2 hardware pin (CTRL_REG5)
    data[0] = REG_CTRL_REG_5;
    data[1] = 0x00;
    writeRegs(data, 2);
    
    // Step 7: Put the device in Active Mode
    data[0] = REG_CTRL_REG_1;
    data[1] = 0x19;
    writeRegs(data, 2);

    MMA8451Q_usr2_fptr = fptr;
    MMA8451Q_Int2.fall( this, &MMA8451Q::Motion_IRQ);

}

void MMA8451Q::Motion_IRQ( void)
{
    unsigned char t;
    
    // Determine source of the interrupt by first reading the system interrupt
    readRegs( REG_INT_SRC, &t, 1);
    //
    if ( (t & 0x04) == 0x04) {
        // Read the Motion/Freefall Function to clear the interrupt
        readRegs( REG_FF_MT_SRC, &t, 1);
        // Run the user supplied function
        MMA8451Q_usr2_fptr();
    }
}

void MMA8451Q::OrientationDetect( void(*fptr)(void))
{
    OrientationDetect( fptr, Z_LOCKOUT_14, Z_BKFR_80, PL_THS_15, PL_HYS_0);
}

void MMA8451Q::OrientationDetect( void(*fptr)(void), unsigned int Z_LockOut, unsigned int Z_BkFr, unsigned int PL_Thsld, unsigned int PL_Hyst)
{
    unsigned char t;

    // Soft Reset
    Reset();
        
    // Reset orientation value.
    OrientationState = 0;
    OrientationStateUpdated = 0;
    
    // Step 1: Put the part into Standby Mode
    Standby();
    
    // Step 2: Set the data rate to 50 Hz (for example, but can choose any sample rate).
    readRegs( REG_CTRL_REG_1, &t, 1);       // Note: Can combine this step with above
    t &= 0xC7;                             // Clear the sample rate bits
    t |= 0x20;                             // Set the sample rate bits to 50 Hz
    unsigned char data[2] = {REG_CTRL_REG_1, t};
    writeRegs(data, 2);                     // Write updated value into the register.   
    
    
    // Step 3: Set the PL_EN bit in Register 0x11 PL_CFG. This will enable the orientation detection.
    readRegs( REG_DBCNTM, &t, 1);
    data[0] = REG_DBCNTM;
    data[1] = t | 0x40;
    writeRegs(data, 2);
    
    // Step 4: Set the Back/Front Angle trip points in register 0x13 following the table in the data sheet.
    // NOTE: This register is readable in all versions of MMA845xQ but it is only modifiable in the
    // MMA8451Q.
    readRegs( REG_BKFR, &t, 1);
    t &= 0x3F;                      // Clear bit 7 and 6    
    data[0] = REG_BKFR;
    data[1] = t | Z_BkFr;
    writeRegs(data, 2);             // Write in the updated Back/Front Angle

    // Step 5: Set the Z-Lockout angle trip point in register 0x13 following the table in the data sheet.
    // NOTE: This register is readable in all versions of MMA845xQ but it is only modifiable in the
    // MMA8451Q.
    readRegs( REG_BKFR, &t, 1);
    t &= 0xF8;                      // Clear the last three bits of the register
    data[0] = REG_BKFR;
    data[1] = t | Z_LockOut;
    writeRegs(data, 2);             // Write in the updated Z-lockout angle
    
    // Step 6: Set the Trip Threshold Angle
    // NOTE: This register is readable in all versions of MMA845xQ but it is only modifiable in the
    // MMA8451Q.
    // Select the angle desired in the table, and,
    // Enter in the values given in the table for the corresponding angle.
    // Refer to Figure 7 for the reference frame of the trip angles.
    readRegs( REG_P_L_THS, &t, 1);
    t &= 0x07;                      // Clear the Threshold values
    data[0] = REG_P_L_THS;
    data[1] = t | (PL_Thsld<<3);
    writeRegs(data, 2);             
    
    // Step 7: Set the Hysteresis Angle
    // NOTE: This register is readable in all versions of MMA845xQ but it is only modifiable in the
    // MMA8451Q.
    // Select the hysteresis value based on the desired final trip points (threshold + hysteresis)
    // Enter in the values given in the table for that corresponding angle.
    // Note: Care must be taken. Review the final resulting angles. Make sure there isn’t a resulting trip value
    // greater than 90 or less than 0.
    // The following are the options for setting the hysteresis.
    readRegs( REG_P_L_THS, &t, 1);
    t &= 0xF8;                      // Clear the Hysteresis values
    data[0] = REG_P_L_THS;
    data[1] = t | PL_Hyst;
    writeRegs(data, 2);             
    
    // Step 8: Register 0x2D, Control Register 4 configures all embedded features for interrupt
    // detection.
    // To set this device up to run an interrupt service routine:
    // Program the Orientation Detection bit in Control Register 4.
    // Set bit 4 to enable the orientation detection “INT_EN_LNDPRT”.
    readRegs( REG_CTRL_REG_4, &t, 1);
    data[0] = REG_CTRL_REG_4;
    data[1] = t | 0x10;                 // Set bit 4
    writeRegs(data, 2);             
    
    // Step 9: Register 0x2E is Control Register 5 which gives the option of routing the interrupt to
    // either INT1 or INT2
    // Depending on which interrupt pin is enabled and configured to the processor:
    // Set bit 4 “INT_CFG_LNDPRT” to configure INT1, or,
    // Leave the bit clear to configure INT2.
    readRegs( REG_CTRL_REG_5, &t, 1);
    data[0] = REG_CTRL_REG_5;
    data[1] = t | 0x10;                 // Set bit 4 to choose the interrupt to route to INT1
    writeRegs(data, 2);             
    
    // Step 10: Set the debounce counter in register 0x12
    // This value will scale depending on the application-specific required ODR.
    // If the device is set to go to sleep, reset the debounce counter before the device goes to sleep. This setting
    // helps avoid long delays since the debounce will always scale with the current sample rate. The debounce
    // can be set between 50 ms - 100 ms to avoid long delays.
    data[0] = REG_DBNCE;
    data[1] = 0x05;                     // This sets the debounce counter to 100 ms at 50 Hz
    writeRegs(data, 2);             
    
    // Step 11: Put the device in Active Mode
    Active();
 
    MMA8451Q_usr1_fptr = fptr;
    MMA8451Q_Int1.fall( this, &MMA8451Q::Orientation_IRQ);

}

void MMA8451Q::Orientation_IRQ( void)
{
    unsigned char t;
    
    // Determine source of the interrupt by first reading the system interrupt
    readRegs( REG_INT_SRC, &t, 1);
    //
    if ( (t & 0x10) == 0x10) {
        // Read the PL State from the Status Register, clear the interrupt
        readRegs( REG_PL_STATUS, &t, 1);
        // Set the orientation state variable
        OrientationState = t;
        OrientationStateUpdated = 1;
        // Run the user supplied function
        MMA8451Q_usr1_fptr();
    }
}

unsigned char MMA8451Q::GetOrientationState( void)
{
    if ( OrientationStateUpdated) {
        OrientationStateUpdated = 0;
        return OrientationState;
    }
    //
    return 0;
}

void MMA8451Q::DataReady( void(*fptr)(void), unsigned char ODR)
{
    // Soft Reset
    Reset();
    
    // Step 1: Put the device into Standby Mode: Register 0x2A CTRL_REG1
    // Set the device ODR value and Standby
    unsigned char data[2] = {REG_CTRL_REG_1, ((ODR<<3) & 0xFE)};
    writeRegs(data, 2);

    // Step 2: Enable Data Ready Interrupt Function in the System (CTRL_REG4)
    data[0] = REG_CTRL_REG_4;
    data[1] = 0x01;
    writeRegs(data, 2);
    
    // Step 6: Route the Data Ready Interrupt Function to INT2 hardware pin (CTRL_REG5)
    data[0] = REG_CTRL_REG_5;
    data[1] = 0x00;
    writeRegs(data, 2);
    
    // Step 7: Put the device in Active Mode
    data[0] = REG_CTRL_REG_1;
    data[1] = ((ODR<<3) | 0x01);
    writeRegs(data, 2);

    MMA8451Q_usr2_fptr = fptr;
    MMA8451Q_Int2.fall( this, &MMA8451Q::DataReady_IRQ);

}

void MMA8451Q::DataReady_IRQ( void)
{
    unsigned char t;
    
    // Determine source of the interrupt by first reading the system interrupt
    readRegs( REG_INT_SRC, &t, 1);
    //
    if ( (t & 0x01) == 0x01) {
        // Read the DataReady_IRQ Function to clear the interrupt
        readRegs( REG_FF_MT_SRC, &t, 1);
        // Run the user supplied function
        MMA8451Q_usr2_fptr();
    }
}


void MMA8451Q::Active( void)
{
    unsigned char t;
    
    // Activate the peripheral
    readRegs(REG_CTRL_REG_1, &t, 1);
    unsigned char data[2] = {REG_CTRL_REG_1, t|0x01};
    writeRegs(data, 2);
}

void MMA8451Q::Standby( void)
{
    unsigned char t;
    
    // Standby
    readRegs(REG_CTRL_REG_1, &t, 1);
    unsigned char data[2] = {REG_CTRL_REG_1, t&0xFE};
    writeRegs(data, 2);
}

uint8_t MMA8451Q::getWhoAmI() {
    uint8_t who_am_i = 0;
    readRegs(REG_WHO_AM_I, &who_am_i, 1);
    return who_am_i;
}

float MMA8451Q::getAccX() {
    return (float(getAccAxis(REG_OUT_X_MSB))/4096.0);
}

float MMA8451Q::getAccY() {
    return (float(getAccAxis(REG_OUT_Y_MSB))/4096.0);
}

float MMA8451Q::getAccZ() {
    return (float(getAccAxis(REG_OUT_Z_MSB))/4096.0);
}

void MMA8451Q::getAccAllAxis(float * res) {
    res[0] = getAccX();
    res[1] = getAccY();
    res[2] = getAccZ();
}

int16_t MMA8451Q::getAccAxis(uint8_t addr) {
    int16_t acc;
    uint8_t res[2];
    readRegs(addr, res, 2);

    acc = (res[0] << 6) | (res[1] >> 2);
    if (acc > UINT14_MAX/2)
        acc -= UINT14_MAX;

    return acc;
}

unsigned int MMA8451Q::getAccRawAllAxis( int16_t * res) 
{
    if ( isDataAvailable() & ZYXDR) 
    {
        getAccRawX( &res[0]);
        getAccRawY( &res[1]);
        getAccRawZ( &res[2]);
        return 1;
    } else
        return 0;
}

int16_t MMA8451Q::getAccRawX( int16_t * res) 
{
    if ( isDataAvailable() & XDR) 
    {
        *res = getAccAxis(REG_OUT_X_MSB);
        return 1;
    } else
        return 0;        
}

int16_t MMA8451Q::getAccRawY( int16_t * res) 
{
    if ( isDataAvailable() & YDR) 
    {
        *res = getAccAxis(REG_OUT_Y_MSB);
        return 1;
    } else
        return 0;        
}

int16_t MMA8451Q::getAccRawZ( int16_t * res) 
{
    if ( isDataAvailable() & ZDR) 
    {
        *res = getAccAxis(REG_OUT_Z_MSB);
        return 1;
    } else
        return 0;        
}

unsigned int MMA8451Q::isDataAvailable( void)
{
    unsigned char status;
    
    readRegs( REG_STATUS, &status, 1);

    return (status);
    
}

void MMA8451Q::readRegs(int addr, uint8_t * data, int len) {
    char t[1] = {addr};
    m_i2c.write(m_addr, t, 1, true);
    m_i2c.read(m_addr, (char *)data, len);
}

void MMA8451Q::writeRegs(uint8_t * data, int len) {
    m_i2c.write(m_addr, (char *)data, len);
}
