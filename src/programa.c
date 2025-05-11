#include "keyboard.h"
#include "admin.h"
#include "events.h"
#include "display.h"
#include "sapi.h"


void main(void){
	boardConfig();
	init_Keyboard();
	init_Manager();
	init_Display();

	while(1){
		update_FSM_Keyboard();
		update_FSM_Manager();
		update_Display();
	}
}
