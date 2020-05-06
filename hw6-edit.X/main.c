#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include"font.h"

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

//void heartbeat(void);
void ssd1306_setup(void);
//void setPin(unsigned char address, unsigned char register_1, unsigned char value);
//unsigned char readPin(unsigned char address, unsigned char register_2);
void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color);
void ssd1306_update(void);
void drawChar(unsigned char x,unsigned char y,int letterinhex);
void drawMessage(unsigned char x,unsigned char y,unsigned char *message);
void checker(void);

//IMU Function Prototypes
unsigned char imu_setup(void);
unsigned char I2C_read_multiple(unsigned char address, unsigned char register_1, unsigned char * data, int length);
void print_bars_x (signed short x);
void print_bars_y (signed short y);

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
    i2c_master_setup();//so ssd1306 can use i2c comm protocol
    ssd1306_setup();
    imu_setup();
    __builtin_enable_interrupts();
    
    while (1) 
    {        
        /*Check that WHOAMI accessed correctly
        unsigned char b;
        b=imu_setup();
        int a=(int)(b);
        char message[50]; 
        sprintf(message, "WHOAMI=%d", a); //Part 3 prints correctly 
        drawMessage(128/2,24,message);*/
        
        signed short temp,g_x,g_y,g_z,a_x,a_y,a_z;
        char buffer[50];
        //signed short *pointer_2;
        unsigned char data[14],*pointer_1;
        pointer_1=data;
        //pointer_2=buffer;
       
        I2C_read_multiple(0b11010110,0x20,pointer_1,14);//reads all 14 register values, returns data
        
        //Check that read is correct
        /*temp =((data[1]<<8)|data[0]);
        sprintf(buffer,"T=%d",temp);
        drawMessage(0,0,buffer);*/
        ssd1306_drawPixel(128/2,32/2, 1);
        ssd1306_update();
        
        /*g_x=((data[3]<<8)|data[2]);
        g_y=((data[5]<<8)|data[4]);*/

        /*g_z=((data[7]<<8)|data[6]);
        sprintf(buffer,"g_z=%d",g_z);
        drawMessage(0,24,buffer);*/
        
        a_x=((data[9]<<8)|data[8]);
        /*sprintf(buffer,"a_x=%d",a_x);
        drawMessage(128/2,0,buffer);*/
        
        a_y=((data[11]<<8)|data[10]);
        /*sprintf(buffer,"a_y=%d",a_y);
        drawMessage(128/2,8,buffer);*/
        
        ssd1306_clear();
        print_bars_y(a_y);
        print_bars_x(a_x);
        ssd1306_update();
        
        /*a_z=((data[13]<<8)|data[12]);
        sprintf(buffer,"a_z=%d",a_z);
        drawMessage(128/2,16,buffer);*/
        
        //heartbeat();
        //checker();
        /*unsigned char i = '&';
        int help=i;*/
        /*drawChar(128/2,32/2,help);//character we want to print*/
    } 

}

/*void checker(){
    int a=0;
    while(1){
    _CP0_SET_COUNT(0); 
    char message[100]; 
    sprintf(message, "a=%d", a);
    drawMessage(80,23,message);
    ssd1306_update();
    float count=_CP0_GET_COUNT();
    char counter[100];
    sprintf(counter, "FPS=%6.2f", (48000000/count)); //Part 3 prints correctly 
    drawMessage(0,23,counter);
    a++;
    }
}*/

//Function to print char array generated by sprintf @ a position
void drawMessage(unsigned char x,unsigned char y,unsigned char *message)//pointer store the address of message
{
     int d = 0;
 
 while (message[d] != 0) {
     if (d <= 25) {
     drawChar((d*5) + x, 0 + y, message[d]);
 }
     else if (d > 25 && d <= 50) {
     drawChar(((d-25)*5) + x, 8 + y, message[d]);
 }
     else if (d > 50 && d <= 75) {
     drawChar(((d-50)*5) + x, 16 + y, message[d]);
 }
     else if (d > 75 && d <= 100) {
     drawChar(((d-75)*5) + x, 24 + y, message[d]);
 }
     d++;
    
                          }
}

//Function to draw a letter at a position using font.h
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

unsigned char I2C_read_multiple(unsigned char address, unsigned char register_1, unsigned char * data, int length)
{
    i2c_master_start();
     i2c_master_send(address);//Write to this specific slave address
     i2c_master_send(register_1);//read from GPIOB so specify it's address 0x13. Read all the pins on GPIOB
     i2c_master_restart();
     i2c_master_send(0b11010111);//Read from this specific slave at this address
     int i;
     for (i=0;i<(length-1);i++)
     {
     data[i] = i2c_master_recv();//data that we want transmitted back
     i2c_master_ack(0);//ok I got the message bruh
     }
     data[i+1]=i2c_master_recv();
     i2c_master_ack(1); //at this point we have unsigned char array read from IMU
     i2c_master_stop();
     return data; 
}
    
void print_bars_x (signed short x) {
    float bar;
    int i = 0;
    
    bar = (float)x/32767 * 32;
    
    if (bar >= 0){
    while (i <= bar){
        ssd1306_drawPixel (64 , 16 + i, 1);
        i ++;
      
    }
    } 
    else if (bar < 0) {
        bar = -bar;
        while (i <= bar){
        ssd1306_drawPixel (64 , 16 - i, 1);
        i ++;
      
    }
        
    }
      
}

void print_bars_y (signed short y) {
    float bar;
    int i = 0;
    
    bar = (float)y/32767 * 128;
    
    if (bar >= 0){
    while (i <= bar){
        ssd1306_drawPixel (64 - i , 16, 1);
        i ++;
      
    }
    } 
    else if (bar < 0) {
        bar = -bar;
        while (i <= bar){
        ssd1306_drawPixel (64 + i , 16, 1);
        i ++;
      
    }
        }
}