/*
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC18F27Q43
        Driver Version    :  2.00
*/


#include "mcc_generated_files/mcc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>


// Easy delay function
void short_delay(unsigned int value);
void short_delay(unsigned int value)
{
    unsigned int i,j;
    for (i=0; i<value; i++)
    {
        j=i; // Basically just takes advantage of clock cycles to make a delay function
        // Loop takes an amount of time dependant on amount of loops set by "value"
    }
}

// Initialize ports for I2C communication
void i2c_init(void);
void i2c_init(void)
{
    SDA1_OD = 1;                //enable open-derain
    SDA1_ANS = 0;               //disable analog input, set as digital I/O only
    
    SCL1_SetHigh();             //set SCL as high
    SDA1_SetLow();              //set SDA as low so when output is enabled it will be pulled low
    SCL1_SetDigitalOutput();    //set SCL as output, TTL driven
    SDA1_SetDigitalInput();     //set SDA as input, which allows line to be pulled high with pull-up resistors
    
    //I2C_DELAY
    short_delay(1);
}

// Initiating an I2C start condition:
void i2c_start_condition( void );
void i2c_start_condition( void )
{
    SDA1_SetDigitalInput(); // Start condition begins with setting SDA line high in preparation for transition during clock high
    short_delay(1); // Wait to avoid accidental/unintentional action
    SCL1_SetHigh(); // Clock line goes high in preparation for start condition to be made
    short_delay(1); // Another delay to avoid accidental/unintentional action
    SDA1_SetDigitalOutput(); // Setting SDA to output pulls it low. Transitioning SDA High-Low while SCL is high creates start condition recognized by each device
    short_delay(1); // Another delay to avoid accidental/unintentional action
    SCL1_SetLow(); // Clock sent low again to be ready for next transaction
    short_delay(1); // Another delay to ensure next action does not begin too soon after clock is set low again
}

// Initiating an I2C stop condition:
void i2c_stop_condition( void );
void i2c_stop_condition( void )
{
    SDA1_SetDigitalOutput(); // Stop condition begins with setting SDA line low in preparation for transition during clock high 
    short_delay(1); // Wait to avoid accidental/unintentional action
    SCL1_SetHigh(); // Clock line goes high in preparation for stop condition to be made
    short_delay(1); // Another delay to avoid accidental/unintentional action
    SDA1_SetDigitalInput(); // Setting SDA to input pulls it high. Transitioning SDA Low-High while SCL is high creates stop condition recognized by each device
    short_delay(1); // Another delay to ensure next action does not begin too soon after clock is set low again
}

// Writing a bit in I2C:
void i2c_write_bit( uint8_t b );
void i2c_write_bit( uint8_t b )
{
    if( b > 0 ) 
    {
        SDA1_SetDigitalInput(); // Setting as input lets line go, which is pulled high by pull-up resistors, sending 1 if b is not zero
    }
    else 
    {
        SDA1_SetDigitalOutput();
    }

    short_delay(1);
    SCL1_SetHigh();
    short_delay(1);
    SCL1_SetLow();
    short_delay(1);
}

// Reading a bit in I2C:
uint8_t i2c_read_bit( void );
uint8_t i2c_read_bit( void )
{
    uint8_t b;
    //I2C_SET_SDA
    SDA1_SetDigitalInput(); // Setting as input lets line go, which is pulled high by pull-up resistors
    short_delay(1); // Delay to avoid accidental start/stop conditions or unintentional bit sending
    //I2C_SET_SCL
    SCL1_SetHigh(); // Sets SCL line high
    short_delay(1);
    
    if( SDA1_GetValue() ) // Read state of data line
    {
        b = 1; // Bit state when read
    }
    else 
    {
        b = 0; // Bit state when read
    }
    
    SCL1_SetLow(); // Set clock low to end transaction

    return b; // Returns read bit status
}

// Writing a byte with I2C:
bool i2c_write_byte( uint8_t B, bool start, bool stop );
bool i2c_write_byte( uint8_t B, bool start, bool stop )
{
    uint8_t ack = 0;

    if( start )
    {
        i2c_start_condition(); // Writing a byte begins by sending a start condition if desired
    }

    uint8_t i;
    for( i = 0; i < 8; i++ ) // Loop 8 times to write 8 bits
    {
        i2c_write_bit( B & 0x80 ); // writes a 1 bit if the next address bit in B is a 1, otherwise writes a 0
        B <<= 1; // Shifts out MSB after writing it to be ready to send next bit
    }

    ack = i2c_read_bit(); // Reads a single bit after the byte is sent, this represents the ack/nack bit that the peripheral will (or won't) respond with
    short_delay(1);
    if( stop ) 
    {
        i2c_stop_condition(); // Sends a stop condition if desired 
    }
    
    return ack; // Function returns the ack/nack bit state, in the state that it is read in (ack = 0, nack = 1)
    // Will be compensated for in the code that calls this function
}

// Reading a byte with I2C:
uint8_t i2c_read_byte( bool ack, bool stop );
uint8_t i2c_read_byte( bool ack, bool stop )
{
    uint8_t B = 0;

    uint8_t i;
    for( i = 0; i < 8; i++ )
    {
        B <<= 1;
        B |= i2c_read_bit();
    }

    if( ack )
    {
        i2c_write_bit(0);
    }
    else 
    {
        i2c_write_bit(1);
    }

    if( stop ) 
    {
        i2c_stop_condition();
    }
    
    return B;
}


//convert 2 ASCII char to hex byte
unsigned char ASCII_hex(unsigned char FN, unsigned char SN);
unsigned char ASCII_hex(unsigned char FN, unsigned char SN)
{
    
    unsigned char num2digit, cFN, cSN;
    if (FN >= 'A')
        cFN = (FN - 'A' + 10) * 16;
    else
        cFN = (FN - '0') * 16;
    
    if (SN >= 'A')
        cSN = SN - 'A' + 10;
    else
        cSN = SN - '0';
    
    num2digit = cFN + cSN;
    return num2digit;
}

//convert 3 ASCII char to hex byte
//note: this does not check for roll over
unsigned char ASCII_hex_3(unsigned char FN, unsigned char SN, unsigned char TN);
unsigned char ASCII_hex_3(unsigned char FN, unsigned char SN, unsigned char TN)
{
    unsigned char num3digit;
    num3digit = (unsigned char)(((FN & 0x0F)*100) + ((SN & 0x0F)*10) + (TN & 0x0F));
    return num3digit;
}
////////////////////////////////////////////////////////////////////////////////////////


// INA260 Specific Receive Byte Data from Registers
uint16_t i2c_receive_byte_ina( uint8_t address, uint8_t reg );
uint16_t i2c_receive_byte_ina( uint8_t address, uint8_t reg )
{   // Initiate Communication by sending out address and register
    uint16_t two_byte_register = 0;
    float reading;
    if( !i2c_write_byte( address << 1, true, false ) )   // If ACK, Send address with R/W bit Low), Start condition, No Stop
    {
        if( !i2c_write_byte( reg, false, true ) )   // If ACK, Send desired register, No Start/Stop condition
        {      
            // Initiate Read by sending out address and receiving register contents
            
            if( !i2c_write_byte( ( address << 1) | 0x01, true, false ) )   // If ACK, Send address left shifted by 1 with R/W bit High, Start, No Stop
            {
                uint8_t i2c_msb = i2c_read_byte( true, false );     // Read MSB byte, send ACK, No Stop Condition
                uint8_t i2c_lsb = i2c_read_byte( false, true );     // Read LSB byte, No ACK, Stop Condition
                
                //printf("\n\rMSB output = 0x%2X", i2c_msb);  // Printing out hex value in MSB register
                //printf("\n\rLSB output = 0x%2X", i2c_lsb);  // Printing out hex value in LSB register
                
                two_byte_register = ((i2c_msb << 8) + i2c_lsb);
                
                //printf("\n\rRegister Output = 0x%4X", two_byte_register); // Read out register contents
                //printf("\n\rRegister Output 1's compliment = 0x%4X", ~two_byte_register); // Print out 1's compliment of register contents
            }
        }
    }

    i2c_stop_condition();
    return two_byte_register;   // return zero if NACKed
}

// NOTE: Configuration byte data can be found in INA260 datasheet pg 22.
// Master configuration command that allows for full modification of config register
void i2c_config_byte_ina( uint8_t address, uint8_t config_msb, uint8_t config_lsb );
void i2c_config_byte_ina( uint8_t address, uint8_t config_msb, uint8_t config_lsb )
{   // Initiate Communication by sending out address and register
    if( !i2c_write_byte( address << 1, true, false ) )   // If ACK, Send address with R/W bit Low), Start condition, No Stop
    {
        if( !i2c_write_byte( 0x00, false, false ) )   // If ACK, Send config register, No Start/Stop condition
        {      
            if( !i2c_write_byte( config_msb, false, false ) )
            {    
                i2c_write_byte( config_lsb, false, false );
            }    
        }
    }

    i2c_stop_condition();
}

// Set configuration register for 8.8ms sampling, averages 4 readings
void i2c_config_10ms_set_ina( uint8_t address);
void i2c_config_10ms_set_ina( uint8_t address)
{   // Initiate Communication by sending out address and register
    if( !i2c_write_byte( address << 1, true, false ) )   // If ACK, Send address with R/W bit Low), Start condition, No Stop
    {
        if( !i2c_write_byte( 0x00, false, false ) )   // If ACK, Send config register, No Start/Stop condition
        {      
            if( !i2c_write_byte( 0x63, false, false ) ) // MSB set
            {    
                i2c_write_byte( 0x27, false, false ); // LSB set
            }    
        }
    }

    i2c_stop_condition();
}

// Set configuration register for 150.528 sampling, averages 128 readings with 588uS conversion time
void i2c_config_150ms_set_ina( uint8_t address);
void i2c_config_150ms_set_ina( uint8_t address)
{   // Initiate Communication by sending out address and register
    if( !i2c_write_byte( address << 1, true, false ) )   // If ACK, Send address with R/W bit Low), Start condition, No Stop
    {
        if( !i2c_write_byte( 0x00, false, false ) )   // If ACK, Send config register, No Start/Stop condition
        {      
            if( !i2c_write_byte( 0x68, false, false ) ) // MSB set
            {    
                i2c_write_byte( 0xDF, false, false ); // LSB set
            }    
        }
    }

    i2c_stop_condition();
}

// RESET configuration register to default values
void i2c_config_reset_ina( uint8_t address);
void i2c_config_reset_ina( uint8_t address)
{   // Initiate Communication by sending out address and register
    if( !i2c_write_byte( address << 1, true, false ) )   // If ACK, Send address with R/W bit Low), Start condition, No Stop
    {
        if( !i2c_write_byte( 0x00, false, false ) )   // If ACK, Send config register, No Start/Stop condition
        {      
            if( !i2c_write_byte( 0xE1, false, false ) ) // MSB set
            {    
                i2c_write_byte( 0x27, false, false ); // LSB set
            }    
        }
    }

    i2c_stop_condition();
}



/*
                         Main application
 */
void main(void)
{
    
    unsigned char menu_state = 0, input;
    uint8_t inputData;
    unsigned char FN, SN, TN, HN, goCharacter;
    bool ws;
    uint8_t i2c_addr, i2c_register, i2c_msb, i2c_lsb;
    uint16_t config;
    float voltage = 0;
    int loop;

    
    
    SYSTEM_Initialize();                    // Initialize the device

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();      // Enable the Global Interrupts


    /**
     * This program uses an auxiliary UART to USB (FTDI) module to display diagnostic information, run tests, and manually control the board
     * The following few lines display board information on boot up.
     * The menu system is alway active and an FTDI can be plugged in at any time (assuming the header is installed).
     * The menu system does not impose any performance reductions by being active.  
     * As long as command line inputs are not received it remains quiescent. 
     */
    printf("\fEdge Embedded Data Forwarder Test Device");
    printf("\r\n\tRevision: 1v0");
    printf("\r\n\tRevision Date: " __DATE__);
    printf("\r\n\tRevision Time: " __TIME__);
    printf("\r\n(C)Spring 2022 Elias Timmons, Rowan University");
    
    i2c_init();
    short_delay(1);


    while (1)
    {
        
            //Display Menu
        if (menu_state==0)      //if menu has not been presented, write it out
        {
            while(UART1_is_rx_ready())
            {
                input = UART1_Read();      //purge the input buffer
            }

           printf("\n\n\n\rController Menu\r\n");
           
           printf("\r\n\n\t   Current and Voltage Measurements");
           printf("\r\n\tE: Display INA Data");
           printf("\r\n\tF: Display Data At Location");
           printf("\r\n\tG: Write to INA Configuration Register");
           printf("\r\n\tH: Config Data preset for 8.8ms averaging");
           printf("\r\n\tR: RESET Config Data");
           
           printf("\r\n\n\tX: Continuous Readout");
           
           printf("\r\n\n\tI/O: Relay 0 On/Off");
           printf("\r\n\n\tK/L: Relay 1 On/Off");
           
           printf("\r\n\nEnter a menu choice: ");
           menu_state=1;       //set the menu state as presented
        }

        if(UART1_is_rx_ready())
        {
            menu_state=0;                //once we get a character we will need to repaint the menu when done
            input = UART1_Read();      //get the command byte

            switch(tolower(input))       //parse the command byte
            {          
                
                case 'i':                //turn ON Relay0
                    Relay0_SetHigh(); 
                    break;

                case 'o':                //turn OFF Relay0
                    Relay0_SetLow();
                    break;
                
                case 'k':                //turn ON Relay1
                    Relay1_SetHigh(); 
                    break;

                case 'l':                //turn OFF Relay1
                    Relay1_SetLow();
                    break;
                
                
                case 'e':
                    printf("\n\rEnter the I2C address:");
                    FN = UART1_Read();               //get 1st digit
                    SN = UART1_Read();               //get 2nd digit
                    i2c_addr = ASCII_hex(FN,SN);        //convert the ASCII input to hex value
                    printf("\n\rI2C Address = 0x%2X", i2c_addr);
                    
                    
                    float voltage = i2c_receive_byte_ina(i2c_addr, 0x02) * 0.00125;

                    if ((i2c_receive_byte_ina(i2c_addr, 0x01) & 0x8000) == 0)
                    {
                        printf("\n\rCurrent Loop Positive = %f A", i2c_receive_byte_ina(i2c_addr, 0x01) * 0.00125);
                        // If byte & 0x80 results in 0, the value in the address is positive and can be read
                    }
                    else
                    {
                        printf("\n\rCurrent Loop Negative = %f A", ((~i2c_receive_byte_ina(i2c_addr, 0x01) - 0x0001)) * -1 * 0.001250);
                        // Else byte is negative, so it must be inverted and 1 added to reverse the two's compliment formatting
                    }
                    
                    float power = i2c_receive_byte_ina(i2c_addr, 0x03) * 0.01;
                    
                    printf("\n\rVoltage = %f V", voltage);

                    printf("\n\rPower = %f W", power);
                    
                    if (ws == 0)
                    {
                        printf("\n\rI2C ERROR INA Receive");
                    }
                    short_delay(1);
                    
                    break;
                    
                // Current Configuration Readout    
                case 'f':
                    printf("\n\rEnter the I2C address:");
                    FN = UART1_Read();               //get 1st digit
                    SN = UART1_Read();               //get 2nd digit
                    i2c_addr = ASCII_hex(FN,SN);        //convert the ASCII input to hex value
                    printf("\n\rI2C Address = 0x%2X", i2c_addr);
                   
                    printf("\n\rReceive Config Byte Check = 0x%2x", i2c_receive_byte_ina(i2c_addr, 0x00));
                    
                    if (ws == 0)
                    {
                        printf("\n\rI2C ERROR Data/Location Receive");
                    }
                    short_delay(1);
                    
                    break;
                    
                // Master Write to Configuration Register
                case 'g':
                    printf("\n\rEnter the I2C address:");
                    FN = UART1_Read();               //get 1st digit
                    SN = UART1_Read();               //get 2nd digit
                    i2c_addr = ASCII_hex(FN,SN);        //convert the ASCII input to hex value
                    printf("\n\rI2C Address = 0x%2X", i2c_addr);
                   
                    printf("\n\rReceive Config Byte Check = 0x%2x", i2c_receive_byte_ina(i2c_addr, 0x00));
                    
                    if (ws == 0)
                    {
                        printf("\n\rI2C ERROR Data/Location Receive");
                    }
                    short_delay(1);
                    
                    printf("\n\rEnter 2 Bytes to Write (4 hex values):");
                    FN = UART1_Read();               //get 1st digit
                    SN = UART1_Read();               //get 2nd digit
                    TN = UART1_Read();               //get 3rd digit
                    HN = UART1_Read();               //get 4th digit
                    i2c_msb = ASCII_hex(FN,SN);        //convert the ASCII input to hex value
                    i2c_lsb = ASCII_hex(TN,HN);        //convert the ASCII input to hex value
                    
                    config = ((i2c_msb << 8) + i2c_lsb);
                    printf("\n\rConfig Byte Entered = 0x%4x", config);
                    
                    i2c_config_byte_ina(i2c_addr, i2c_msb, i2c_lsb);
                    
                    short_delay(1);
                    
                    printf("\n\rConfig Byte Check = 0x%2x", i2c_receive_byte_ina(i2c_addr, 0x00));
                    
                    break;
                    
                // Preset Configuration Data for 8.8ms Sampling
                case 'h':
                    printf("\n\rEnter the I2C address:");
                    FN = UART1_Read();               //get 1st digit
                    SN = UART1_Read();               //get 2nd digit
                    i2c_addr = ASCII_hex(FN,SN);        //convert the ASCII input to hex value
                    printf("\n\rI2C Address = 0x%2X", i2c_addr);
                    
                    printf("\n\rInitial Config Byte Check = 0x%2x", i2c_receive_byte_ina(i2c_addr, 0x00));
                    
                    i2c_config_10ms_set_ina(i2c_addr);

                    short_delay(1);
                    
                    printf("\n\rUpdated Config Byte Check = 0x%2x", i2c_receive_byte_ina(i2c_addr, 0x00));
                    
                    break;    
                    
                // Configuration Register Reset (Same as Power Cycling)
                case 'r':
                    printf("\n\r(RESET) I2C address of device (RESET):");
                    FN = UART1_Read();               //get 1st digit
                    SN = UART1_Read();               //get 2nd digit
                    i2c_addr = ASCII_hex(FN,SN);        //convert the ASCII input to hex value
                    printf("\n\rI2C Address = 0x%2X", i2c_addr);
                    
                    printf("\n\rInitial Config Byte Check = 0x%2x", i2c_receive_byte_ina(i2c_addr, 0x00));
                    
                    i2c_config_reset_ina(i2c_addr);

                    short_delay(1);
                    
                    printf("\n\rRESET Byte Check = 0x%2x", i2c_receive_byte_ina(i2c_addr, 0x00));
                    
                    break;    
                    
                    
                case 'x':
                    printf("\n\rEnter the I2C address:");
                    FN = UART1_Read();               //get 1st digit
                    SN = UART1_Read();               //get 2nd digit
                    i2c_addr = ASCII_hex(FN,SN);        //convert the ASCII input to hex value
                    printf("\n\rI2C Address = 0x%2X", i2c_addr);
                                          
                    for (loop = 1; loop < 1001; loop++)
                    {
                        printf("\n\r%i", loop);
                    if ((i2c_receive_byte_ina(i2c_addr, 0x01) & 0x8000) == 0)
                    {
                        printf(",%f", i2c_receive_byte_ina(i2c_addr, 0x01) * 0.00125);
                        // If byte & 0x80 results in 0, the value in the address is positive and can be read
                    }
                    else
                    {
                        printf(",%f", ((~i2c_receive_byte_ina(i2c_addr, 0x01) - 0x0001)) * -1 * 0.001250);
                        // Else byte is negative, so it must be inverted and 1 added to reverse the two's compliment formatting
                        //printf("\n\rCurrent Byte = 0x%2X", i2c_receive_byte_ina(i2c_addr, 0x01));
                    }
                    voltage = i2c_receive_byte_ina(i2c_addr, 0x02) * 0.00125;
                    printf(",%f", voltage);
                    power = i2c_receive_byte_ina(i2c_addr, 0x03) * 0.01;
                    printf(",%f", power);
                    
                    
                    }
                   
                    if (ws == 0)
                    {
                        printf("\n\rI2C ERROR INA Receive");
                    }
                    short_delay(1);
                    
                    break;    
                    
                    
                    
                default:
                    printf("\r\n\r\nInvalid command.\r\n\r\n");
                    break;
            } //end switch input
        } //end UART ready 
    } //end while
}
/**
 End of File
*/