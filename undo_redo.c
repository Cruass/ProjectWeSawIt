#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "undo_redo.h"

// Inisialisasi stack (top = -1 berarti kosong)
void initStack(UndoStack *s) {
    s->top = -1;
}

int isStackEmpty(UndoStack *s) {
    return s->top == -1;
}

int isStackFull(UndoStack *s) {
    return s->top == MAX_UNDO - 1;
}

// Push: simpan DataUndo ke puncak stack
void pushDataUndo(UndoStack *s, DataUndo d) {
    if (isStackFull(s)) {
        // Stack penuh → geser semua ke bawah (buang yang paling lama)
        for (int i = 0; i < MAX_UNDO - 1; i++) {
            s->data[i] = s->data[i + 1];
        }
        // top tetap di MAX_UNDO-1, tinggal overwrite
        s->data[s->top] = d;
    } else {
        s->top++;
        s->data[s->top] = d;
    }
}

// Pop: ambil DataUndo dari puncak stack
DataUndo popDataUndo(UndoStack *s) {
    // Pastikan stack tidak kosong sebelum pop
    DataUndo d = s->data[s->top];
    s->top--;
    return d;
}

// Capture: baca seluruh isi linked list → simpan ke DataUndo
DataUndo captureDataUndo(Cursor *cursor) {
    DataUndo d;
    d.lineCount = 0;
    d.cursorRow = cursor->cursorRow;
    d.cursorCol = cursor->cursorCol;

    Node *curr = cursor->head;
    while (curr != NULL && d.lineCount < MAX_LINES) {
        strncpy(d.lines[d.lineCount], curr->data, MAX_COLS - 1);
        d.lines[d.lineCount][MAX_COLS - 1] = '\0';
        d.lineCount++;
        curr = curr->next;
    }
    return d;
}

// Apply: terapkan DataUndo ke linked list (hapus list lama, isi ulang)
void applyDataUndo(Cursor *cursor, DataUndo d) {
    // Bebaskan linked list yang sekarang
    freeList(cursor);

    // Isi ulang dari DataUndo
    for (int i = 0; i < d.lineCount; i++) {
        appendNode(cursor, d.lines[i]);
    }

    // Restore posisi kursor
    cursor->cursorRow = d.cursorRow;
    cursor->cursorCol = d.cursorCol;

    // Set cursor->current ke node yang sesuai dengan cursorRow
    Node *curr = cursor->head;
    for (int i = 0; i < d.cursorRow && curr != NULL; i++) {
        curr = curr->next;
    }
    cursor->current = curr;
}

