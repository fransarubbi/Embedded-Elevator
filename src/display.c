#include "display.h"
#include "elevator.h"
#include "events.h"
#include <string.h>
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


StateDisplay stateDisplay;
DisplayDataManager displayDataManager;
Event displayEvent;
bool_t flagSetting, flagGoingUp, flagGoingDown, flagStoped;
delay_t displayDelay;
char text_row0[DISPLAY_COLS + 1];
char text_row1[DISPLAY_COLS + 1];
char number_text[2];


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


enum{
	ARROW_UP = 0,
	ARROW_DOWN = 1,
};



void init_Display(void){
	i2cInit(I2C0, 100000);
	delay(LCD_STARTUP_WAIT_MS);
	lcdInit(16, 2, 5, 8);
	lcdCreateChar(ARROW_UP, arrowUp);
	lcdCreateChar(ARROW_DOWN, arrowDown);
	lcdCursorSet(LCD_CURSOR_OFF);
	lcdClear();

	flagGoingUp = 0;
	flagGoingDown = 0;
	flagStoped = 0;

	// Limpiar buffers
	memset(displayDataManager.row0_buffer, ' ', DISPLAY_COLS);
	memset(displayDataManager.row1_buffer, ' ', DISPLAY_COLS);
	memset(displayDataManager.row0_current, ' ', DISPLAY_COLS);
	memset(displayDataManager.row1_current, ' ', DISPLAY_COLS);

	displayDataManager.row0_buffer[DISPLAY_COLS] = '\0';
	displayDataManager.row1_buffer[DISPLAY_COLS] = '\0';
	displayDataManager.row0_current[DISPLAY_COLS] = '\0';
	displayDataManager.row1_current[DISPLAY_COLS] = '\0';
	stateDisplay = DISPLAY_UPDATE_ROW0;
}


void set_row0(char text[]) {
    if (text == NULL) return;
    // Formatear el texto para que ocupe exactamente 16 caracteres
    snprintf(displayDataManager.row0_buffer, DISPLAY_COLS + 1, "%-16.16s", text);
}


void set_row1(char text[]) {
    if (text == NULL) return;
    // Formatear el texto para que ocupe exactamente 16 caracteres
    snprintf(displayDataManager.row1_buffer, DISPLAY_COLS + 1, "%-16.16s", text);
}


void update_Display(){

	switch (stateDisplay) {
	case DISPLAY_UPDATE_ROW0:
		if(consult_DisplayEventQueue(&displayEventQueue, &displayEvent)){
			supress_DisplayEventQueue(&displayEventQueue);
			if(displayEvent == eSetting){
				flagSetting = 1;
				flagStoped = 0;
				flagGoingUp = 0;
				flagGoingDown = 0;
			}
			if(displayEvent == eStop){
				flagSetting = 0;
				flagStoped = 1;
				flagGoingUp = 0;
				flagGoingDown = 0;
			}
			if(displayEvent == eGoingUp){
				flagSetting = 0;
				flagGoingUp = 1;
				flagGoingDown = 0;
				flagStoped = 0;
			}
			if(displayEvent == eGoingDown){
				flagSetting = 0;
				flagGoingDown = 1;
				flagGoingUp = 0;
				flagStoped = 0;
			}
		}

		if(flagSetting){
			snprintf(text_row0, sizeof(text_row0), "    FUERA DE    ");
			set_row0(text_row0);
			if (strcmp(displayDataManager.row0_buffer, displayDataManager.row0_current) != 0) {
				lcdGoToXY(0, 0);
				lcdSendStringRaw(displayDataManager.row0_buffer);
				strcpy(displayDataManager.row0_current, displayDataManager.row0_buffer);
			}
		}

		if(flagStoped){
			snprintf(text_row0, sizeof(text_row0), "Piso actual: %d", sme.currentFloor);
			set_row0(text_row0);
			if (strcmp(displayDataManager.row0_buffer, displayDataManager.row0_current) != 0) {
				lcdGoToXY(0, 0);
				lcdSendStringRaw(displayDataManager.row0_buffer);
				strcpy(displayDataManager.row0_current, displayDataManager.row0_buffer);
			}
		}

		if(flagGoingUp){
			lcdGoToXY(0, 0);
			lcdData(ARROW_UP);
			snprintf(text_row0, sizeof(text_row0), "%d  Destino: %d", sme.currentFloor, sme.destiny);
			set_row0(text_row0);
			if (strcmp(displayDataManager.row0_buffer, displayDataManager.row0_current) != 0) {
				lcdSendStringRaw(displayDataManager.row0_buffer);
				strcpy(displayDataManager.row0_current, displayDataManager.row0_buffer);
			}
		}

		if(flagGoingDown){
			lcdGoToXY(0, 0);
			lcdData(ARROW_DOWN);
			snprintf(text_row0, sizeof(text_row0), "%d  Destino: %d", sme.currentFloor, sme.destiny);
			set_row0(text_row0);
			if (strcmp(displayDataManager.row0_buffer, displayDataManager.row0_current) != 0) {
				lcdSendStringRaw(displayDataManager.row0_buffer);
				strcpy(displayDataManager.row0_current, displayDataManager.row0_buffer);
			}
		}
		stateDisplay = DISPLAY_UPDATE_ROW1;
		break;


	case DISPLAY_UPDATE_ROW1:
		if(flagSetting){
			lcdGoToXY(0, 1);
			snprintf(text_row1, sizeof(text_row1), "    SERVICIO    ");
			set_row1(text_row1);
			if (strcmp(displayDataManager.row1_buffer, displayDataManager.row1_current) != 0) {
				lcdSendStringRaw(displayDataManager.row1_buffer);
				strcpy(displayDataManager.row1_current, displayDataManager.row1_buffer);
			}
		}
		else{
			lcdGoToXY(0, 1);
			snprintf(text_row1, sizeof(text_row1), "Destino: %c%c", number_text[0], number_text[1]);
			set_row1(text_row1);
			if (strcmp(displayDataManager.row1_buffer, displayDataManager.row1_current) != 0) {
				lcdSendStringRaw(displayDataManager.row1_buffer);
				strcpy(displayDataManager.row1_current, displayDataManager.row1_buffer);
			}
		}
		stateDisplay = DISPLAY_UPDATE_ROW0;
		break;
	}
}

