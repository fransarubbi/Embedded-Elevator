#include "sapi.h"
#include "admin.h"
#include "events.h"
#include "display.h"
#include "elevator.h"


#define delayDisplay 4


/*	Teclado Matricial:
 * 	1	2	3	A
 * 	4	5	6	B
 * 	7	8	9	C
 * 	*	0	#	D
 *
 * 	Valores del teclado:
 * 	0	1	2	3
 * 	4	5	6	7
 * 	8	9	10	11
 * 	12	13	14	15
 *
 * 	Los valores
 * 	3 (A)  confirma el numero
 * 	7 (B)  borra el numero
 * 	12 (*)   -- sin uso --
 * 	14 (F)   -- sin uso --
 * 	No son validos salvo para esos casos mencionados. Por ej: antes de recibir
 * 	como valido la A, debe haber algun numero ingresado previamente
 * 	El 11 (C) representa el signo menos para ingresar el subsuelo
 */


StateManager stateManager;
uint8_t number, dec;
int8_t num, n[2];  /* n[0] es el numero MSB, n[1] es el numero LSB */
extern bool_t keyOk;



void init_Manager(void){
	stateManager = WAIT_MANAGER;
	n[0] = 0;
	n[1] = 0;
}


bool_t validate_number(uint8_t flag, uint8_t num){
	bool_t validate = 0;
	if((num >= 0 && num <= 2) || (num >= 4 && num <= 6) || (num >= 8 && num <= 10)){
		validate = 1;
	}
	if(validate){
		return 1;
	}
	else{
		if(flag){
			if((num == 3) || (num == 7) || (num == 13)){
				return 1;
			}
			else{
				return 0;
			}
		}
		else{
			if((num == 11) || (num == 13)){
				return 1;
			}
			else{
				return 0;
			}
		}
	}
}


void decoder(uint8_t *dec, uint8_t num){
	switch(num){
	case 0: *dec = 1;
		break;
	case 1: *dec = 2;
		break;
	case 2: *dec = 3;
		break;
	case 4: *dec = 4;
		break;
	case 5: *dec = 5;
		break;
	case 6: *dec = 6;
		break;
	case 8: *dec = 7;
		break;
	case 9: *dec = 8;
		break;
	case 10: *dec = 9;
		break;
	case 13: *dec = 0;
		break;
	}
}


void update_FSM_Manager(){
	switch(stateManager){
	case WAIT_MANAGER:
		number_text[0] = ' ';
		number_text[1] = ' ';
		if(keyOk){
			keyOk = 0;
			if(consult_KeyQueue(&keyQueue, &number)){
				supress_KeyQueue(&keyQueue);
				if(validate_number(0, number)){
					decoder(&dec, number);
					switch(number){
					case 11: n[0] = -1;
							 number_text[1] = '-';
							 stateManager = NUMBER_1;
							 break;
					case 13: n[0] = 0;
							 number_text[1] = '0';
							 stateManager = NUMBER_1;
							 break;
					default: n[0] = dec;
							 number_text[1] = '0' + dec;
							 stateManager = NUMBER_1;
							 break;
					}
				}
			}
		}
		break;

	case NUMBER_1:
		if(keyOk){
			keyOk = 0;
			if(consult_KeyQueue(&keyQueue, &number)){
				supress_KeyQueue(&keyQueue);
				if(validate_number(1, number)){
					decoder(&dec, number);
					switch(number){
					case 3: if(n[0] == -1){
								n[1] = -n[0];
							}
							else{
								n[1] = n[0];
								n[0] = 0;
							}
							stateManager = FINISH;
							break;
					case 7: stateManager = WAIT_MANAGER;
							break;
					default: n[1] = dec;
							 number_text[0] = number_text[1];
							 number_text[1] = '0' + dec;
							 stateManager = NUMBER_2;
							 break;
					}
				}
			}
		}
		break;

	case NUMBER_2:
		if(keyOk){
			keyOk = 0;
			if(consult_KeyQueue(&keyQueue, &number)){
				supress_KeyQueue(&keyQueue);
				if(number == 3){
					stateManager = FINISH;
				}
				if(number == 7){
					number_text[1] = number_text[0];
					number_text[0] = ' ';
					stateManager = NUMBER_1;
				}
			}
		}
		break;

	case FINISH:
		if(n[0] == -1){
			num = n[1]*-1;
			if(num >= (-aof.amountSubs) && sme.current != SETTING){
				insert_OrderQueue(&orderQueue, num);
			}
		}
		else{
			num = n[0]*10 + n[1];
			if(num == 99) {
				insert_EventQueue(&eventQueue, eSetting);
			}
			else if((num >= 0 && num <= aof.amountFloors) && sme.current != SETTING){
				insert_OrderQueue(&orderQueue, num);
			}
		}
		stateManager = WAIT_MANAGER;
		break;
	}
}


