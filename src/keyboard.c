#include "keyboard.h"
#include "events.h"
#include "sapi.h"

#define	delayDebounce 80
#define delayScan 10
#define ROWS 4
#define COLS 4


StateKey stateKey;
static uint8_t scanRow;
static uint8_t scanCol;
static uint8_t keyRow;
static uint8_t keyCol;
static uint8_t keyCode;      // Código de la tecla presionada
bool_t keyOk;             // Flag para indicar si la tecla ya fue leída
static delay_t vDelayDeb;     // Retardo de anti-rebote
static delay_t vDelayScan;    // Retardo de escaneo


const uint8_t pinsRows[4] = {RS232_TXD, CAN_RD, CAN_TD, T_COL1};
const uint8_t pinsCols[4] = {T_FIL0, T_FIL3, T_FIL2, T_COL0};


void init_Keyboard(void){

	uint8_t i = 0;
	for(i = 0; i < 4; i++){
		// Configurar las filas como salidas
		gpioInit(pinsRows[i], GPIO_OUTPUT);   // @suppress("Symbol is not resolved")
	    gpioWrite(pinsRows[i], OFF);
	    // Configurar las columnas como entradas, con resistencias de pull-up internas
	    gpioInit(pinsCols[i], GPIO_INPUT_PULLDOWN);       // @suppress("Symbol is not resolved")
	}

	scanRow = 0;
	scanCol = 0;
	keyRow = 0;
	keyCol = 0;
	keyCode = 0;      // Código de la tecla presionada
	keyOk = 0;
	stateKey = WAIT_KEYBOARD;
	delayInit(&vDelayScan, delayScan);
}


void update_FSM_Keyboard(){
	switch (stateKey){
	case WAIT_KEYBOARD:
		if(delayRead(&vDelayScan)){
			scanRow = (scanRow + 1) & 3;
		}

		scanCol = readKeyboard(scanRow);    // Leer estado actual del teclado

		if(scanCol != 4){
			keyRow = scanRow;
			keyCol = scanCol;
			delayInit(&vDelayDeb, delayDebounce);
			stateKey = DEBOUNCE_KEYBOARD;
		}
		break;

	case DEBOUNCE_KEYBOARD:
		if (delayRead(&vDelayDeb)){
			scanCol = readKeyboard(scanRow);

			if(scanCol == keyCol){
				stateKey = PRESS_KEYBOARD;
			}
			else{
				stateKey = WAIT_KEYBOARD;
			}
		}
		break;

	case PRESS_KEYBOARD:
		scanCol = readKeyboard(scanRow);
		if(scanCol == 4){
			keyOk = 1;
			keyCode = scanRow * COLS + keyCol;
			insert_KeyQueue(&keyQueue, keyCode);
			stateKey = WAIT_KEYBOARD;
		}
	}
}


uint8_t readKeyboard(uint8_t row){
	uint8_t i;

	for(i = 0; i < 4; i++){
		gpioWrite(pinsRows[i], OFF);
	}

	gpioWrite(pinsRows[row], ON);

	if (gpioRead(pinsCols[0]) == ON) return 0;      // Determinar índice de columna (0-3)
	else if (gpioRead(pinsCols[1]) == ON) return 1;
	else if (gpioRead(pinsCols[2]) == ON) return 2;
	else if (gpioRead(pinsCols[3]) == ON) return 3;
	else return 4;
}

