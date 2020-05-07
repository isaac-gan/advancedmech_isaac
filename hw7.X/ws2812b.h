#ifndef WS2812B_H__
#define WS2812B_H__

#include<xc.h> // processor SFR definitions

// link three 8bit colors together
typedef struct {
    unsigned char r,g,b;
} wsColor; //can refer to wsColor directly w wsColor.r etc so that can pass these 3 colors as 1 unit in and out of functions

void ws2812b_setup();//initialise timer to perform timing to generate pulses
void ws2812b_setColor(wsColor*,int);//sets color of pulses. Build array of all the bits that need to be sent then send them
wsColor HSBtoRGB(float hue, float sat, float brightness);//to generate rainbow array 

#endif
