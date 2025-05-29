#ifndef DISPLAY_H
#define DISPLAY_H

#include "sapi.h"
#include "elevator.h"

#define DISPLAY_COLS 16
#define DISPLAY_ROWS 2


/* Maquina de estados del display segun filas */
typedef enum {
    DISPLAY_UPDATE_ROW0,
    DISPLAY_UPDATE_ROW1
} StateDisplay;


/* Modo actual del display */
typedef enum{
	DISPLAY_MODE_SETTING,
	DISPLAY_MODE_STOP,
	DISPLAY_MODE_GOING_UP,
	DISPLAY_MODE_GOING_DOWN,
	DISPLAY_MODE_ALARM,
	DISPLAY_MODE_CLOSING,
	DISPLAY_MODE_OPENING
} DisplayMode;


/* Estructura de datos del display */
typedef struct {
	DisplayMode displayMode;                // Modo del display
    char row0_buffer[DISPLAY_COLS + 1];     // Buffer fila superior
    char row1_buffer[DISPLAY_COLS + 1];     // Buffer fila inferior
    char row0_current[DISPLAY_COLS + 1];    // Contenido actual fila 0
    char row1_current[DISPLAY_COLS + 1];    // Contenido actual fila 1
} DisplayDataManager;


/* Declarar variables */
extern StateDisplay stateDisplay;
extern DisplayDataManager displayDataManager;
extern char text_row0[DISPLAY_COLS + 1];
extern char text_row1[DISPLAY_COLS + 1];
extern char number_text[2];


void init_Display(void);
void update_Display(void);
void set_row0(char[]);
void set_row1(char[]);
void update_row_data(bool_t);

#endif
