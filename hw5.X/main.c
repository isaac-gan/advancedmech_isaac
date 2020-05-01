#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include"ws2812b.h"

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

void ssd1306_setup(void);
//void setPin(unsigned char address, unsigned char register_1, unsigned char value);
//unsigned char readPin(unsigned char address, unsigned char register_2);
void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color);
void ssd1306_update(void);
//void drawChar(unsigned char x,unsigned char y,int letterinhex);
//void drawMessage(unsigned char x,unsigned char y,char *message);
//void checker(void);
void heartbeat(void);
void ws2812b_setup();
void ws2812b_setColor(wsColor * c, int numLEDs);
wsColor HSBtoRGB(float hue, float sat, float brightness);

/*wsColor red;
red=HSBtoRGB(0,1,0.1);
wsColor green;
green=HSBtoRGB(120,1,0.1);
wsColor blue;
blue=HSBtoRGB(240,1,0.1);
wsColor yellow;
yellow=HSBtoRGB(70,1,0.1);*/

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
    ws2812b_setup();    
    
    __builtin_enable_interrupts();
    
    while (1) 
    {
        //heartbeat();
        wsColor c[4];
        int i;
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()< 3000000)
        {
        //initial state of LED
        c[0] = HSBtoRGB(0,1,0.1); //red
        c[1] = HSBtoRGB(120,1,0.1);//green
        c[2] = HSBtoRGB(240,1,0.1);//blue
        c[3] = HSBtoRGB(70,1,0.1);//yellow
        int numLEDs=4;
        wsColor *pointer;//pointer check
        pointer=&c;
        ws2812b_setColor(pointer,numLEDs);
        }
        
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()< 3000000)
        {
        //initial state of LED
        c[0] = HSBtoRGB(70,1,0.1); 
        c[1] = HSBtoRGB(0,1,0.1);
        c[2] = HSBtoRGB(120,1,0.1);
        c[3] = HSBtoRGB(240,1,0.1);
        int numLEDs=4;
        wsColor *pointer;//pointer check
        pointer=&c;
        ws2812b_setColor(pointer,numLEDs);
        }
        
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()< 3000000)
        {
        //initial state of LED
        c[0] = HSBtoRGB(240,1,0.1); 
        c[1] = HSBtoRGB(70,1,0.1);
        c[2] = HSBtoRGB(0,1,0.1);
        c[3] = HSBtoRGB(120,1,0.1);
        int numLEDs=4;
        wsColor *pointer;//pointer check
        pointer=&c;
        ws2812b_setColor(pointer,numLEDs);
        }
        
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()< 3000000)
        {
        //initial state of LED
        c[0] = HSBtoRGB(120,1,0.1); 
        c[1] = HSBtoRGB(240,1,0.1);
        c[2] = HSBtoRGB(70,1,0.1);
        c[3] = HSBtoRGB(0,1,0.1);
        int numLEDs=4;
        wsColor *pointer;//pointer check
        pointer=&c;
        ws2812b_setColor(pointer,numLEDs);
        }  
    } 
}

void heartbeat(void){ //blink LED + screen        
        _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 1;
             ssd1306_drawPixel(128/2,16,1);  

         }
         _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 0;
             ssd1306_drawPixel(128/2,16,0);
         }
         _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 1;
             ssd1306_drawPixel(128/2,16,1);
         }
         _CP0_SET_COUNT(0);
         while (_CP0_GET_COUNT()< 12000000){
             LATAbits.LATA4 = 0;
             ssd1306_drawPixel(128/2,16,0);
         }                     
}
