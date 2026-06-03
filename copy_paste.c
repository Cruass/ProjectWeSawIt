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

    // Langkah 1: Simpan posisi kolom awal sebelum paste
    int kolomAwal = cursor->cursorCol;

    // Langkah 2: Ambil teks kanan kursor, lalu potong baris sekarang di posisi kursor
    char kanan[MAX_COLS];
    strcpy(kanan, curr->data + cursor->cursorCol);
    curr->data[cursor->cursorCol] = '\0';

    // Langkah 3: Simpan panjang clipboard sebelum digabung
    int panjangClipboard = strlen(clipboard);

    // Langkah 4: Gabungkan clipboard + kanan jadi satu string
    char gabungan[MAX_COLS * 20] = "";
    strcat(gabungan, clipboard);
    strcat(gabungan, kanan);

    // Langkah 5: Hitung di baris dan kolom mana kursor harus berhenti
    int barisAkhirPaste = 0;
    int kolomAkhirPaste = 0;
    for (int i = 0; i < panjangClipboard; i++) {
        if (gabungan[i] == '\n') {
            barisAkhirPaste++;
            kolomAkhirPaste = 0;
        } else {
            kolomAkhirPaste++;
        }
    }

    // Langkah 6: Proses gabungan per baris (dipisah oleh \n)
    char *p = gabungan;
    char *end = gabungan + strlen(gabungan);
    int baris = 0;

    while (p <= end) {
        char *nl = strchr(p, '\n');
        int panjang = nl ? (int)(nl - p) : (int)(end - p);

        if (baris == 0) {
            // Baris pertama → tempel ke baris sekarang
            int sisa = MAX_COLS - 1 - strlen(curr->data);
            if (sisa > 0) {
                strncat(curr->data, p, panjang < sisa ? panjang : sisa);
            }

            // Kalau tidak muat → sisa turun ke baris baru
            if (panjang > sisa) {
                char tmpSisa[MAX_COLS] = "";
                int panjangSisa = panjang - sisa;
                strncat(tmpSisa, p + sisa, panjangSisa < MAX_COLS-1 ? panjangSisa : MAX_COLS-1);
                insertNodeAt(cursor, cursor->cursorRow + baris + 1, tmpSisa);
                baris++;
            }

        } else {
            // Baris berikutnya karena ada \n → buat node baru
            char baris_baru[MAX_COLS] = "";
            int sisa = MAX_COLS - 1;
            strncat(baris_baru, p, panjang < sisa ? panjang : sisa);
            insertNodeAt(cursor, cursor->cursorRow + baris, baris_baru);

            // Kalau baris baru ini juga tidak muat → sisa turun lagi
            if (panjang > sisa) {
                char tmpSisa[MAX_COLS] = "";
                int panjangSisa = panjang - sisa;
                strncat(tmpSisa, p + sisa, panjangSisa < MAX_COLS-1 ? panjangSisa : MAX_COLS-1);
                insertNodeAt(cursor, cursor->cursorRow + baris + 1, tmpSisa);
                baris++;
            }
        }

        if (!nl) break;
        p = nl + 1;
        baris++;
    }

    // Langkah 7: Update posisi kursor tepat setelah teks yang di-paste
    Node *terakhir = cursor->current;
    for (int i = 0; i < barisAkhirPaste; i++)
        if (terakhir->next) terakhir = terakhir->next;

    cursor->current   = terakhir;
    cursor->cursorRow += barisAkhirPaste;

    // Kalau paste satu baris → kolom awal + panjang clipboard
    // Kalau paste multi baris → mulai dari 0 di baris baru
    if (barisAkhirPaste == 0) {
        cursor->cursorCol = kolomAwal + kolomAkhirPaste;
    } else {
        cursor->cursorCol = kolomAkhirPaste;
    }

    if (cursor->cursorCol > MAX_COLS - 1) cursor->cursorCol = MAX_COLS - 1;
}