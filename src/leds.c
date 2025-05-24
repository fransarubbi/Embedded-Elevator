#include "leds.h"
#include "sapi.h"
#include "events.h"

#define	cTicksLed	150


static LedState currentStateGPIO0;
static LedState currentStateGPIO2;
static LedState currentStateGPIO4;
static LedState currentStateLED1;
static LedState currentStateLED2;
static LedState currentStateLED3;
static LedFlashingState currentStateFlashing;
static delay_t vTickLed;
Event event;
static bool_t flag1, flag2, flag3, flag4;


/*
	LED1: ascensor en movimiento.
	LED2: ascensor detenido o en planta baja.
	LED3: puerta abierta.
	LED VERDE (GPIO0):  problema con puerta.
	LED AZUL (GPIO2): puerta abriéndose.
	LED AMARILLO (GPIO4): puerta cerrándose.
 */


// Funcion privada
static void aLedGPIO0(actionLed_t action){  // 0 2 4
	switch (action){
	case cInitLed:
		gpioInit(GPIO0, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(GPIO0, 0);
		break;
	case cOnLed:
		gpioWrite(GPIO0, 1);
		break;
	case cOffLed:
		gpioWrite(GPIO0, 0);
		break;
	}
}

static void aLedLED3(actionLed_t action){
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

static void aLedLED1(actionLed_t action){
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

static void aLedGPIO2(actionLed_t action){
	switch (action){
	case cInitLed:
		gpioInit(GPIO2, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(GPIO2, 0);
		break;
	case cOnLed:
		gpioWrite(GPIO2, 1);
		break;
	case cOffLed:
		gpioWrite(GPIO2, 0);
		break;
	}
}

static void aLedGPIO4(actionLed_t action){
	switch (action){
	case cInitLed:
		gpioInit(GPIO4, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(GPIO4, 0);
		break;
	case cOnLed:
		gpioWrite(GPIO4, 1);
		break;
	case cOffLed:
		gpioWrite(GPIO4, 0);
		break;
	}
}

static void aLedLED2(actionLed_t action){
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


// Funciones Globales
void init_FSM_Flashing(void){
	aLedGPIO0(cOffLed);
	delayInit(&vTickLed,cTicksLed);
	currentStateFlashing = FLASHING_SLEEP;
}


void init_Led(void){
	flag1 = 0;
	flag2 = 0;
	flag3 = 0;
	flag4 = 0;
	aLedGPIO0(cInitLed);
	aLedGPIO2(cInitLed);
	aLedGPIO4(cInitLed);
	aLedLED1(cInitLed);
	aLedLED2(cInitLed);
	aLedLED3(cInitLed);
	currentStateGPIO0 = SLEEP;
	currentStateGPIO2 = SLEEP;
	currentStateGPIO4 = SLEEP;
	currentStateLED1 = SLEEP;
	currentStateLED2 = SLEEP;
	currentStateLED3 = SLEEP;
	init_FSM_Flashing();
}


void update_FSM_Led(void){
	switch (currentStateGPIO0){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eAlarm){
				supress_LedEventQueue(&ledEventQueue);
				currentStateGPIO0 = FLASHING;
				aLedGPIO0(cOnLed);
			}
		}
		break;

	case FLASHING:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eEndAlarm){
				supress_LedEventQueue(&ledEventQueue);
				currentStateGPIO0 = SLEEP;
				currentStateFlashing = FLASHING_SLEEP;
				aLedGPIO0(cOffLed);
			}
		}

		switch (currentStateFlashing){
		case FLASHING_SLEEP:
			if (delayRead(&vTickLed)){
				currentStateFlashing = FLASHING_AWAKE;
				aLedGPIO0(cOnLed);
				delayRead(&vTickLed);
			}
			break;
		case FLASHING_AWAKE:
			if (delayRead(&vTickLed)){
				currentStateFlashing = FLASHING_SLEEP;
				aLedGPIO0(cOffLed);
				delayRead(&vTickLed);
			}
			break;
		}
		break;
	}

	switch (currentStateLED3){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eOpenDoor){
				supress_LedEventQueue(&ledEventQueue);
				flag1 = 1;
				currentStateLED3 = AWAKE;
				aLedLED3(cOnLed);
			}
		}
		break;

	case AWAKE:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eClosedDoor){
				supress_LedEventQueue(&ledEventQueue);
				flag2 = 1;
				currentStateLED3 = SLEEP;
				aLedLED3(cOffLed);
			}
		}
		break;
	}

	switch (currentStateLED1){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eGoingUp || event == eGoingDown){
				supress_LedEventQueue(&ledEventQueue);
				flag3 = 1;
				currentStateLED1 = AWAKE;
				aLedLED1(cOnLed);
			}
		}
		break;

	case AWAKE:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eStop){
				supress_LedEventQueue(&ledEventQueue);
				flag4 = 1;
				currentStateLED1 = SLEEP;
				aLedLED1(cOffLed);
			}
		}
		break;
	}

	switch (currentStateGPIO2){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eOpeningDoor){
				supress_LedEventQueue(&ledEventQueue);
				currentStateGPIO2 = AWAKE;
				aLedGPIO2(cOnLed);
			}
		}
		break;

	case AWAKE:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(flag1){
				flag1 = 0;
				currentStateGPIO2 = SLEEP;
				aLedGPIO2(cOffLed);
			}
		}
		break;
	}

	switch (currentStateGPIO4){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(event == eClosingDoor){
				supress_LedEventQueue(&ledEventQueue);
				currentStateGPIO4 = AWAKE;
				aLedGPIO4(cOnLed);
			}
		}
		break;

	case AWAKE:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(flag2){
				flag2 = 0;
				currentStateGPIO4 = SLEEP;
				aLedGPIO4(cOffLed);
			}
		}
		break;
	}

	switch (currentStateLED2){
	case SLEEP:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(flag4 || sme.currentFloor == 0){
				flag4 = 0;
				currentStateLED2 = AWAKE;
				aLedLED2(cOnLed);
			}
		}
		break;

	case AWAKE:
		if(consult_LedEventQueue(&ledEventQueue, &event)){
			if(flag3){
				flag3 = 0;
				if(sme.currentFloor != 0){
					currentStateLED2 = SLEEP;
					aLedLED2(cOffLed);
				}
				supress_LedEventQueue(&ledEventQueue);
			}
		}
		break;
	}
}
