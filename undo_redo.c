#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "undo_redo.h"

// Inisialisasi stack kosong
void initStack(UndoStack *s) {
    s->top = NULL;
    s->size = 0;
}

int isStackEmpty(UndoStack *s) {
    return s->top == NULL;
}

// Push: tambahkan node di puncak
void pushDataUndo(UndoStack *s, DataUndo d) {
    UndoNode *newNode = (UndoNode*)malloc(sizeof(UndoNode));
    if (newNode == NULL) return;  // gagal alokasi
    newNode->data = d;
    newNode->next = s->top;
    s->top = newNode;
    s->size++;
}

// Pop: hapus node puncak dan kembalikan datanya
DataUndo popDataUndo(UndoStack *s) {
    // DataUndo kosong sebagai fallback
    DataUndo kosong;
    kosong.lineCount = 0;
    kosong.cursorRow = 0;
    kosong.cursorCol = 0;

    if (isStackEmpty(s)) return kosong;

    UndoNode *temp = s->top;
    DataUndo d = temp->data;
    s->top = s->top->next;
    free(temp);
    s->size--;
    return d;
}

// Bebaskan seluruh node dalam stack
void freeUndoStack(UndoStack *s) {
    while (!isStackEmpty(s)) {
        popDataUndo(s);   // pop sudah free node
    }
    // Tidak perlu free(s) karena s sendiri dialokasikan di luar
}

// Capture: baca seluruh linked list editor → simpan ke DataUndo
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

// Apply: terapkan DataUndo ke linked list editor
void applyDataUndo(Cursor *cursor, DataUndo d) {
    // Bebaskan linked list yang sekarang
    freeList(cursor);

    // Isi ulang dari DataUndo
    for (int i = 0; i < d.lineCount; i++) {
        appendNode(cursor, d.lines[i]);
    }

    // Jika setelah load tidak ada baris (misal lineCount = 0), buat baris kosong
    if (cursor->rowCount == 0) {
        appendNode(cursor, "");
    }

    // Restore posisi kursor
    cursor->cursorRow = d.cursorRow;
    cursor->cursorCol = d.cursorCol;

    // Pastikan cursorRow tidak melebihi jumlah baris
    if (cursor->cursorRow >= cursor->rowCount)
        cursor->cursorRow = cursor->rowCount - 1;
    if (cursor->cursorRow < 0) cursor->cursorRow = 0;

    // Set cursor->current ke node yang sesuai dengan cursorRow
    Node *curr = cursor->head;
    for (int i = 0; i < cursor->cursorRow && curr != NULL; i++) {
        curr = curr->next;
    }
    cursor->current = curr ? curr : cursor->head;

    // Pastikan kolom tidak melebihi panjang baris
    int len = (cursor->current) ? strlen(cursor->current->data) : 0;
    if (cursor->cursorCol > len) cursor->cursorCol = len;
}

// Dipanggil SEBELUM setiap perubahan teks
void saveUndoState(UndoStack *undoStack, UndoStack *redoStack, Cursor *cursor) {
    DataUndo d = captureDataUndo(cursor);
    pushDataUndo(undoStack, d);

    // Setiap aksi baru, redo stack harus dikosongkan
    freeUndoStack(redoStack);
    initStack(redoStack);
}

// Ctrl+Z
void doUndo(UndoStack *undoStack, UndoStack *redoStack, Cursor *cursor) {
    if (isStackEmpty(undoStack)) return;

    // Simpan kondisi sekarang ke redo stack
    DataUndo current = captureDataUndo(cursor);
    pushDataUndo(redoStack, current);

    // Ambil state sebelumnya
    DataUndo prev = popDataUndo(undoStack);
    applyDataUndo(cursor, prev);
}

// Ctrl+Y
void doRedo(UndoStack *undoStack, UndoStack *redoStack, Cursor *cursor) {
    if (isStackEmpty(redoStack)) return;

    // Simpan kondisi sekarang ke undo stack
    DataUndo current = captureDataUndo(cursor);
    pushDataUndo(undoStack, current);

    // Ambil state redo
    DataUndo next = popDataUndo(redoStack);
    applyDataUndo(cursor, next);
}