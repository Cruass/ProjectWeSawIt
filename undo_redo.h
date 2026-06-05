#ifndef UNDO_REDO_H
#define UNDO_REDO_H

#include "linkedlist.h"

#define MAX_LINES 500   // maksimal baris yang disimpan per state (tetap)

// Data dari satu state undo/redo (sama seperti sebelumnya)
typedef struct {
    char lines[MAX_LINES][MAX_COLS];
    int lineCount;
    int cursorRow;
    int cursorCol;
} DataUndo;

// Node untuk linked list stack
typedef struct UndoNode {
    DataUndo data;
    struct UndoNode *next;
} UndoNode;

// Stack menggunakan linked list
typedef struct {
    UndoNode *top;
    int size;           // opsional: jumlah elemen saat ini
} UndoStack;

// Fungsi stack
void initStack(UndoStack *s);
int isStackEmpty(UndoStack *s);
void pushDataUndo(UndoStack *s, DataUndo d);
DataUndo popDataUndo(UndoStack *s);
void freeUndoStack(UndoStack *s);   // membebaskan seluruh isi stack

// Fungsi untuk menyimpan state
void saveUndoState(UndoStack *undoStack, UndoStack *redoStack, Cursor *cursor);
void doUndo(UndoStack *undoStack, UndoStack *redoStack, Cursor *cursor);
void doRedo(UndoStack *undoStack, UndoStack *redoStack, Cursor *cursor);

// Helper
DataUndo captureDataUndo(Cursor *cursor);
void applyDataUndo(Cursor *cursor, DataUndo d);

#endif