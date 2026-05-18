#ifndef IRFAN1_H
#define IRFAN1_H

#include "linkedlist.h"   // karena menggunakan Node *

// Membuat file baru (dokumen kosong) dan menjalankan editor
void createNewFile(void);

// Keluar dari program
void exitEditor(void);

void handleTextEditing(int ch, Cursor *cursor);

#endif