#ifndef LEDS_H_
#define LEDS_H_


/* Estado del led titilante */
typedef enum{
	FLASHING_SLEEP,
	FLASHING_AWAKE
} LedFlashingState;


typedef enum{cInitLed = 0, cOnLed, cOffLed} actionLed_t;


void init_FSM_Flashing(void);
void init_Led(void);
void update_FSM_Led(void);


#endif /* LEDS_H_ */
