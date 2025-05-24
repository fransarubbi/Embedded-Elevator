#ifndef DISPLAY_H
#define DISPLAY_H

#include "sapi.h"
#include "elevator.h"

#define DISPLAY_COLS 16
#define DISPLAY_ROWS 2


typedef enum {
    DISPLAY_UPDATE_ROW0,
    DISPLAY_UPDATE_ROW1
} StateDisplay;


typedef struct {
    char row0_buffer[DISPLAY_COLS + 1];     // Buffer fila superior
    char row1_buffer[DISPLAY_COLS + 1];     // Buffer fila inferior
    char row0_current[DISPLAY_COLS + 1];    // Contenido actual fila 0
    char row1_current[DISPLAY_COLS + 1];    // Contenido actual fila 1
} DisplayDataManager;


extern StateDisplay stateDisplay;
extern DisplayDataManager displayDataManager;
extern char text_row1[DISPLAY_COLS + 1];
extern char number_text[2];


void init_Display(void);
void update_Display(void);
void set_row0(char[]);
void set_row1(char[]);


#endif
