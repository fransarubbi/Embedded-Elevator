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
bool_t flagSetting, flagAlarm, flagClosing, flagOpening;
delay_t displayDelay;
extern bool_t flagDisplay;
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


/* Inicializar el display */

void init_Display(void){
	i2cInit(I2C0, 100000);
	delay(LCD_STARTUP_WAIT_MS);
	lcdInit(16, 2, 5, 8);
	lcdCreateChar(ARROW_UP, arrowUp);
	lcdCreateChar(ARROW_DOWN, arrowDown);
	lcdCursorSet(LCD_CURSOR_OFF);
	lcdClear();

	flagSetting = 0;
	flagAlarm = 0;
	flagClosing = 0;
	flagOpening = 0;

	// Limpiar buffers
	memset(displayDataManager.row0_buffer, ' ', DISPLAY_COLS);
	memset(displayDataManager.row1_buffer, ' ', DISPLAY_COLS);
	memset(displayDataManager.row0_current, ' ', DISPLAY_COLS);
	memset(displayDataManager.row1_current, ' ', DISPLAY_COLS);

	displayDataManager.displayMode = DISPLAY_MODE_SETTING;
	displayDataManager.row0_buffer[DISPLAY_COLS] = '\0';
	displayDataManager.row1_buffer[DISPLAY_COLS] = '\0';
	displayDataManager.row0_current[DISPLAY_COLS] = '\0';
	displayDataManager.row1_current[DISPLAY_COLS] = '\0';
	stateDisplay = DISPLAY_UPDATE_ROW0;
}


/* Colocar en buffer el texto a imprimir */

void set_row0(char text[]) {
    if (text == NULL) return;
    // Formatear el texto para que ocupe exactamente 16 caracteres
    snprintf(displayDataManager.row0_buffer, DISPLAY_COLS + 1, "%-16.16s", text);
}


/* Colocar en buffer el texto a imprimir */

void set_row1(char text[]) {
    if (text == NULL) return;
    // Formatear el texto para que ocupe exactamente 16 caracteres
    snprintf(displayDataManager.row1_buffer, DISPLAY_COLS + 1, "%-16.16s", text);
}


/* Funcion para actualizar la fila
 * Se compara lo que tiene buffer con lo que esta actualmente mostrandose en el display
 * si son distintos, se actualiza la informacion
 * */

void update_row_data(bool_t row){
	if(row){
		if (strcmp(displayDataManager.row1_buffer, displayDataManager.row1_current) != 0) {
			lcdSendStringRaw(displayDataManager.row1_buffer);
			strcpy(displayDataManager.row1_current, displayDataManager.row1_buffer);
		}
	}
	else{
		if (strcmp(displayDataManager.row0_buffer, displayDataManager.row0_current) != 0) {
			lcdSendStringRaw(displayDataManager.row0_buffer);
			strcpy(displayDataManager.row0_current, displayDataManager.row0_buffer);
		}
	}
}


/* Funcion para actualizar el display */

void update_Display(){

	if(consult_DisplayEventQueue(&displayEventQueue, &displayEvent)){  /* Consultar eventos del display */
		supress_DisplayEventQueue(&displayEventQueue);
		if(displayEvent == eSetting){   /* Setear los flags segun corresponda si se produce el evento */
			flagSetting = 1;
			flagAlarm = 0;
			flagClosing = 0;
			flagOpening = 0;
			displayDataManager.displayMode = DISPLAY_MODE_SETTING;
		}
		if(displayEvent == eStop){
			flagSetting = 0;
			flagAlarm = 0;
			flagClosing = 0;
			flagOpening = 0;
			displayDataManager.displayMode = DISPLAY_MODE_STOP;
		}
		if(displayEvent == eGoingUp){
			flagSetting = 0;
			flagAlarm = 0;
			flagClosing = 0;
			flagOpening = 0;
			displayDataManager.displayMode = DISPLAY_MODE_GOING_UP;
		}
		if(displayEvent == eGoingDown){
			flagSetting = 0;
			flagAlarm = 0;
			flagClosing = 0;
			flagOpening = 0;
			displayDataManager.displayMode = DISPLAY_MODE_GOING_DOWN;
		}
		if(displayEvent == eAlarm){
			flagAlarm = 1;
			flagSetting = 0;
			flagClosing = 0;
			flagOpening = 0;
			displayDataManager.displayMode = DISPLAY_MODE_ALARM;
		}
		if(displayEvent == eClosingDoor){
			flagClosing = 1;
			flagSetting = 0;
			flagAlarm = 0;
			flagOpening = 0;
			displayDataManager.displayMode = DISPLAY_MODE_CLOSING;
		}
		if(displayEvent == eOpeningDoor){
			flagOpening = 1;
			flagSetting = 0;
			flagAlarm = 0;
			flagClosing = 0;
			displayDataManager.displayMode = DISPLAY_MODE_OPENING;
		}
	}

	switch (stateDisplay) {
	case DISPLAY_UPDATE_ROW0:
		switch(displayDataManager.displayMode){
		case DISPLAY_MODE_SETTING:
			lcdGoToXY(0, 0);
			snprintf(text_row0, sizeof(text_row0), "    FUERA DE    ");
			set_row0(text_row0);
			update_row_data(0);
			break;

		case DISPLAY_MODE_STOP:
			lcdGoToXY(0, 0);
			snprintf(text_row0, sizeof(text_row0), "Piso actual: %d", sme.currentFloor);
			set_row0(text_row0);
			update_row_data(0);
			break;

		case DISPLAY_MODE_GOING_UP:
			lcdGoToXY(0, 0);
			lcdData(ARROW_UP);
			snprintf(text_row0, sizeof(text_row0), "%d  Destino: %d", sme.currentFloor, sme.destiny);
			set_row0(text_row0);
			update_row_data(0);
			break;

		case DISPLAY_MODE_GOING_DOWN:
			lcdGoToXY(0, 0);
			lcdData(ARROW_DOWN);
			snprintf(text_row0, sizeof(text_row0), "%d  Destino: %d", sme.currentFloor, sme.destiny);
			set_row0(text_row0);
			update_row_data(0);
			break;

		case DISPLAY_MODE_ALARM:
			lcdGoToXY(0, 0);
			snprintf(text_row0, sizeof(text_row0), "Puerta bloqueada");
			set_row0(text_row0);
			update_row_data(0);
			break;

		case DISPLAY_MODE_CLOSING:
			lcdGoToXY(0, 0);
			snprintf(text_row0, sizeof(text_row0), "Cerrando puerta");
			set_row0(text_row0);
			update_row_data(0);
			break;

		case DISPLAY_MODE_OPENING:
			lcdGoToXY(0, 0);
			snprintf(text_row0, sizeof(text_row0), "Abriendo puerta");
			set_row0(text_row0);
			update_row_data(0);
			break;
		}

		stateDisplay = DISPLAY_UPDATE_ROW1;
		break;


	case DISPLAY_UPDATE_ROW1:
		if(flagSetting && flagDisplay){
			lcdGoToXY(0, 1);
			snprintf(text_row1, sizeof(text_row1), "    SERVICIO    ");
			set_row1(text_row1);
			update_row_data(1);
		}
		else if(flagClosing || flagOpening || flagAlarm){
			lcdGoToXY(0, 1);
			snprintf(text_row1, sizeof(text_row1), "                ");
			set_row1(text_row1);
			update_row_data(1);
		}
		else{
			lcdGoToXY(0, 1);
			snprintf(text_row1, sizeof(text_row1), "Destino: %c%c", number_text[0], number_text[1]);
			set_row1(text_row1);
			update_row_data(1);
		}
		stateDisplay = DISPLAY_UPDATE_ROW0;
		break;
	}
}

