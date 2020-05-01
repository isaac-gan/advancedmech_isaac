// WS2812B library

#include "ws2812b.h"
// other includes if necessary for debugging

// Timer2 delay times, you can tune these if necessary
#define LOWTIME 15 // number of 48MHz cycles to be low for 0.35uS
#define HIGHTIME 65 // number of 48MHz cycles to be high for 1.65uS. If notice that LEDs running a little slower, could be due to extra while or if statement then cut by one

// setup Timer2 for 48MHz, and setup the output pin
void ws2812b_setup() {
    T2CONbits.TCKPS = 0; // Timer2 prescaler N=1 (1:1)
    PR2 = 65535; // maximum period
    TMR2 = 0; // initialize Timer2 to 0
    T2CONbits.ON = 1; // turn on Timer2

    // initialize digital output pin as off, B6
    TRISBbits.TRISB6=0;// TRISB6 config as digital o/p
    LATBbits.LATB6=0;// LATB6 set to digital low
}

// build an array of high/low times from the color input array, then output the high/low bits
void ws2812b_setColor(wsColor * c, int numLEDs) { //loop through every single LED we have 
    int i = 0; int j = 0; // for loops
    int numBits = 2 * 3 * 8 * numLEDs; // the number of high/low bits to store, 2 per color bit
    volatile unsigned int delay_times[2*3*8*4]; // I only gave you 5 WS2812B, adjust this if you get more somewhere. Save the amount of time between high and low bits into delay time

    // start at time at 0
    delay_times[0] = 0;
    
    int nB = 1; // which high/low bit you are storing, 2 per color bit, 24 color bits per WS2812B
	
    // loop through each WS2812B
    for (i = 0; i < numLEDs; i++) {
        // loop through each color bit, MSB first
        for (j = 7; j >= 0; j--) { //1st 8 bits => Color Red
            // if the bit is a 1. Shift bit over to get MSB
            if ((c[i].r>>j)&0x1) { /* From 8 bit number, identify the bit in c[0].r, is it 1 */ //((c[j].r>>j)&&0x1)
                // the high is longer
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;//how long pin will be high
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;//how long pin will be low
                nB++;//this ensures a digital H is output onto MSB of Red 8 bit message we are sending
            } 
            // if the bit is a 0
            else {
                // the low is longer
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
            }
        }
        
        // once complete if else statement copy and do it again for green
        for (j = 7; j >= 0; j--) { //1st 8 bits => Color Green
            // if the bit is a 1. Shift bit over to get MSB
            if ((c[i].g>>j)==1) { /* From 8 bit number, identify the bit in c[0].r, is it 1 */
                // the high is longer
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;//how long pin will be high
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;//how long pin will be low
                nB++;//this ensures a digital H is output onto MSB of Red 8 bit message we are sending
            } 
            // if the bit is a 0
            else {
                // the low is longer
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
            }
        }
        
		// do it again for blue
        for (j = 7; j >= 0; j--) { //1st 8 bits => Color Blue
            // if the bit is a 1. Shift bit over to get MSB
            if ((c[i].b>>j)==1) { /* From 8 bit number, identify the bit in c[0].r, is it 1 */
                // the high is longer
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;//how long pin will be high
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;//how long pin will be low
                nB++;//this ensures a digital H is output onto MSB of Red 8 bit message we are sending
            } 
            // if the bit is a 0
            else {
                // the low is longer
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
            }
        }
    }

    // turn on the pin for the first high/low
    LATBbits.LATB6 = 1;
    TMR2 = 0; // start the timer. Based on datasheet, always start off with High. Just a matter of high shorter or longer.
    for (i = 1; i < numBits; i++) {//wait until 1st delay time then invert pin
        while (TMR2 < delay_times[i]) {//generate the square wave to send digital high or low
        }
        LATBINV = 0b1000000; // invert B6
    }
    LATBbits.LATB6 = 0;
    TMR2 = 0;
    while(TMR2 < 2400){} // wait 50uS, reset condition, so that next time we write can start with 1st LED in strip
}

// adapted from https://forum.arduino.cc/index.php?topic=8498.0
// hue is a number from 0 to 360 that describes a color on the color wheel
// sat is the saturation level, from 0 to 1, where 1 is full color and 0 is gray
// brightness sets the maximum brightness, from 0 to 1
wsColor HSBtoRGB(float hue, float sat, float brightness) {//hue represents what angle we are on in circle
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

    if (sat == 0.0) {
        red = brightness;
        green = brightness;
        blue = brightness;
    } else {
        if (hue == 360.0) {
            hue = 0;
        }

        int slice = hue / 60.0;
        float hue_frac = (hue / 60.0) - slice;

        float aa = brightness * (1.0 - sat);
        float bb = brightness * (1.0 - sat * hue_frac);
        float cc = brightness * (1.0 - sat * (1.0 - hue_frac));

        switch (slice) {
            case 0:
                red = brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = brightness;
                break;
            case 5:
                red = brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    unsigned char ired = red * 255.0;
    unsigned char igreen = green * 255.0;
    unsigned char iblue = blue * 255.0;

    wsColor c;
    c.r = ired;
    c.g = igreen;
    c.b = iblue;
    return c;
}

//take adv of above function to generate colors,in a loop set each color 30 or 60 deg out of phase
//and update colour to get a rainbow pattern. doing same thing just 30/60 deg out of phase in color wheel
//add factor of 10 to high/low time to debug on nscope, but doing so means LED won't turn on
//is proportion of high to low correct, are bits coming out MSB to LSB
//display int value and bit level on LCD screen to check how many elements were in array
//heartbeat w LED or on LCD,so that yes I was telling LCDs to change colour and if they are not smth else is wrong