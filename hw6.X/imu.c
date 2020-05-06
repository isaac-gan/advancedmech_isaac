#include "imu.h"

void imu_setup(){
    unsigned char who = 0;

    //initialisation always read from IMU_WHOAMI
    //Read from IMU_WHOAMI
     i2c_master_start();
     i2c_master_send(0b01010110);//Write to this specific slave address
     i2c_master_send(0x0F);//read from WHOAMI at this add.
     i2c_master_restart();
     i2c_master_send(0b01010111);//Read from this specific slave at this address
     who = i2c_master_recv();//data that we want transmitted back
     i2c_master_ack(1);//ok I got the message bruh
     i2c_master_stop();    
    
    if (who != 0b01101001){ //if comes back w value means I2C is working
        while(1){
            heartbeat();
            }
    }
    //if it works then turn on resp functions
    // init IMU_CTRL1_XL
    
    // init IMU_CTRL2_G

    // init IMU_CTRL3_C - seq read bit

}

//void imu_read(unsigned char reg, signed short * data, int len){ //takes in start register we try to read from
    //create an array of signed shorts, that's where the data ends up
    //int len is the number of signed shorts
    // call read multiple from the imu - write as a function, once got those 14 unsigned chars, combine them to get 7 signed shorts. each data takes 2 reads so you need len*2 chars
    
    // turn the chars into the shorts