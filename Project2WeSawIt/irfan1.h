#ifndef irfan_h1
#define irfan_h1

// Deklarasi fungsi-fungsi editor
void createNewFile();
void exitEditor ();
void handleTextEditing(int ch, char text[][256], int *cursorRow, int *cursorCol, int *rowCount);

#endif