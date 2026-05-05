#ifndef irfan1_h
#define irfan1_h
#include "edit_cursor.h"      // MAX_ROWS ada di sini

void createNewFile();
void exitEditor ();
void handleTextEditing(int ch, char text[][MAX_COLS], int *cursorRow, int *cursorCol, int *rowCount);

#endif