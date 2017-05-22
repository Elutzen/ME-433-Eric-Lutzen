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

//imu 
void i2c_master_setup(void); // set up I2C 1 as a master, at 100 kHz

void i2c_master_start(void); // send a START signal
void i2c_master_restart(void); // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void); // receive a byte of data
void i2c_master_ack(int val); // send an ACK (0) or NACK (1)
void i2c_master_stop(void);

#define SLAVE_ADDR 0b1101011

void i2c_master_setup(void) {
    I2C2BRG = 390; // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
    // look up PGD for your PIC32
    I2C2CONbits.ON = 1; // turn on the I2C2 module
}

// Start a transmission on the I2C bus

void i2c_master_start(void) {
    I2C2CONbits.SEN = 1; // send the start bit
    while (I2C2CONbits.SEN) {
        ;
    } // wait for the start bit to be sent
}

void i2c_master_restart(void) {
    I2C2CONbits.RSEN = 1; // send a restart 
    while (I2C2CONbits.RSEN) {
        ;
    } // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
    I2C2TRN = byte; // if an address, bit 0 = 0 for write, 1 for read
    while (I2C2STATbits.TRSTAT) {
        ;
    } // wait for the transmission to finish
    if (I2C2STATbits.ACKSTAT) { // if this is high, slave has not acknowledged
        // ("I2C2 Master: failed to receive ACK\r\n");
    }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1; // start receiving data
    while (!I2C2STATbits.RBF) {
        ;
    } // wait to receive the data
    return I2C2RCV; // read and return the data
}

void i2c_master_ack(int val) { // sends ACK = 0 (slave should send another byte)
    // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val; // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1; // send ACKDT
    while (I2C2CONbits.ACKEN) {
        ;
    } // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) { // send a STOP:
    I2C2CONbits.PEN = 1; // comm is complete and master relinquishes bus
    while (I2C2CONbits.PEN) {
        ;
    } // wait for STOP to complete
}

void init_IMU() {
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;

    i2c_master_setup(); // init I2C2, which we use as a master
    __builtin_enable_interrupts();
    char CTRL1_XL = 0b10000010;
    char CTRL2_G = 0b1000100;
    char CTRL3_C = 0b00000100;
    //accelerometer
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1);
    i2c_master_send(0x10);
    i2c_master_send(CTRL1_XL);
    i2c_master_stop();
    //gyro
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1);
    i2c_master_send(0x11);
    i2c_master_send(CTRL2_G);
    i2c_master_stop();
    //multi read
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1);
    i2c_master_send(0x12);
    i2c_master_send(CTRL3_C);
    i2c_master_stop();
}

void setExpander(char pin, char level) {

    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1);
    i2c_master_send(0x9);
    char num = 0 | (level << pin);
    i2c_master_send(num);
    i2c_master_stop();
}

char getExpander() {
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1);
    i2c_master_send(0x9);
    i2c_master_restart();
    i2c_master_send((SLAVE_ADDR << 1) | 1);
    char temp = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return temp;
}

void I2C_read_multiple(unsigned char address, unsigned char reg, unsigned char * data, int length) {
    i2c_master_start();
    int i = 0;
    i2c_master_send(address << 1);
    i2c_master_send(reg + i); // starts at temp low, goes however long
    i2c_master_restart();
    i2c_master_send((address << 1) | 1);
    for (i; i < length; ++i) {
        char temp = i2c_master_recv();
        if (i < length - 1) {
            i2c_master_ack(0);
        }
        data[i] = temp;
    }
    i2c_master_ack(1);
    i2c_master_stop();
}

short combineData(char * data, int index) {
    short value = data[index + 1] << 8;
    value = data[index];
    return value;
}
// end imu 

void character(char c, unsigned short x, unsigned short y) {
    int temp = (int) c - 32;
    int col = 0;
    int row = 0;
    for (col = 0; col < 5; col++) {
        c = 0x01;
        for (row = 0; row < 8; row++) {
            if ((ASCII[temp][col] & c) >> row) {
                LCD_drawPixel(x + col, y + row, 0x0000);
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

void rect(int x1, int x2, int y1, int y2, unsigned short color) {
    int i, j;
    for (i = x1; i < x2; i++) {
        for (j = y1; j < y2; j++) {
            LCD_drawPixel(i, j, color);
        }
    }
}

void progress(int length) {
    rect(0, 128, 50, 55, WHITE);
    int neg = 0;
    if (length < 0) {
        neg = 1;
        length = length* -1;
    }
    length = length / 2;
    if (neg) {
        rect(64 - length, 64, 50, 55, GREEN);

    } else {
        rect(64, 64 + length, 50, 55, GREEN);
    }
}

int main() {
    __builtin_disable_interrupts();
    SPI1_init();
    LCD_init();
    init_IMU();
    __builtin_enable_interrupts();
    LCD_clearScreen(WHITE);
    char message[200];
    sprintf(message, "TEST");
    string(message, 32, 30);
    int count = -100;
    int length = 14;
    char msg[100];
     int aX= 0;
     int aY = 0;
     int aZ = 0;
    unsigned char data[length];
    while (1) {
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 64000) {
        }
        _CP0_SET_COUNT(0);
        count++;
        if (count > 99) {
            count = -100;
        }
        sprintf(message, "%4d", count);
        string(message, 50, 42);
        progress(count);

       I2C_read_multiple(SLAVE_ADDR, 0x20, data, length);
        short temp = combineData(data, 0);
        short gyroX = combineData(data, 2);
        short gyroY = combineData(data, 4);
        short gyroZ = combineData(data, 6);
        short accelX = combineData(data, 8);
        short accelY = combineData(data, 10);
        short accelZ = combineData(data, 12);
        aX += accelX;
        aY += accelY;
        aZ += accelZ;
        if(count%2 ==0){
            aX /= 2;
            aY /= 2;
            aZ /= 2;
        sprintf(msg, "AccelX: %d", aX);
        string(msg, 28, 56);
        sprintf(msg, "AccelY: %d", aY);
        string(msg, 28, 70);
        sprintf(msg, "AccelZ: %d", aZ);
        string(msg, 28, 80); 
        }
    }
    return 0;
}
