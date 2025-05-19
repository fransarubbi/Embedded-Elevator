#include "display.h"
#include "sapi.h"


/*		c4	c3	c2	c1	c0
 * r0 	x	x	x	x	x
 * r1 	x	x	x	x	x
 * r2 	x	x	x	x	x
 * r3 	x	x	x	x	x
 * r4 	x	x	x	x	x
 * r5 	x	x	x	x	x
 * r6 	x	x	x	x	x
 * r7 	x	x	x	x	x
 *
 * El CGRAM consta de una matriz de 8x5
 */


const char arrowUp[8] = {
	0b00100,
	0b01110,
	0b11111,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100
};


const char arrowDown[8] = {
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b11111,
	0b01110,
	0b00100
};


const char lock[8] = {
	0b01110,
	0b10001,
	0b10001,
	0b10001,
	0b11111,
	0b11011,
	0b11011,
	0b11111
};


const char unlock[8] = {
	0b01110,
	0b10000,
	0b10000,
	0b10000,
	0b11111,
	0b11011,
	0b11011,
	0b11111
};


const char less[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b01111,
	0b11110,
	0b00000,
	0b00000,
	0b00000
};


enum{
	ARROW_UP = 0,
	ARROW_DOWN = 1,
	LOCK = 2,
	UNLOCK = 3
};


char bufferUp[16] = "PROBANDO";
char bufferDown[16] = "enter floor:";


/*char h[] = "hola";
char c[] = "chau";*/

void init_Display(){
	i2cInit(I2C0, 100000);
	delay(LCD_STARTUP_WAIT_MS);
	lcdInit(16, 2, 5, 8);  // Inicializar LCD de 16x2 con cada caracter de 5x8 pixels
	// Cargar el caracter a CGRAM
	lcdCreateChar(ARROW_UP, arrowUp);
	lcdCreateChar(ARROW_DOWN, arrowDown);
	lcdCreateChar(LOCK, lock);
	lcdCreateChar(UNLOCK, unlock);
	lcdCursorSet(LCD_CURSOR_OFF);
	lcdClear();
}


/*void update_Display(){
	lcdGoToXY(0, 0);
	lcdSendStringRaw(h);  //bufferUp
	lcdGoToXY(0, 1);
	lcdSendStringRaw(c); //bufferDown
	delay(3000);
	lcdClear();
}*/


void display_firstkey(char bufferDown[], char num){
	bufferDown[14] = num;
}


void display_secondkey(char bufferDown[], char num){
	bufferDown[13] = bufferDown[14];
	bufferDown[14] = num;
}


void display_pb(char bufferDown[]){
	bufferDown[13] = 'P';
	bufferDown[14] = 'b';
}


void display_less(char bufferDown[]){
	bufferDown[14] = '-';
}


void clean_bufferDown(char bufferDown[]){
	bufferDown[13] = ' ';
	bufferDown[14] = ' ';
}


void display_delete_number(char bufferDown[]){
	bufferDown[14] = bufferDown[13];
	bufferDown[13] = ' ';
}


