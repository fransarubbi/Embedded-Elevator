#ifndef ELEVATOR_H_
#define ELEVATOR_H_

#include "sapi.h"

#define MAX_FLOORS 26


typedef enum {    //Todos los estados
	SETTING,
    STOPED,
    GOING_UP,
    GOING_DOWN
} StateElevatorID;


typedef struct {
	bool_t flag;
	int8_t floor;
} floor;


typedef struct {
	floor array[MAX_FLOORS];
	int8_t amountFloors;
	int8_t amountSubs;
	int8_t index;
} arrayOfFloors;


typedef struct StateMachineElevator StateMachineElevator;

typedef void (*StateHandler)(StateMachineElevator*, arrayOfFloors*);


struct StateMachineElevator{
	StateElevatorID current;
	StateElevatorID direction;
    StateHandler handler;
    int8_t currentFloor;
    int8_t destiny;
};





extern StateMachineElevator sme;
extern arrayOfFloors aof;
typedef enum{t0, t1, t2} aTimers_t;

void transition_to(StateMachineElevator*, StateHandler, StateElevatorID);   // Transición de estado
void update_FSM_Elevator(StateMachineElevator*, arrayOfFloors*);

void init_Elevator(StateMachineElevator*);
void init_Floors(arrayOfFloors*);


void state_SETTING(StateMachineElevator*, arrayOfFloors*);
void state_STOPED(StateMachineElevator*, arrayOfFloors*);
void state_GOING_UP(StateMachineElevator*, arrayOfFloors*);
void state_GOING_DOWN(StateMachineElevator*, arrayOfFloors*);
void state_CLOSED_DOOR(StateMachineElevator*, arrayOfFloors*);
void state_OPEN_DOOR(StateMachineElevator*, arrayOfFloors*);
void state_CLOSING_DOOR(StateMachineElevator*, arrayOfFloors*);
void state_OPENING_DOOR(StateMachineElevator*, arrayOfFloors*);


void setting_timers(void);
bool_t validate_order(int8_t);
void select_order(StateMachineElevator*, arrayOfFloors*);
bool_t check_order(arrayOfFloors*);
void match_floor(arrayOfFloors*);
void disable_timer(aTimers_t);
void enable_timer(aTimers_t);
void direction_up(int8_t, StateMachineElevator*);
void direction_down(int8_t, StateMachineElevator*);



#endif /* ELEVATOR_H_ */
