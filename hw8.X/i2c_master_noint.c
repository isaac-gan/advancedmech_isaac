// I2C Master utilities, using polling rather than interrupts
// The functions must be called in the correct order as per the I2C protocol
// I2C pins need pull-up resistors, 2k-10k
#include "i2c_master_noint.h"

/*void init_ssd1306_i2c(){//Initialisation of SSD1306 by sending configuration of SFRs
     i2c_master_setup();
     
     //Setting of IODIRA to 0x00
     i2c_master_start();
     i2c_master_send(0b01000000);//Writing
     i2c_master_send(0x00);//address of IODIRA
     i2c_master_send(0x00);//set content of IODIRA to 0x00 so that all A pins o/p
     i2c_master_stop();
     
     //Setting of IODIRB to 0XFF so it is i/p
     i2c_master_start();
     i2c_master_send(0b01000000);//Writing
     i2c_master_send(0x01);//address of IODIRB
     i2c_master_send(0xFF);//set content of IODIRB
     i2c_master_stop();
 }*/

void heartbeat(){ //blink LED as a form of debugging, reusing circuit from HW1
    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to set heartbeat frequency
        // remember the core timer runs at half the sysclk
        /*in the infinite loop, if the value of SCK goes low, 
         * turn on A4 for 0.5s, off for 0.5s, on for 0.5s, 
         * and off for 0.5s (so two one-second square wave blinks).*/        
        _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 1;
         }
         _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 0;
         }
         _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 1;
         }
         _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 0;
         }
                     
}

void i2c_master_setup(void) {
    // using a large BRG to see it on the nScope, make it smaller after verifying that code works
    // look up TPGD in the datasheet. BRG = Baud Rate Generator
    I2C1BRG = 16; // I2CBRG = [1/(2*Fsck) - TPGD]*Pblck - 2 (TPGD is the Pulse Gobbler Delay). Typical  baud rate is 100khz, 400khz and 1MHz, Usually set to 400khz.
    I2C1CONbits.ON = 1; // turn on the I2C1 module
}

void i2c_master_start(void) {
    I2C1CONbits.SEN = 1; // send the start bit
    while (I2C1CONbits.SEN) {
        ; 
    } // wait for the start bit to be sent
}

void i2c_master_restart(void) {
    I2C1CONbits.RSEN = 1; // send a restart 
    while (I2C1CONbits.RSEN) { //while loops  which say wait  until the thing trying to send or receive occurs
        ; //make heartbeat code LED stop blinking when this occurs
    } // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
    I2C1TRN = byte; // if an address, bit 0 = 0 for write, 1 for read
    while (I2C1STATbits.TRSTAT) {
        ;
    } // wait for the transmission to finish
    if (I2C1STATbits.ACKSTAT) { // if this is high, slave has not acknowledged
        // ("I2C1 Master: failed to receive ACK\r\n");
        while(1){heartbeat();  
                } // get stuck here if the chip does not ACK back. If chip does not ack that it received what was sent, PIC just hangs. Good indicator that comm needs to be power reset if PIC freezes. If notice LED is not blinking means got stuck in while loops and need to do power reset. 
    } //in code blinking LED as heartbeat, and if LED stops blinking means need to power reset
}
//hardwired RESET high

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C1CONbits.RCEN = 1; // start receiving data
    while (!I2C1STATbits.RBF) {
        ;
    } // wait to receive the data
    return I2C1RCV; // read and return the data
}

void i2c_master_ack(int val) { // sends ACK = 0 (slave should send another byte)
    // or NACK = 1 (no more bytes requested from slave)
    I2C1CONbits.ACKDT = val; // store ACK/NACK in ACKDT
    I2C1CONbits.ACKEN = 1; // send ACKDT
    while (I2C1CONbits.ACKEN) {
        ;
    } // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) { // send a STOP:
    I2C1CONbits.PEN = 1; // comm is complete and master relinquishes bus
    while (I2C1CONbits.PEN) {
        ;
    } // wait for STOP to complete
}