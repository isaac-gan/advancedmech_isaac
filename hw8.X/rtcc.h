#ifndef RTCC_H__
#define RTCC_H__

#include <xc.h>

typedef struct { //copy of RTCDATE and RTCTIME to store characters for date and time
    unsigned char sec01;
    unsigned char sec10;
    unsigned char min01;
    unsigned char min10;
    unsigned char hr01;
    unsigned char hr10;
    
    unsigned char yr01;
    unsigned char yr10;
    unsigned char mn01;
    unsigned char mn10;
    unsigned char dy01;
    unsigned char dy10;
    unsigned char wk; //Day of Week Sunday 0 Monday 1 Tuesday 2 Wednesday 3 Thursday 4 Friday 5 Saturday 6
} rtccTime;

void rtcc_setup(unsigned long time, unsigned long date); //takes hex number of time and hex date to set PIC to
rtccTime readRTCC();
void dayOfTheWeek(unsigned char, char*); // get the word for the day of the week
//dayOfTheWeek - based on uchar wk 0-6, sends back the letters in a char array
#endif