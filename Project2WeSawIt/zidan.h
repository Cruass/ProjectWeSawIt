#ifndef ZIDAN_H
#define ZIDAN_H

#include "edit_cursor.h"

void findAndReplace();
void handleCursorMovement(int ch, int *cursorRow, int *cursorCol, int rowCount, char text[][MAX_COLS]);

#endif