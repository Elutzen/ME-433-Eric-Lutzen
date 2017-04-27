/* 
 * File:   main.c
 * Author: elutz
 *
 * Created on April 17, 2017, 9:35 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "ILI9163C.h"
include "PIC32.h"
#include <xc.h>
#include <sys/attribs.h>

void character(char c, unsigned short col, unsigned short row);

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
        SPI1_init();
        LCD_clearScreen(BLACK);
        char message[200];
        sprintf(message,"HELLO WORLD");
        string(message,28,30);
        return 0;
    }
