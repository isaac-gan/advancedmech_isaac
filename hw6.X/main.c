//HW6-Original
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include"font.h" //in order to print letters

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

void heartbeat(void);
void init_mcp23017(void);
void setPin(unsigned char address, unsigned char register_1, unsigned char value);
unsigned char readPin(unsigned char address, unsigned char register_2);

//SSD1306 Functions
void drawChar(unsigned char x,unsigned char y,int letterinhex);
void ssd1306_setup(void);

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts - diff styles of interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4=0;
    TRISBbits.TRISB4=1;//controls whether pin is i/p or o/p
    
    //LATAbits.LATA4=0;
    __builtin_enable_interrupts();
    i2c_master_setup();//so ssd1306 can use i2c comm protocol
    init_mcp23017();
    imu_setup();

    while (1) {
    _CP0_SET_COUNT(0);
    LATAbits.LATA4=!LATAbits.LATA4;
    
    /*imu_read(IMU_OUT_TEMP_L,imu,7);
    /*if (0){ //read all data and print to screen to make sure values look correct
        sprintf(message,"g:%d %d %d", imu[1],imu[2],imu[3]);
        drawString(message,0,0);
        sprintf(message,"a:%d %d %d",imu[4],imu[5],imu[6]);
        drawString(message,0,8);
        sprintf(message,"t:%d",imu[0]);
        drawString(message,0,16);
    }
    
    else //take data and interpret as pixels and draw them as bars
        bar_x(-imu[5],1);//draws inclinometer, tells us which way is down
        bar_y(imu[4],1);//take accel numbers from x and y and convert to bars
    
    }*/
    
    /*ssd1306_update();*/
    
    /*while (_CP0_GET_COUNT()<48000000/2/20) {
    }*/
        /*Parts from HW3
        unsigned char button_state;
        int button_state_reduced;
        button_state= readPin(0b01000000,0x13);//read state of GPB0. If GPB0 is low, turn on LED. I2C control byte format, then GPIOB register address
        //LATAbits.LATA4=1;  
        button_state_reduced = button_state%2;
        //LATAbits.LATA4=button_state_reduced;  
        if (!button_state_reduced){
        setPin(0b01000000,0x14,0xFF);//turn on LED, Address of SFR OLATA,value that sets no 7 to high 0b10000000
        }
        else
            {setPin(0b01000000,0x14,0x00);//turn off LED
            }
         */
    }
}
    
void setPin(unsigned char address, unsigned char register_1, unsigned char value){
    i2c_master_start();
    i2c_master_send(address);//writing to this specific slave address
    i2c_master_send(register_1);//address of the register inside MCP peripheral
    i2c_master_send(value);//set OL7 bit to high
    i2c_master_stop();
}

unsigned char readPin(unsigned char address, unsigned char register_2)
{
     i2c_master_start();
     i2c_master_send(address);//Write to this specific slave address
     i2c_master_send(register_2);//read from GPIOB so specify it's address 0x13. Read all the pins on GPIOB
     i2c_master_restart();
     i2c_master_send(0b01000001);//Read from this specific slave at this address
     unsigned char data = i2c_master_recv();//data that we want transmitted back
     i2c_master_ack(1);//ok I got the message bruh
     i2c_master_stop();
     return data;//pass this uchar up to main will process in main
}

 void init_mcp23017(){//Initialisation of MCP23017 by sending configuration of SFRs
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
 }
 
 void drawChar(unsigned char x,unsigned char y,int letterinhex)
{
    //letter[8];
    int letter,j,k,m;
    letter = 0xFF&letterinhex; //for eg letter is 0x21
    //sprintf(letter,"%d",character);
    for (j=0;j<=4;j++)//j refers to column number
        {//now we have the specified row and column in ASCII array
        for (k=0;k<=7;k++)//loops through hex number. We perform bitwise & to extract the LSB integer value for each of the 8 bits.
            {
            m=((ASCII[letter-0x20][j])>>k)&1;
            ssd1306_drawPixel(j+x,k+y,m);//x,y,color 
            ssd1306_update();
            }
    }
}

 void heartbeat(void){ //blink LED + screen        
        _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 1;
             ssd1306_drawPixel(10,10,1);
         }
         _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 0;
             ssd1306_drawPixel(10,10,0);
         }
         _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 1;
             ssd1306_drawPixel(10,10,1);
         }
         _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 0;
             ssd1306_drawPixel(10,10,0);
         }                     
}