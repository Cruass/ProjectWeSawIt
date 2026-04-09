#ifndef irfan1_h
#define irfan1_h

void createNewFile();
void exitEditor ();
void handleTextEditing(int ch, char text[][256], int *cursorRow, int *cursorCol, int *rowCount);

#endif