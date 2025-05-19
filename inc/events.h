#ifndef EVENTS_H_
#define EVENTS_H_

#include "sapi.h"
#include "elevator.h"

#define MAX_QUEUE 8
#define MAX_ORDERS 16


typedef enum{
	eThereIsSomeone,
	eThereIsNoOne,
	eSetting,
	eGoingUp,
	eGoingDown,
	eStop,
	eOpenDoor,
	eClosedDoor,
	eOpeningDoor,
	eClosingDoor,
	eAlarm,
	eEndAlarm
} Event;


typedef struct{
	Event buffer[MAX_QUEUE];
	uint8_t top;
	uint8_t last;
	uint8_t cant;
} EventQueue;


typedef struct{
	uint8_t buffer[MAX_QUEUE];
	uint8_t top;
	uint8_t last;
	uint8_t cant;
} KeyQueue;


typedef struct{
	int8_t buffer[MAX_QUEUE];
	uint8_t top;
	uint8_t last;
	uint8_t cant;
} OrderQueue;


typedef struct{
	Event buffer[MAX_QUEUE];
	uint8_t top;
	uint8_t last;
	uint8_t cant;
} LedEventQueue;



extern KeyQueue keyQueue;
extern EventQueue eventQueue;
extern LedEventQueue ledEventQueue;
extern OrderQueue orderQueue;


void init_KeyQueue(KeyQueue*);
bool_t consult_KeyQueue(KeyQueue*, uint8_t*);
bool_t insert_KeyQueue(KeyQueue*, uint8_t);
void supress_KeyQueue(KeyQueue*);

void init_EventQueue(EventQueue*);
bool_t consult_EventQueue(EventQueue*, Event*);
bool_t insert_EventQueue(EventQueue*, Event);
void supress_EventQueue(EventQueue*);

void init_LedEventQueue(LedEventQueue*);
bool_t consult_LedEventQueue(LedEventQueue*, Event*);
bool_t insert_LedEventQueue(LedEventQueue*, Event);
void supress_LedEventQueue(LedEventQueue*);

void init_OrderQueue(OrderQueue*);
bool_t consult_OrderQueue(OrderQueue*, int8_t*);
bool_t insert_OrderQueue(OrderQueue*, int8_t);
void supress_OrderQueue(OrderQueue*);


void translator(OrderQueue*, arrayOfFloors*);
void verify_order(arrayOfFloors*, StateMachineElevator*);


#endif /* EVENTS_H_ */
