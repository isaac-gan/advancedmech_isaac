#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>

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

#define NUMSAMPS 2000
#define TWO_PI 3.14159*2

//Global variable definition
static volatile int sinewave[NUMSAMPS]; //reference sine wave data storage
static volatile int trianglewave[NUMSAMPS]; //reference triangle wave data storage

void initSPI();
unsigned char spi_io(unsigned short o);
unsigned short construct_voltage();

void makesinewave();
void maketrianglewave();

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
    TRISBbits.TRISB4=1;
    
    initSPI();
    
    __builtin_enable_interrupts();
    
    unsigned short i=0;
    makesinewave();
    maketrianglewave();
    while (1) {
        unsigned short p_s,p_t; //start constructing message to send to MCP Channel A to see 1.65V 
        unsigned char c=0; //function selects the mode
        int d;
        for (d=0;i<NUMSAMPS;d++)
            {
        unsigned short v_triangle = trianglewave[d]; 
        c=1;
        p_t = c<<15;
        p_t = p_t | 0b111<<12;
        p_t = p_t | v_triangle;
        //return p;
        unsigned short v_sine = sinewave[d];
        c=0;
        p_s = c<<15;
        p_s = p_s | 0b111<<12;
        p_s = p_s | v_sine;
        
        LATAbits.LATA0 = 0; //bring CS low
        //i = construct_voltage();
        spi_io(p_s>>8); //perform 2 x 8-bit writes. 1st 8 bit number contains a
        spi_io(p_s);
        spi_io(p_t>>8); //perform 2 x 8-bit writes. 1st 8 bit number contains a
        spi_io(p_t);
        LATAbits.LATA0=1; //bring CS high
        
          i++;
            if (i == 1000) {
                i = 0;
            }
        
        /* TESTING SPI i++;
        if (i==100) {
            i=0;
        }*/
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()<4800/2) { 
        }
        /*HW1 STUFF // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the sysclk
        /*in the infinite loop, if the value of B4 is pushed, 
         * turn on A4 for 0.5s, off for 0.5s, on for 0.5s, 
         * and off for 0.5s (so two one-second square wave blinks).
        while (!PORTBbits.RB4) {
        
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
         }*/
        }
        }
    }

// initialize SPI1
void initSPI() {
    // Pin B14 has to be SCK1, Pin 25 on chip
    // Turn off analog pins
    ANSELA =0; //1 for analog, turns off all analog pins on A port 
    // Make A0 an output pin for CS
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 1; //make CS high
    //make A1 SDO1. But A1 is also an AN1 (analogue input pin) anything that's analog takes precedence over reprog pins. Need turn off analog cap of those pins
    RPA1Rbits.RPA1R= 0b0011;
    // Set B5 as SDI1
    SDI1Rbits.SDI1R = 0b0001;

    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1.4; // Determines clock frequency. 1000 for 24kHz, 1 for 12MHz; 1.4 for 10MHz 
    // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1], 
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi 
}

//send a byte via spi and return the response
unsigned char spi_io(unsigned short o){
    //function that takes the channel and the voltage (as an unsigned short, 0-4096) as inputs
    SPI1BUF=o; //where we put data to be sent
    while(!SPI1STATbits.SPIRBF) { //wait to receive the byte
    ;
    }
    return SPI1BUF;
}

/*unsigned short construct_voltage() {
    //Channel A write 0 to bit 15
unsigned char c = 0; //function selects the mode
int d,i;
for (i=0;i<NUMSAMPS;i++)
    {
    unsigned short v = sinewave[i]; 
    unsigned short p; //start constructing message to send to MCP Channel A to see 1.65V 
      p = c<<15;
      p = p | 0b111<<12;
      p = p | v;
      return p;  
    }    
}*/

void makesinewave() //sine wave generation: plot y = 2048+2048sinx
{ 
    int i;
    float phaseIncrement = TWO_PI/NUMSAMPS;
    float currentPhase = 0.0;
        for (i=0;i<NUMSAMPS;i++)
        {
        sinewave[i]= 2048+2048*sin(currentPhase);
        currentPhase += phaseIncrement;
        }
}

void maketrianglewave() //triangle wave generation
{ 
  int i = 0;
    for (i = 0; i < NUMSAMPS; ++i) 
    { 
      if ( i < NUMSAMPS/2) 
      { 
        trianglewave[i] = (4096/(NUMSAMPS/2))*i; //increasing part of triangle wave
      }
      else if (i>NUMSAMPS/2)
      { 
        trianglewave[i] = (-4096/(NUMSAMPS/2))*i+4096; //decreasing part of triangle wave
      }
    }
}
