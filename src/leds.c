#include "leds.h"
#include "sapi.h"
#include "events.h"

#define	cTicksLed	150


static LedFlashingState currentStateFlashing;
static delay_t vTickLed;
Event event;
static bool_t flag;


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



void init_Led(void){
	flag = 0;
	aLedGPIO0(cInitLed);
	aLedGPIO2(cInitLed);
	aLedGPIO4(cInitLed);
	aLedLED1(cInitLed);
	aLedLED2(cInitLed);
	aLedLED3(cInitLed);
	aLedGPIO0(cOffLed);
	delayInit(&vTickLed,cTicksLed);
	currentStateFlashing = FLASHING_SLEEP;
}


void update_FSM_Led(void){
	if(consult_LedEventQueue(&ledEventQueue, &event)){
		supress_LedEventQueue(&ledEventQueue);
		if(event == eOpenDoor){
			aLedLED3(cOnLed);
			aLedGPIO2(cOffLed);
		}
		if(event == eGoingUp || event == eGoingDown){
			aLedLED1(cOnLed);
			aLedLED2(cOffLed);
		}
		if(event == eStop){
			aLedLED1(cOffLed);
			aLedLED2(cOnLed);
		}
		if(event == eOpeningDoor){
			aLedGPIO2(cOnLed);
		}
		if(event == eClosingDoor){
			aLedGPIO4(cOnLed);
			aLedLED3(cOffLed);
		}
		if(event == eClosedDoor){
			aLedGPIO4(cOffLed);
		}
		if(event == eAlarm){
			flag = 1;
		}
		if(event == eEndAlarm){
			flag = 0;
		}
	}

	if(flag){
		switch(currentStateFlashing){
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
	}
	else{
		aLedGPIO0(cOffLed);
	}
}
