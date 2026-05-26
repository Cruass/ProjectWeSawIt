#ifndef UNDO_REDO_H
#define UNDO_REDO_H

#include "linkedlist.h"

#define MAX_UNDO 50
#define MAX_LINES 500

typedef struct {
    char lines[MAX_LINES][MAX_COLS];
    int lineCount;
    int cursorRow;
    int cursorCol;
} DataUndo;

typedef struct {
    DataUndo states[MAX_UNDO];
    int top;
} UndoStack;

//Fungsi stack
void initStack          (UndoStack *stack);
int isStackEmpty        (UndoStack *stack);
int isStackFull         (UndoStack *stack);
void pushDataUndo       (UndoStack *stack, DataUndo d);
DataUndo popDataUndo    (UndoStack *stack);

//Fungsi untuk menyimpan state
void saveUndoState  (UndoStack *undoStack, UndoStack *redoStack, Cursor *cursor);
void doUndo         (UndoStack *udoStack, UndoStack *redoStack, Cursor *cursor);
void doRedo         (UndoStack *undoStack, UndoStack *redoStack, Cursor *cursor);

//Helper
DataUndo captureDataUndo (Cursor *cursor);
void applyDataUndo       (Cursor *cursor, DataUndo d);

#endif