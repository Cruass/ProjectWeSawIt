#ifndef IRFAN1_H
#define IRFAN1_H

#include "linkedlist.h"   // karena menggunakan Node *

// Membuat file baru (dokumen kosong) dan menjalankan editor
void createNewFile(void);

// Keluar dari program
void exitEditor(void);
void insertNodeAt(Cursor *cursor, int index, const char line[]);
void handleTextEditing(int ch, Cursor *cursor);

#endif