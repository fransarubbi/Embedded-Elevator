#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "sapi.h"


typedef enum{
	WAIT_KEYBOARD,
	DEBOUNCE_KEYBOARD,
	PRESS_KEYBOARD
} StateKey;


extern const uint8_t pinsRows[4];

extern const uint8_t pinsCols[4];


void init_Keyboard(void);
void update_FSM_Keyboard(void);
uint8_t readKeyboard(uint8_t);


#endif /* KEYBOARD_H */
