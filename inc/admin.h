#ifndef ADMIN_H
#define ADMIN_H

#include "sapi.h"


typedef enum{
	WAIT_MANAGER,
	LESS,
	NUMBER_1,
	NUMBER_2,
	PB,
	FINISH
} StateManager;


extern StateManager stateManager;


void init_Manager(void);
void update_FSM_Manager(void);
bool_t validate_Number(bool_t, int8_t);
void decoder(uint8_t*, uint8_t);



#endif /* ADMIN_H */
