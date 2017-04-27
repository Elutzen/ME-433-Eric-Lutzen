/* 
 * File:   main.c
 * Author: elutz
 *
 * Created on April 17, 2017, 9:35 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "ILI9163C.h"
#include "PIC32.h"
#include <xc.h>
#include <sys/attribs.h>

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
                LCD_drawPixel(x + col, y + row, 0xFFFF);
            }
            c = c << 1;
        }
    }
}

void string(char * msg, int x, int y) {
    int i = 0;
    int j = 0;
    while (msg[i]) {
        character(msg[i], x + (j % 19)*6, y + 10 * (j / 19));
        i++;
        j++;
    }
}
void rect(int x1, int x2, int y1, int y2,unsigned short color){
    int i,j;
    for(i = x1;i<x2;i++){
        for(j=y1;j<y2;j++){
            LCD_drawPixel(i,j,color);
        }
    }
}
void progress(int length){
    rect(0,128,50,55,WHITE);
    int neg = 0;
    if(length<0){
        neg = 1;
        length = length* -1;
    }
    length = length/2;
    if(neg){
        rect(64-length,64,50,55,GREEN);
        
    }
    else{
        rect(64,64+length,50,55,GREEN);
    }
}
int main(){
        __builtin_disable_interrupts();
        SPI1_init();
        LCD_init();
        __builtin_enable_interrupts();
        LCD_clearScreen(WHITE);
        char message[200];
    sprintf(message, "HELLO WORLD");
    string(message, 32, 30);
    int count = -100;
    while (1) {
       _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 64000) {}
        _CP0_SET_COUNT(0);
        count ++;
        if(count > 99){
            count = -100;
        }
       sprintf(message,"%4d",count);
       string(message, 50, 42);
       progress(count);
    }
    return 0;
}
