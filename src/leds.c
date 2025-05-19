#include "leds.h"
#include "sapi.h"
#include "events.h"

#define	cTicksLed	150


static LedState currentStateR;
static LedState currentStateG;
static LedState currentStateB;
static LedState currentState1;
static LedState currentState2;
static LedState currentState3;
static LedFlashingState currentStateFlashingR;
static delay_t vTickLed;
static Event event;
static bool_t flag1, flag2, flag3, flag4;


// Funcion privada
static void aLedR(actionLed_t action){
	switch (action){
	case cInitLed:
		gpioInit(LEDR, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(LEDR, 0);
		break;
	case cOnLed:
		gpioWrite(LEDR, 1);
		break;
	case cOffLed:
		gpioWrite(LEDR, 0);
		break;
	}
}

static void aLedG(actionLed_t action){
	switch (action){
	case cInitLed:
		gpioInit(LEDG, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(LEDG, 0);
		break;
	case cOnLed:
		gpioWrite(LEDG, 1);
		break;
	case cOffLed:
		gpioWrite(LEDG, 0);
		break;
	}
}

static void aLedB(actionLed_t action){
	switch (action){
	case cInitLed:
		gpioInit(LEDB, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(LEDB, 0);
		break;
	case cOnLed:
		gpioWrite(LEDB, 1);
		break;
	case cOffLed:
		gpioWrite(LEDB, 0);
		break;
	}
}

static void aLed1(actionLed_t action){
	switch (action){
	case cInitLed:
		gpioInit(LED1, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(LED1, 0);
		break;
	case cOnLed:
		gpioWrite(LED1, 1);
		break;
	case cOffLed:
		gpioWrite(LED1, 0);
		break;
	}
}

static void aLed2(actionLed_t action){
	switch (action){
	case cInitLed:
		gpioInit(LED2, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(LED2, 0);
		break;
	case cOnLed:
		gpioWrite(LED2, 1);
		break;
	case cOffLed:
		gpioWrite(LED2, 0);
		break;
	}
}

static void aLed3(actionLed_t action){
	switch (action){
	case cInitLed:
		gpioInit(LED3, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(LED3, 0);
		break;
	case cOnLed:
		gpioWrite(LED3, 1);
		break;
	case cOffLed:
		gpioWrite(LED3, 0);
		break;
	}
}


// Funciones Globales
void init_FSM_Flashing(void){
	aLedR(cOffLed);
	delayInit(&vTickLed,cTicksLed);
	currentStateFlashingR = FLASHING_SLEEP;
}


void init_Led(void){
	flag1 = 0;
	flag2 = 0;
	flag3 = 0;
	flag4 = 0;
	aLedR(cInitLed);
	aLedG(cInitLed);
	aLedB(cInitLed);
	aLed1(cInitLed);
	aLed2(cInitLed);
	aLed3(cInitLed);
	currentStateR = SLEEP;
	currentStateG = SLEEP;
	currentStateB = SLEEP;
	currentState1 = SLEEP;
	currentState2 = SLEEP;
	currentState3 = SLEEP;
	init_FSM_Flashing();
}


void update_FSM_Led(void){
	switch (currentStateR){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eAlarm){
				supress_LedEventQueue(&ledEventQueue);
				currentStateR = FLASHING;
				aLedR(cOnLed);
			}
		}
		break;

	case FLASHING:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eEndAlarm){
				supress_LedEventQueue(&ledEventQueue);
				currentStateR = SLEEP;
				currentStateFlashingR = FLASHING_SLEEP;
				aLedR(cOffLed);
			}
		}

		switch (currentStateFlashingR){
		case FLASHING_SLEEP:
			if (delayRead(&vTickLed)){
				currentStateFlashingR = FLASHING_AWAKE;
				aLedR(cOnLed);
				delayRead(&vTickLed);
			}
			break;
		case FLASHING_AWAKE:
			if (delayRead(&vTickLed)){
				currentStateFlashingR = FLASHING_SLEEP;
				aLedR(cOffLed);
				delayRead(&vTickLed);
			}
			break;
		}
		break;
	}

	switch (currentStateG){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eOpenDoor){
				supress_LedEventQueue(&ledEventQueue);
				flag1 = 1;
				currentStateG = AWAKE;
				aLedG(cOnLed);
			}
		}
		break;

	case AWAKE:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eClosedDoor){
				supress_LedEventQueue(&ledEventQueue);
				flag2 = 1;
				currentStateG = SLEEP;
				aLedG(cOffLed);
			}
		}
		break;
	}

	switch (currentStateB){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eGoingUp || event == eGoingDown){
				supress_LedEventQueue(&ledEventQueue);
				flag3 = 1;
				currentStateB = AWAKE;
				aLedB(cOnLed);
			}
		}
		break;

	case AWAKE:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eStop){
				supress_LedEventQueue(&ledEventQueue);
				flag4 = 1;
				currentStateB = SLEEP;
				aLedB(cOffLed);
			}
		}
		break;
	}

	switch (currentState1){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eOpeningDoor){
				supress_LedEventQueue(&ledEventQueue);
				currentState1 = AWAKE;
				aLed1(cOnLed);
			}
		}
		break;

	case AWAKE:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(flag1){
				flag1 = 0;
				currentState1 = SLEEP;
				aLed1(cOffLed);
			}
		}
		break;
	}

	switch (currentState2){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eClosingDoor){
				supress_LedEventQueue(&ledEventQueue);
				currentState2 = AWAKE;
				aLed2(cOnLed);
			}
		}
		break;

	case AWAKE:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(flag2){
				currentState2 = SLEEP;
				aLed2(cOffLed);
			}
		}
		break;
	}

	switch (currentState3){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(flag4 || sme.currentFloor == 0){
				flag4 = 0;
				currentState3 = AWAKE;
				aLed3(cOnLed);
			}
		}
		break;

	case AWAKE:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(flag3){
				flag3 = 0;
				if(sme.currentFloor != 0){
					currentState3 = SLEEP;
					aLed3(cOffLed);
				}
				supress_LedEventQueue(&ledEventQueue);
			}
		}
		break;
	}
}
