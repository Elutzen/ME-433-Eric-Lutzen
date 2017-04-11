/* 
 * File:   main.c
 * Author: elutz
 *
 * Created on April 10, 2017, 11:31 PM
 */

#include <stdio.h>
#include "PIC_config.h"
#include "SPI_config.h"
#include<math.h>


/*
 * 
 */
static volatile float sineWave[200];  
static volatile float triangleWave[200];  
void waves();
void cycle();

int main() {
    initSPI();
    waves();
    while(1){
        cycle();
    }
    return 0;
}

void waves(){
    int i = 0;
     for(i = 0; i < 200; i++) {
		triangleWave[i] = 255 * i / 200;
		sineWave[i] = 127.5 + 127.5 * sin(2 * 3.1415926 * 10 * (i % 100) / 1000);
	}  
}

void cycle(){
    static int j = 0;
    if(_CP0_GET_COUNT() >24000){
        _CP0_SET_COUNT(0);
        setOutputVoltage(0,sineWave[j]);
        setOutputVoltage(1,triangleWave[j]);
        j++;
        if(j>199){
            j=0;
        }
    }
}

