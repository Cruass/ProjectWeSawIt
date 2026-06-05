#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "irfan1.h"
#include "edit_cursor.h"
#include "linkedlist.h"

// HELPER: INSERT NODE DI TENGAH

void insertNodeAt(Cursor *cursor, int index, const char line[]) {
    Node *newNode = createNode(line);
    if (newNode == NULL) return;

    if (cursor->head == NULL || index == 0) {
        newNode->next = cursor->head;
        if (cursor->head != NULL) cursor->head->prev = newNode;
        cursor->head = newNode;
        cursor->rowCount++;
        return;
    }

    // Cari node pada posisi index-1
    Node *prev = cursor->head;
    int i = 0;
    while (prev->next != NULL && i < index - 1) {
        prev = prev->next;
        i++;
    }

    newNode->next = prev->next;
    newNode->prev = prev;
    if (prev->next != NULL) prev->next->prev = newNode;
    prev->next = newNode;
    cursor->rowCount++;
}

// HELPER: HAPUS NODE DI TENGAH
void deleteNodeAt(Cursor *cursor, int index) {
    if (cursor->head == NULL) return;

    Node *target = cursor->head;
    int i = 0;
    while (target != NULL && i < index) {
        target = target->next;
        i++;
    }
    if (target == NULL) return;

    if (target->prev != NULL) target->prev->next = target->next;
    else cursor->head = target->next;

    if (target->next != NULL) target->next->prev = target->prev;

    free(target);
    cursor->rowCount--;
}

// CREATE NEW FILE
void createNewFile(void) {
    char filename[50];

    printf("Masukkan Nama File : ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    // Buat file kosong dulu agar bisa dibuka oleh runEditor
    FILE *f = fopen(filename, "w");
    if (f) fclose(f);

    runEditor(filename);
    printf("File berhasil disimpan.\n");
}

// EXIT EDITOR
void exitEditor(void) {
    printf("Terimakasih!\n");
    exit(0);
}

// HANDLE TEXT EDITING
void handleTextEditing(int ch, Cursor *cursor) 
{
    Node *curr = cursor->current;
    if (curr == NULL) return;

    // BACKSPACE
    if (ch == 8) {
        if (cursor->cursorCol > 0) {
            // Hapus karakter di dalam baris
            int len = strlen(curr->data);
            if (cursor->cursorCol <= len) {
                for (int i = cursor->cursorCol; i <= len; i++)
                    curr->data[i - 1] = curr->data[i];
                cursor->cursorCol--;
            }
        } else if (cursor->cursorRow > 0) {
            // Gabung dengan baris sebelumnya
            Node *prev = curr->prev;
            if (prev == NULL) return;

            if (curr->data[0] == '\0') {              // Jika baris saat ini kosong
                deleteNodeAt(cursor, cursor->cursorRow);
                cursor->cursorRow--;
                cursor->current = prev;
                cursor->cursorCol = strlen(prev->data);
                return;                               // Keluar, tidak perlu gabung teks
                }  

            int space = MAX_COLS - 1 - strlen(prev->data); // ruang tersisa

            if (space > 0) {
                // 1. Tempelkan maksimal 'space' karakter dari curr ke prev
                strncat(prev->data, curr->data, space);
                prev->data[MAX_COLS - 1] = '\0';   // pastikan null-terminator (jika strncat mentok)

                // 2. Tentukan berapa karakter yang benar-benar dipindahkan
                int take = (strlen(curr->data) < space) ? strlen(curr->data) : space;

                // 3. Geser sisa teks di curr ke kiri (menghapus 'take' karakter pertama)
                memmove(curr->data, curr->data + take, strlen(curr->data) - take + 1);

                // 4. Jika curr sekarang kosong, hapus node-nya; jika tidak, kursor di awal sisa
                if (curr->data[0] == '\0') {
                    deleteNodeAt(cursor, cursor->cursorRow);
                    cursor->cursorRow--;
                    cursor->current = prev;
                    cursor->cursorCol = strlen(prev->data); // kursor di akhir teks gabungan
                } else {
                    cursor->cursorCol = 0; // kursor di awal sisa teks di baris kedua
                }
            }else {
                cursor->current = prev;
                cursor->cursorRow--;
                cursor->cursorCol = strlen(prev->data); // kursor di akhir baris sebelumnya
            }
        }
    }

    // ENTER
    else if (ch == 13) {
        char tail[MAX_COLS] = "";
        if (cursor->cursorCol < (int)strlen(curr->data)) {
            strcpy(tail, &curr->data[cursor->cursorCol]);
        }
        curr->data[cursor->cursorCol] = '\0';

        insertNodeAt(cursor, cursor->cursorRow + 1, tail);

        // Update current ke node baru
        cursor->current = curr->next;
        cursor->cursorRow++;
        cursor->cursorCol = 0;
    }

    // KARAKTER BIASA (ASCII 32-126)
    else if (ch >= 32 && ch <= 126) {
        // Jika kolom mentok, buat baris baru (word-wrap)
    if (cursor->cursorCol >= MAX_COLS - 1) {
        char tail[MAX_COLS] = "";
        strcpy(tail, &curr->data[cursor->cursorCol]);
        curr->data[cursor->cursorCol] = '\0';

        insertNodeAt(cursor, cursor->cursorRow + 1, tail);
        cursor->cursorRow++;
        cursor->cursorCol = 0;
        cursor->current = curr->next;
        curr = cursor->current;
        if (curr == NULL) return;
    }

        // Sisipkan karakter di posisi kursor
    int len = strlen(curr->data);
    if (len < MAX_COLS - 1) {
        for (int i = len; i >= cursor->cursorCol; i--)
             curr->data[i + 1] = curr->data[i];
        curr->data[cursor->cursorCol] = (char)ch;
        cursor->cursorCol++;
    } else {
            // Baris Penuh : cek karakter paling kanan
        char lastChar = curr->data[MAX_COLS - 2];
        if (lastChar == ' ') {
            for (int i = MAX_COLS - 2; i > cursor->cursorCol; i--)
                curr->data[i] = curr->data[i-1];
            curr->data[cursor->cursorCol] = (char)ch;
            curr->data[MAX_COLS - 1] = '\0';
            cursor->cursorCol++;
        } else {

            // Karakter paling kanan bukan spasi => overflow ke baris berikutnya
            char overflow[2] = { lastChar, '\0'};
            for (int i = MAX_COLS - 2; i > cursor->cursorCol; i--)
                curr->data[i] = curr->data[i-1];
            curr->data[cursor->cursorCol] = (char)ch;
            curr->data[MAX_COLS -1] = '\0';
            cursor->cursorCol++;

            if (curr->next != NULL) {
                // Sisip overflow di awal baris berikutnya
                Node *nextNode = curr->next;
                int nextLen = strlen(nextNode->data);
                if (nextLen < MAX_COLS - 1) {
                        for (int i = nextLen; i >= 0; i--)
                            nextNode->data[i+1] = nextNode->data[i];
                        nextNode->data[0] = overflow[0];    
                    }
                } else {
                    //Tidak ada baris berikutnya, buat baris baru
                    insertNodeAt(cursor, cursor->cursorRow + 1, overflow);
                }
            //Kalau kursor sudah di ujung baris, pindah ke bari berikutnya
                if (cursor->cursorCol >= MAX_COLS - 1) {
                    cursor->current = curr->next;
                    cursor->cursorRow++;
                    cursor->cursorCol = 0;
                    if (cursor->current == NULL) cursor->current = curr;
                }
            }
        }
    }
}