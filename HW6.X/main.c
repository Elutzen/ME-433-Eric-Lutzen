/* 
 * File:   main.c
 * Author: elutz
 *
 * Created on April 17, 2017, 9:35 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "ILI9163C.h"
#include "spi.h"
#include <xc.h>
#include <sys/attribs.h>

void character(char c, unsigned short col, unsigned short row);
// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

void character(char c, unsigned short x, unsigned short y){
    int temp = (int)c - 32;
    int col = 0;
    int row = 0;
    for(col = 0; col < 5; col++){
        c = 0x01;
        for(row = 0; row< 8; row++){
            if((ASCII[temp][col] & c) >> row){
                LCD_drawPixel(x+col,y+row,0x0000);
            } else {
                LCD_drawPixel(x + col,y +row, 0xFFFF);
            }
            c = c <<1;
        }
    }
}
    
    void string(char * msg, int x, int y){
        int i = 0;
        int j = 0;
        while(msg[i]){
            character(msg[i], x + (j%19)*6,y+10*(j/10));
            i++;
            j++;
        }
    }
    int main(){
        LCD_init();
        spi_init();
        LCD_clearScreen(0xFFFF);
        char message[200];
        sprintf(message,"HELLO WORLD");
        string(message,28,30);
        return 0;
    }
