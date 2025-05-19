#include "button.h"
#include "events.h"
#include "elevator.h"
#include "sapi.h"



enum Button {WAIT_BUTTON, DEBOUNCE_BUTTON, PRESS_BUTTON};
enum Button state_someone;
enum Button state_noOne;
static delay_t vDelayDeb;


/*
 * GPIO1 -> hay alguien
 * GPIO3 -> no hay nadie
 */


//	Funciones Globales
void init_Button(void){
	gpioInit(GPIO1, GPIO_INPUT); //  Hay alguien  @suppress("Symbol is not resolved")
	gpioInit(GPIO3, GPIO_INPUT); //  No hay nadie  @suppress("Symbol is not resolved")
	delayInit(&vDelayDeb , cDelayDebounce);
	state_someone = WAIT_BUTTON;
	state_noOne = WAIT_BUTTON;
}


void update_FSM_Button(void){
	switch (state_someone){
	case WAIT_BUTTON:
		if (gpioRead(GPIO1)){
			state_someone = DEBOUNCE_BUTTON;
			delayRead(&vDelayDeb);
		}
		break;
	case DEBOUNCE_BUTTON:
		if (delayRead(&vDelayDeb)){
			if (gpioRead(GPIO1)){
				state_someone = PRESS_BUTTON;
				if(sme.current == STOPED){
					insert_EventQueue(&eventQueue, eThereIsSomeone);
				}
			}else{
				state_someone = WAIT_BUTTON;
			}
		}
		break;
	case PRESS_BUTTON:
		if (!gpioRead(GPIO1)){
			state_someone = WAIT_BUTTON;
		}
	}

	switch (state_noOne){
	case WAIT_BUTTON:
		if (gpioRead(GPIO3)){
			state_noOne = DEBOUNCE_BUTTON;
			delayRead(&vDelayDeb);
		}
		break;
	case DEBOUNCE_BUTTON:
		if (delayRead(&vDelayDeb)){
			if (gpioRead(GPIO3)){
				state_noOne = PRESS_BUTTON;
				if(sme.current == STOPED){
					insert_EventQueue(&eventQueue, eThereIsNoOne);
				}
			}else{
				state_noOne = WAIT_BUTTON;
			}
		}
		break;
	case PRESS_BUTTON:
		if (!gpioRead(GPIO3)){
			state_noOne = WAIT_BUTTON;
		}
	}
}
