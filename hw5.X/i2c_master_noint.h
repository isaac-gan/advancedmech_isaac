#ifndef I2C_MASTER_NOINT_H__
#define I2C_MASTER_NOINT_H__
// Header file for i2c_master_noint.c
// helps implement use I2C1 as a master without using interrupts

#include <xc.h>

void i2c_master_setup(void); // set up I2C1 as master - set BAUD rate here. Contains BAUD
void i2c_master_start(void); // send a START signal (send start bits)
void i2c_master_restart(void); // send a RESTART signal (send restart bit  for read))
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void); // receive a byte of data
void i2c_master_ack(int val); // send an ACK (0) or NACK (1). When receiving need to acknowledge what we are reading 
void i2c_master_stop(void); // send a stop bit

#endif

/*Reading 
 * Call I2C master setup
 * START
 * SEND
 * STOP
 */


/*Writing
 * Call I2C master setup
 * START
 * SEND address for writing and register we want to read from
 * RESTART
 * SEND address w reading
 * REC
 * ACK
 * STOP
 */
