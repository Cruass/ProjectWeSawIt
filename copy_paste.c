#include <stdio.h>
#include <string.h>
#include "copy_paste.h"
#include "linkedlist.h"
#include "irfan1.h"   // agar bisa pakai insertNodeAt


static char clipboard[2000] = {0};   // clipboard lokal di file ini

// Fungsi copy (block selection)
void copySelection(Cursor *cursor) {
    clipboard[0] = '\0';
    if (!cursor->selAktif) return;

    int r0 = cursor->selMulaiBaris, c0 = cursor->selMulaiKolom;
    int r1 = cursor->selAkhirBaris, c1 = cursor->selAkhirKolom;

    // Normalisasi
    if (r0 > r1 || (r0 == r1 && c0 > c1)) {
        int t = r0; r0 = r1; r1 = t;
        t = c0; c0 = c1; c1 = t;
    }

    Node *cur = cursor->head;
    int row = 0;
    while (cur && row <= r1) {
        if (row >= r0) {
            int len = strlen(cur->data);
            int dari = (row == r0) ? c0 : 0;
            int ke   = (row == r1) ? c1 : len;
            if (dari > len) dari = len;
            if (ke > len) ke = len;
            if (ke > dari) {
                strncat(clipboard, cur->data + dari, ke - dari);
            }
            if (row != r1) strcat(clipboard, "\n");
        }
        cur = cur->next;
        row++;
    }
}

// Fungsi paste
void pasteClipboard(Cursor *cursor) {
    if (!clipboard[0] || !cursor->current) return;

    Node *curr = cursor->current;
    char kanan[MAX_COLS];
    strcpy(kanan, curr->data + cursor->cursorCol);
    curr->data[cursor->cursorCol] = '\0';

    char *p = clipboard;
    char *end = clipboard + strlen(clipboard);
    int baris = 0;

    while (p <= end) {
        char *nl = strchr(p, '\n');
        int panjang = nl ? (int)(nl - p) : (int)(end - p);

        if (baris == 0) {
            strncat(curr->data, p, panjang);
            cursor->cursorCol += panjang;
        } else {
            char baris_baru[MAX_COLS] = "";
            strncat(baris_baru, p, panjang < MAX_COLS-1 ? panjang : MAX_COLS-1);
            // Langsung panggil insertNodeAt dari irfan1.c
            insertNodeAt(cursor, cursor->cursorRow + baris, baris_baru);
        }
        if (!nl) break;
        p = nl + 1;
        baris++;
    }

    // Tempel sisa kanan ke node terakhir
    Node *terakhir = cursor->current;
    for (int i = 0; i < baris; i++)
        if (terakhir->next) terakhir = terakhir->next;
    strncat(terakhir->data, kanan, MAX_COLS - 1 - strlen(terakhir->data));

    // Update posisi kursor
    cursor->current = terakhir;
    cursor->cursorRow += baris;
    cursor->cursorCol = strlen(terakhir->data) - strlen(kanan);
}