#include "elevator.h"
#include "sapi.h"
#include "events.h"
#include "display.h"
#include <stdlib.h>


char menu[] = "\r\n|------------------------------------------------------|\r"
		      "\r\n| Bienvenido al sistema de configuracion del ascensor! |\r"
		      "\r\n|------------------------------------------------------|\r"
		      "\r\n| 1. Configurar velocidad entre piso y piso            |\r"
			  "\r\n| 2. Configurar velocidad de puerta                    |\r"
			  "\r\n| 3. Configurar cantidad de pisos                      |\r"
		      "\r\n| 4. Configurar cantidad de subsuelos                  |\r"
			  "\r\n| 5. Salir del modo configuracion                      |\r"
		      "\r\n|------------------------------------------------------|\r\n";

char menu1[] = "\r\n\n|------------------------------------------|\r"
		         "\r\n|  Que velocidad desea entre piso y piso?  |\r"
			     "\r\n|------------------------------------------|\r"
			     "\r\n| Ingrese el valor que desee, entre 1 y 9: |\r"
		         "\r\n|------------------------------------------|\r";

char menu2[] = "\r\n\n|------------------------------------------|\r"
		         "\r\n|  Que velocidad desea para las puertas?   |\r"
			     "\r\n|------------------------------------------|\r"
			     "\r\n| Ingrese el valor que desee, entre 1 y 9: |\r"
		         "\r\n|------------------------------------------|\r";

char menu3[] = "\r\n\n|-------------------------------------------|\r"
		         "\r\n|       Que cantidad de pisos desea?        |\r"
			     "\r\n|-------------------------------------------|\r"
			     "\r\n| Ingrese el valor que desee, entre 1 y 20: |\r"
		         "\r\n|-------------------------------------------|\r";

char menu4[] = "\r\n\n|------------------------------------------|\r"
		         "\r\n|     Que cantidad de subsuelos desea?     |\r"
			     "\r\n|------------------------------------------|\r"
			     "\r\n| Ingrese el valor que desee, entre 0 y 5: |\r"
		         "\r\n|------------------------------------------|\r";

char menu5[] = "\r\n\n|------------------------------------------|\r"
			     "\r\n|       Resumen de la Configuracion        |\r"
	             "\r\n|------------------------------------------|\r";

StateMachineElevator sme;
arrayOfFloors aof;
uint8_t speedBetweenFloors;
uint8_t speedDoors;
bool_t onEntry, timerInt, blocked, order, go, open, alarm;
int8_t destiny;
Event e;
char text[DISPLAY_COLS + 1];

static char m[10];

/* Configuracion de los timers */
void setting_timers(void) {
	Chip_TIMER_Init(LPC_TIMER0);   // Velocidad entre pisos
	Chip_TIMER_Init(LPC_TIMER1);   // Velocidad de puerta
	Chip_TIMER_Init(LPC_TIMER2);   // Espera de puerta abierta
	Chip_TIMER_PrescaleSet(LPC_TIMER0, 20400 - 1);  //Baja la frecuencia a 10kHz (204Mhz/20400)
	Chip_TIMER_PrescaleSet(LPC_TIMER1, 20400 - 1);
	Chip_TIMER_PrescaleSet(LPC_TIMER2, 20400 - 1);
	Chip_TIMER_SetMatch(LPC_TIMER0, 0, (speedBetweenFloors*10000));
    Chip_TIMER_SetMatch(LPC_TIMER1, 0, (speedDoors*10000));
    Chip_TIMER_SetMatch(LPC_TIMER2, 0, 20000);
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 0);
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER1, 0);
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 0);
    Chip_TIMER_MatchEnableInt(LPC_TIMER0, 0);
    Chip_TIMER_MatchEnableInt(LPC_TIMER1, 0);
    Chip_TIMER_MatchEnableInt(LPC_TIMER2, 0);

    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_SetPriority(TIMER0_IRQn, 1);

    NVIC_DisableIRQ(TIMER1_IRQn);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NVIC_SetPriority(TIMER1_IRQn, 1);

    NVIC_DisableIRQ(TIMER2_IRQn);
    NVIC_ClearPendingIRQ(TIMER2_IRQn);
    NVIC_SetPriority(TIMER2_IRQn, 1);
}


/* Handlers de los timers */
void TIMER0_IRQHandler(void) {
    if (Chip_TIMER_MatchPending(LPC_TIMER0, 0)) {
    	timerInt = 1;
        Chip_TIMER_ClearMatch(LPC_TIMER0, 0);
    }
}


void TIMER1_IRQHandler(void) {
    if (Chip_TIMER_MatchPending(LPC_TIMER1, 0)) {
    	timerInt = 1;
        Chip_TIMER_ClearMatch(LPC_TIMER1, 0);
    }
}


void TIMER2_IRQHandler(void) {
    if (Chip_TIMER_MatchPending(LPC_TIMER2, 0)) {
    	timerInt = 1;
        Chip_TIMER_ClearMatch(LPC_TIMER2, 0);
    }
}


/* Auxiliar de los timers */
void disable_timer(aTimers_t timer){
	switch(timer){
	case t0: NVIC_DisableIRQ(TIMER0_IRQn);
			 Chip_TIMER_Disable(LPC_TIMER0);
		     break;
	case t1: NVIC_DisableIRQ(TIMER1_IRQn);
			 Chip_TIMER_Disable(LPC_TIMER1);
			 break;
	case t2: NVIC_DisableIRQ(TIMER2_IRQn);
			 Chip_TIMER_Disable(LPC_TIMER2);
			 break;
	}
}


void enable_timer(aTimers_t timer){
	switch(timer){
	case t0: NVIC_EnableIRQ(TIMER0_IRQn);
			 Chip_TIMER_Enable(LPC_TIMER0);
		     break;
	case t1: NVIC_EnableIRQ(TIMER1_IRQn);
			 Chip_TIMER_Enable(LPC_TIMER1);
			 break;
	case t2: NVIC_EnableIRQ(TIMER2_IRQn);
			 Chip_TIMER_Enable(LPC_TIMER2);
			 break;
	}
}


/* Auxiliar matchear los pisos con la estructura central de pedidos */
void match_floor(arrayOfFloors *aof){
	int8_t i;
	for(i = 0; i < (aof->amountFloors + 1 + aof->amountSubs); i++){
		aof->array[i].flag = 0;
		aof->array[i].floor = (-aof->amountSubs) + i;
	}
}


/* Auxiliar para dirigirse hacia arriba */
void direction_up(int8_t orderH, StateMachineElevator *sme){
	sme->destiny = orderH;
	onEntry = 1;
	insert_LedEventQueue(&ledEventQueue, eGoingUp);
	sme->direction = GOING_UP;
	insert_DisplayEventQueue(&displayEventQueue, eGoingUp);
	transition_to(sme, state_GOING_UP, GOING_UP);
}


/* Auxiliar para dirigirse hacia abajo */
void direction_down(int8_t orderL, StateMachineElevator *sme){
	sme->destiny = orderL;
	onEntry = 1;
	insert_LedEventQueue(&ledEventQueue, eGoingDown);
	sme->direction = GOING_DOWN;
	insert_DisplayEventQueue(&displayEventQueue, eGoingDown);
	transition_to(sme, state_GOING_DOWN, GOING_DOWN);
}


/* Funcion que verifica si hay pedidos pendientes */
bool_t check_order(arrayOfFloors *aof){
	int8_t i;
	for(i = 0; i < (aof->amountFloors + 1 + aof->amountSubs); i++){  /* Recorro el arreglo hasta encontrar un pedido */
		if(aof->array[i].flag){
			return 1;
		}
	}
	return 0;
}


/* Funcion que elige que orden consumir */
void select_order(StateMachineElevator* sme, arrayOfFloors *aof){
	int8_t orderH, orderL, i, idx, idxH, idxL;
	bool_t candidateH = 0, candidateL = 0;

	idx = sme->currentFloor + aof->amountSubs;

	/* Buscar pedidos de pisos superiores al actual */
	for(i = idx; i < (aof->amountSubs + 1 + aof->amountFloors); i++){
		if(aof->array[i].flag == 1){
			candidateH = 1;
			orderH = aof->array[i].floor;
			idxH = i;
			break;
		}
	}

	/* Buscar pedidos de pisos inferiores al actual */
	for(i = idx; i >= 0; i--){
		if(aof->array[i].flag == 1){
			candidateL = 1;
			orderL = aof->array[i].floor;
			idxL = i;
			break;
		}
	}

	/* Si direction == STOPED, entonces el ascensor esta en la situacion inicial. Nunca ha realizado un recorrido
	 * En este caso, se selecciona el pedido que esta mas cerca respecto al piso actual. Una vez elegida una
	 * direccion, se sigue en esta misma direccion hasta consumir todos los pedidos en ese sentido. Recien alli
	 * se cambia el sentido del ascensor - Heuristica LOOK */

	if(sme->direction == STOPED){  /* Situacion inicial y situacion cuando no hay mas pedidos */
		if(candidateH && candidateL){   /* Hay pedidos inferiores y superiores */
			/* ELijo el pedido mas cercano */
			int8_t disH = (sme->currentFloor - orderH)*-1;
			int8_t disL = sme->currentFloor - orderL;
			if(disH <= disL){
				direction_up(orderH, sme);
			}
			else{
				direction_up(orderL, sme);
			}
		}
		else if(candidateH){  /* Solo hay pedidos superiores */
			direction_up(orderH, sme);
		}
		else if(candidateL){  /* Solo hay pedidos inferiores */
			direction_down(orderL, sme);
		}
	}
	else if(sme->direction == GOING_UP){
		if(candidateH){  /* Solo hay pedidos superiores */
			direction_up(orderH, sme);
		}
		else if(candidateL){  /* Solo hay pedidos inferiores */
			direction_down(orderL, sme);
		}
	}
	else if(sme->direction == GOING_DOWN){
		if(candidateL){  /* Solo hay pedidos inferiores */
			direction_down(orderL, sme);
		}
		else if(candidateH){  /* Solo hay pedidos superiores */
			direction_up(orderH, sme);
		}
	}
}


/* Estado de configuracion */

void state_SETTING(StateMachineElevator* sme, arrayOfFloors* aof) {
	int value;
	char buffer[3];
	char message[512];
	bool_t endFlag = 0;
	uint8_t bufferUART = 0xFF;
	uint8_t uart = 0xFF;
	uint8_t index = 0;

	while (!endFlag) {
		uartWriteString(UART_USB, menu);
		while (uartReadByte(UART_USB, &bufferUART) == 0) { };
		switch(bufferUART){
		case '1': uartWriteString(UART_USB, menu1);
				  while (uartReadByte(UART_USB, &uart) == 0) { };
				  if(uart >= '1' && uart <= '9'){
					  speedBetweenFloors = uart - '0';
					  uart = 0xFF;
				  }
				  else{
					  uartWriteString(UART_USB, "\r\nIngresaste un valor invalido\r\n\n");
				  }
				  break;

		case '2': uartWriteString(UART_USB, menu2);
			  	  while (uartReadByte(UART_USB, &uart) == 0) { };
			  	  if(uart >= '1' && uart <= '9'){
			  		  speedDoors = uart - '0';
			  		  uart = 0xFF;
			  	  }
			  	  else{
			  		  uartWriteString(UART_USB, "\r\nIngresaste un valor invalido\r\n\n");
			  	  }
			  	  break;

		case '3': uartWriteString(UART_USB, menu3);
				  while(1){
					  while (uartReadByte(UART_USB, &uart) == 0) { };
					  uartWriteByte(UART_USB, uart);
					  if (uart == '\r') {
						  uartWriteString(UART_USB, "\r\n");
						  buffer[index] = '\0';  // Terminamos el string
						  break;
					  }

					  if (uart == '\b') {
						  if (index > 0) {
							  index--;           // Retroceder un carácter
							  uartWriteString(UART_USB, "\b \b");   // Borra el carácter en la consola
						  }
						  continue;
					  }

					  if (uart >= '0' && uart <= '9') {
						  if (index < 2) {
							  buffer[index++] = uart;
						  } else {
							  uartWriteString(UART_USB, "\r\nMaximo 2 digitos permitidos. Se ignoraran los digitos menos significativos restantes.\r\n\n");
							  break;
						  }
					  }
				  }
				  index = 0;
				  value = atoi(buffer);

				  if (value > 0 && value <= 20) {
					  aof->amountFloors = (uint8_t)value;
					  uart = 0xFF;
					  sprintf(message, "\r\nNumero ingresado: %d\r\n", aof->amountFloors);
					  uartWriteString(UART_USB, message);
				  }
				  else{
					  uartWriteString(UART_USB, "\r\nEl numero ingresado esta fuera de rango (1 a 20).\r\n");
				  }
				  break;

		case '4': uartWriteString(UART_USB, menu4);
				  while(1){
					  while (uartReadByte(UART_USB, &uart) == 0) { };
					  uartWriteByte(UART_USB, uart);
					  if (uart == '\r') {
						  uartWriteString(UART_USB, "\r\n");
						  buffer[index] = '\0';  // Termina el string
						  break;
					  }

					  if (uart == '\b') {
						  if (index > 0) {
							  index--;           // Retroceder un carácter
							  uartWriteString(UART_USB, "\b \b");   // Borra el carácter en la consola
						  }
						  continue;
					  }

					  if (uart >= '0' && uart <= '9') {
						  if (index < 2) {
							  buffer[index++] = uart;
						  } else {
							  uartWriteString(UART_USB, "\r\nMaximo 2 digitos permitidos. Se ignoraran los digitos menos significativos restantes.\r\n");
							  break;
						  }
					  }
				  }
				  index = 0;
				  value = atoi(buffer);

				  if (value >= 0 && value <= 5) {
					  aof->amountSubs = (uint8_t)value;
					  uart = 0xFF;
					  sprintf(message, "\r\nNumero ingresado: %d\r\n", aof->amountSubs);
					  uartWriteString(UART_USB, message);
				  }
				  else{
					  uartWriteString(UART_USB, "\r\nEl numero ingresado esta fuera de rango (0 a 5).\r\n");
				  }
				  break;

		case '5': endFlag = 1;
				  snprintf(message, sizeof(message),"%s\r"
					         "|  Velocidad de paso entre pisos: %d        |\r\n"
					         "|  Velocidad de apertura de puerta: %d      |\r\n"
					         "|  Cantidad de pisos: %2d                   |\r\n"
					         "|  Cantidad de subsuelos: %d                |\r\n"
					         "|------------------------------------------|\r\n"
					         "|     Saliste del modo configuracion!      |\r\n"
					         "|------------------------------------------|\r\n",
					         menu5, speedBetweenFloors, speedDoors,
					         aof->amountFloors, aof->amountSubs);
				  uartWriteString(UART_USB, message);
				  break;
		}
	}
	setting_timers();
	insert_LedEventQueue(&ledEventQueue, eOpenDoor);
	match_floor(aof);
	onEntry = 1;
	transition_to(sme, state_OPEN_DOOR, STOPED);
}


/* Estado de puerta cerrada */
void state_CLOSED_DOOR(StateMachineElevator* sme, arrayOfFloors* aof){
	if(order && go){
		go = 0;
		insert_LedEventQueue(&ledEventQueue, eClosedDoor);
		select_order(sme, aof);
	}
	if(order && open){
		onEntry = 1;
		open = 0;
		transition_to(sme, state_OPENING_DOOR, STOPED);
	}
}


/* Estado de puerta abierta */
void state_OPEN_DOOR(StateMachineElevator* sme, arrayOfFloors* aof){
	if(onEntry){
		onEntry = 0;
		insert_DisplayEventQueue(&displayEventQueue, eStop);
	}

	if(check_order(aof)){ /* Si hay pedidos, atenderlos */
		onEntry = 1;
		order = 1;
		transition_to(sme, state_CLOSING_DOOR, STOPED);
	}
	else{  /* Si no hay pedidos, ver si se quiere configurar */
		if(sme->currentFloor == 0){
			if(consult_EventQueue(&eventQueue, &e)){
				supress_EventQueue(&eventQueue);
				if(e == eSetting){
					transition_to(sme, state_SETTING, SETTING);
				}
			}
		}
	}
}


/* Estado de cerrando puerta */

void state_CLOSING_DOOR(StateMachineElevator* sme, arrayOfFloors* aof){
	if(onEntry){
		onEntry = 0;
		blocked = 0;
		alarm = 0;
		enable_timer(t2);
	}

	if(timerInt && blocked == 0){  /* Pasaron 2 seg de puerta abierta */
		timerInt = 0;
		disable_timer(t2);    /* Deshabilito el timer de espera de puerta */
		if(consult_EventQueue(&eventQueue, &e)){
			supress_EventQueue(&eventQueue);
			if(e == eThereIsSomeone){  /* Hay alguien, espero 2 seg y si sigue, enciendo alarma*/
				enable_timer(t2);
				Chip_TIMER_Reset(LPC_TIMER2);
				blocked = 1;
			}
			else{  /* Si no hay nadie, cierro la puerta */
				insert_LedEventQueue(&ledEventQueue, eClosingDoor);
				go = 1;
				transition_to(sme, state_CLOSED_DOOR, STOPED);
			}
		}
		else{
			insert_LedEventQueue(&ledEventQueue, eClosingDoor);
			go = 1;
			transition_to(sme, state_CLOSED_DOOR, STOPED);
		}
	}

	if(timerInt && blocked){  /* Pasaron 2 seg desde que habia alguien. Veo si se libero o enciendo alarma */
		timerInt = 0;
		disable_timer(t2);
		if(consult_EventQueue(&eventQueue, &e)){
			if(e == eThereIsNoOne){   /* Ya no hay nadie, cierro la puerta */
				supress_EventQueue(&eventQueue);
				insert_LedEventQueue(&ledEventQueue, eClosingDoor);
				go = 1;
				transition_to(sme, state_CLOSED_DOOR, STOPED);
				blocked = 0;
			}
			if(e == eThereIsSomeone){
				supress_EventQueue(&eventQueue);
				insert_LedEventQueue(&ledEventQueue, eAlarm);
				alarm = 1;
			}
		}
	}

	if(blocked && timerInt == 0 && alarm){
		if(e == eThereIsNoOne){   /* Alarma encendida hasta que ya no haya nadie */
			supress_EventQueue(&eventQueue);
			insert_LedEventQueue(&ledEventQueue, eEndAlarm);
			insert_LedEventQueue(&ledEventQueue, eClosingDoor);
			go = 1;
			transition_to(sme, state_CLOSED_DOOR, STOPED);
			blocked = 0;
		}
	}
}


/* Estado de abriendo puerta */

void state_OPENING_DOOR(StateMachineElevator* sme, arrayOfFloors* aof){
	if(onEntry){
		onEntry = 0;
		enable_timer(t1);
		insert_LedEventQueue(&ledEventQueue, eOpeningDoor);
	}

	if(timerInt){
		timerInt = 0;
		uartWriteString(UART_USB, "OPEN DOOR\r\n");
		insert_LedEventQueue(&ledEventQueue, eOpenDoor);
		disable_timer(t1);
		transition_to(sme, state_OPEN_DOOR, OPEN_DOOR);
	}
}


/* Estado subiendo */

void state_GOING_UP(StateMachineElevator* sme, arrayOfFloors* aof) {
	uint8_t idx;
	if(onEntry){
		onEntry = 0;
		enable_timer(t0);
	}

	if(sme->currentFloor == sme->destiny){
		aof->array[sme->currentFloor + aof->amountSubs].flag = 0;
		disable_timer(t0);
		insert_LedEventQueue(&ledEventQueue, eStop);
		open = 1;
		insert_DisplayEventQueue(&displayEventQueue, eStop);
		transition_to(sme, state_CLOSED_DOOR, STOPED);
	}
	else{
		if(timerInt){
			timerInt = 0;
			sme->currentFloor++;
		}
	}
}


/* Estado bajando */

void state_GOING_DOWN(StateMachineElevator* sme, arrayOfFloors* aof) {
	if(onEntry){
		onEntry = 0;
		enable_timer(t0);
	}

	if(sme->currentFloor == sme->destiny){
		aof->array[sme->currentFloor + aof->amountSubs].flag = 0;
		disable_timer(t0);
		insert_LedEventQueue(&ledEventQueue, eStop);
		open = 1;
		insert_DisplayEventQueue(&displayEventQueue, eStop);
		transition_to(sme, state_CLOSED_DOOR, STOPED);
	}
	else{
		if(timerInt){
			timerInt = 0;
			sme->currentFloor--;
		}
	}
}


/* Funcion de transicion */

void transition_to(StateMachineElevator* sme, StateHandler new_state, StateElevatorID id) {
    sme->handler = new_state;
    sme->current = id;
}


/* Actualizar la maquina */

void update_FSM_Elevator(StateMachineElevator* sme, arrayOfFloors* aof) {
	if (sme->handler) {
		sme->handler(sme, aof);
	}
}


/* Inicializar el ascensor */

void init_Elevator(StateMachineElevator* sme) {
	speedBetweenFloors = 1;
	speedDoors = 1;
	onEntry = 0;
	timerInt = 0;
	blocked = 0;
	order = 0;
	go = 0;
	open = 0;
	alarm = 0;
	sme->current = SETTING;
	sme->currentFloor = 0;
	sme->direction = STOPED;
	sme->destiny = 0;
	insert_DisplayEventQueue(&displayEventQueue, eSetting);
	transition_to(sme, state_SETTING, SETTING);
}


/* Inicializar la estructura central de pedidos */

void init_Floors(arrayOfFloors* aof){
	aof->amountSubs = 5;
	aof->amountFloors = 20;
	aof->index = 0;
	int8_t i;
	for(i = 0; i < MAX_FLOORS; i++){
		aof->array[i].flag = 0;
		aof->array[i].floor = (-aof->amountSubs) + i;
	}
}
