#include "sapi.h"
#include "events.h"
#include "elevator.h"


KeyQueue keyQueue;
EventQueue eventQueue;
LedEventQueue ledEventQueue;
OrderQueue orderQueue;



/*
 * Inicializar la cola
 */
void init_KeyQueue(KeyQueue* keyQueue){
	keyQueue->top = 0;
	keyQueue->last = 0;
	keyQueue->cant = 0;
}

void init_EventQueue(EventQueue* eventQueue){
	eventQueue->top = 0;
	eventQueue->last = 0;
	eventQueue->cant = 0;
}

void init_LedEventQueue(LedEventQueue* ledEventQueue){
	ledEventQueue->top = 0;
	ledEventQueue->last = 0;
	ledEventQueue->cant = 0;
}

void init_OrderQueue(OrderQueue* orderQueue){
	orderQueue->top = 0;
	orderQueue->last = 0;
	orderQueue->cant = 0;
	uint8_t i;
	for(i = 0; i < MAX_QUEUE; i++){
		orderQueue->buffer[i] = -1;
	}
}


/*
 * Consultar el evento que esta en el tope, si la cola no esta vacia
 */
bool_t consult_KeyQueue(KeyQueue* keyQueue, uint8_t* e){
	if(keyQueue->cant == 0){
		return 0;
	}
	*e = keyQueue->buffer[keyQueue->top];
	return 1;
}

bool_t consult_EventQueue(EventQueue* eventQueue, Event* e){
	if(eventQueue->cant == 0){
		return 0;
	}
	*e = eventQueue->buffer[eventQueue->top];
	return 1;
}

bool_t consult_LedEventQueue(LedEventQueue* ledEventQueue, Event* e){
	if(ledEventQueue->cant == 0){
		return 0;
	}
	*e = ledEventQueue->buffer[ledEventQueue->top];
	return 1;
}

bool_t consult_OrderQueue(OrderQueue* orderQueue, int8_t* e){
	if(orderQueue->cant == 0){
		return 0;
	}
	*e = orderQueue->buffer[orderQueue->top];
	return 1;
}


/*
 * Insertar evento en la cola si la misma no esta llena
 */
bool_t insert_KeyQueue(KeyQueue* keyQueue, uint8_t e){
	if(keyQueue->cant == MAX_QUEUE){
		return 0;   // Cola llena
	}
	keyQueue->buffer[keyQueue->last] = e;
	keyQueue->last = (keyQueue->last + 1) & (MAX_QUEUE - 1);
	keyQueue->cant++;
	return 1;
}

bool_t insert_EventQueue(EventQueue* eventQueue, Event e){
	if(eventQueue->cant == MAX_QUEUE){
		return 0;   // Cola llena
	}
	eventQueue->buffer[eventQueue->last] = e;
	eventQueue->last = (eventQueue->last + 1) & (MAX_QUEUE - 1);
	eventQueue->cant++;
	return 1;
}

bool_t insert_LedEventQueue(LedEventQueue* ledEventQueue, Event e){
	if(ledEventQueue->cant == MAX_QUEUE){
		return 0;   // Cola llena
	}
	ledEventQueue->buffer[ledEventQueue->last] = e;
	ledEventQueue->last = (ledEventQueue->last + 1) & (MAX_QUEUE - 1);
	ledEventQueue->cant++;
	return 1;
}

bool_t insert_OrderQueue(OrderQueue* orderQueue, int8_t e){
	if(orderQueue->cant == MAX_QUEUE){
		return 0;
	}
	/* El pedido no existe */
	orderQueue->buffer[orderQueue->last] = e;
	orderQueue->last = (orderQueue->last + 1) & (MAX_QUEUE - 1);
	orderQueue->cant++;
	return 1;
}


/*
 * Eliminar evento del tope de la pila
 */
void supress_KeyQueue(KeyQueue* keyQueue){
	keyQueue->top = (keyQueue->top + 1) & (MAX_QUEUE - 1);
	keyQueue->cant--;
}

void supress_EventQueue(EventQueue* eventQueue){
	eventQueue->top = (eventQueue->top + 1) & (MAX_QUEUE - 1);
	eventQueue->cant--;
}

void supress_LedEventQueue(LedEventQueue* ledEventQueue){
	ledEventQueue->top = (ledEventQueue->top + 1) & (MAX_QUEUE - 1);
	ledEventQueue->cant--;
}

void supress_OrderQueue(OrderQueue* orderQueue){
	orderQueue->top = (orderQueue->top + 1) & (MAX_QUEUE - 1);
	orderQueue->cant--;
}


/* Quitar el pedido de la lista y traducirlo en flag para el array */
void translator(OrderQueue* orderQueue, arrayOfFloors* aof){
	int8_t order, i;
	while(orderQueue->cant > 0){
		if(consult_OrderQueue(orderQueue, &order)){
			supress_OrderQueue(orderQueue);
			i = order + aof->amountSubs;
			aof->array[i].flag = 1;
		}
	}
}


void verify_order(arrayOfFloors* aof, StateMachineElevator *sme){
	if(sme->direction == GOING_UP){
		int8_t proxFloor, proxIdx;
		proxFloor = sme->currentFloor + 1;
		if(proxFloor <= (aof->amountFloors + 1 + aof->amountSubs)){
			proxIdx = proxFloor + aof->amountSubs;
			if(aof->array[proxIdx].flag == 1){
				sme->destiny = proxFloor;
			}
		}
	}
	else if(sme->direction == GOING_DOWN){
		int8_t proxFloor, proxIdx;
		proxFloor = sme->currentFloor - 1;
		if(proxFloor >= (-aof->amountSubs)){
			proxIdx = proxFloor + aof->amountSubs;
			if(aof->array[proxIdx].flag == 1){
				sme->destiny = proxFloor;
			}
		}
	}
}




