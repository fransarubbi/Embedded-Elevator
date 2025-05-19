#include "keyboard.h"
#include "admin.h"
#include "events.h"
#include "elevator.h"
#include "button.h"
#include "leds.h"
#include "sapi.h"



void main(void){
	boardConfig();
	uartConfig(UART_USB, 115200);
	init_KeyQueue(&keyQueue);
	init_EventQueue(&eventQueue);
	init_OrderQueue(&orderQueue);
	init_LedEventQueue(&ledEventQueue);
	init_Keyboard();
	init_Button();
	init_Manager();
	init_Floors(&aof);
	init_Elevator(&sme);
	init_Led();

	while(1){
		update_FSM_Keyboard();
		update_FSM_Manager();
		update_FSM_Button();
		translator(&orderQueue, &aof);
		update_FSM_Elevator(&sme, &aof);
		verify_order(&aof, &sme);
		update_FSM_Led();
	}
}
