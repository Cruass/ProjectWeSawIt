#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "edit_cursor.h"
#include "zidan.h"
#include "irfan1.h"
#include "linkedlist.h"
#include "copy_paste.h"
#include "undo_redo.h"

/* Handle konsol Windows untuk SetConsoleCursorPosition */
static HANDLE hConsole;


/* LOAD FILE KE LINKED LIST */
static void loadFile(Cursor *cursor, const char *filename) { // Fungsi dibuat oleh Rayhan
    FILE *f = fopen(filename, "r");
    if (!f) return;

    char buffer[MAX_COLS];
    while (fgets(buffer, MAX_COLS, f)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        appendNode(cursor, buffer);
    }
    fclose(f);

    // Jika file kosong, tambah satu baris kosong
    if (cursor->rowCount == 0) appendNode(cursor, "");
}

/* Simpan file */
void saveFile(Cursor *cursor, const char *filename) { // Fungsi dibuat oleh Irfan
    FILE *f = fopen(filename, "w");
    if (!f) return;

    Node *cur = cursor->head;
    while (cur != NULL) {
        fprintf(f, "%s", cur->data);
        if (cur->next != NULL) fprintf(f, "\n");
        cur = cur->next;
    }
    fclose(f);
}

void gotoxy(int x, int y) {
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hConsole, coord);
}

int cekPosisiBlok(Cursor *cur, int baris, int kolom) {
    if (cur->selAktif == 0) return 0; // Tidak ada blok aktif

    int barisAwal = cur->selMulaiBaris;
    int kolomAwal = cur->selMulaiKolom;
    int barisAkhir = cur->selAkhirBaris;
    int kolomAkhir = cur->selAkhirKolom;

    if (barisAwal > barisAkhir) {
        int temp;
        temp = barisAwal;
        barisAwal = barisAkhir;
        barisAkhir = temp;

        temp = kolomAwal;
        kolomAwal = kolomAkhir;
        kolomAkhir = temp;
    }
    else if (barisAwal == barisAkhir && kolomAwal > kolomAkhir) {
        int temp = kolomAwal;
        kolomAwal = kolomAkhir;
        kolomAkhir = temp;
    }

    if (baris < barisAwal || baris > barisAkhir) return 0;
    if (baris == barisAwal && kolom < kolomAwal) return 0;
    if (baris == barisAkhir && kolom >= kolomAkhir) return 0;

    return 1; // Posisi berada dalam blok
}

void render(Cursor *cursor) {
    CONSOLE_SCREEN_BUFFER_INFO infoLayar;
    GetConsoleScreenBufferInfo(hConsole, &infoLayar);
    int lebarLayar  = infoLayar.dwSize.X;
    int tinggiLayar = infoLayar.dwSize.Y - 1; 

    // Update scrollOffset agar kursor tetap dalam pandangan
    static int scrollOffset = 0;
    if (cursor->cursorRow < scrollOffset) scrollOffset = cursor->cursorRow;
    if (cursor->cursorRow >= scrollOffset + tinggiLayar) 
        scrollOffset = cursor->cursorRow - tinggiLayar + 1;

    // Arahkan ke node awal berdasarkan offset
    Node *cur = cursor->head;
    int i = 0;
    while (cur != NULL && i < scrollOffset) {
        cur = cur->next;
        i++;
    }

    // Render baris ke layar
    int row = 0;
    while (cur != NULL && row < tinggiLayar) {
        gotoxy(0, row);

        int barisDokumen = scrollOffset + row;
        int panjang = strlen(cur->data);

        int kolom = 0;
        while(kolom < panjang) {
            if (cekPosisiBlok(cursor, barisDokumen, kolom)) {
                SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            else {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            putchar(cur->data[kolom]);
            kolom++;
            
        } 

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        int sisaKanan = lebarLayar - panjang;

        // Bersihkan sisa karakter di ujung kanan
        if (sisaKanan > 0) {
            DWORD jumlahDitulis;
            FillConsoleOutputCharacter(hConsole, ' ', sisaKanan, (COORD){(SHORT)panjang, (SHORT)row}, &jumlahDitulis);
        }
        cur = cur->next;
        row++;
    }

    // Bersihkan baris di bawah konten (area kosong)
    int barisSisa = row;
    while (barisSisa < tinggiLayar) {
        DWORD jumlahDitulis;
        FillConsoleOutputCharacter(hConsole, ' ', lebarLayar,
            (COORD){0, (SHORT)barisSisa}, &jumlahDitulis);
        barisSisa++;
    }

    // Taruh kursor di posisi yang benar
    gotoxy(cursor->cursorCol, cursor->cursorRow - scrollOffset);
}


/* Run editor */
void runEditor(const char *filename) { // Fungsi dibuat oleh Rayhan
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(hInput, 0);

    // Inisialisasi cursor
    Cursor cursor = {NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0};

    // Load file ke linked list
    loadFile(&cursor, filename);

    cursor.cursorRow = 0;
    cursor.cursorCol = 0;

    // Inisialisasi undo/redo stack
    UndoStack undoStack, redoStack;
    initStack(&undoStack);
    initStack(&redoStack);

    int ch;
    while (1) {
        render(&cursor);
        ch = _getch();

        if (ch == 27) { // ESC untuk keluar dan simpan, dibuat oleh Rayhan
            saveFile(&cursor, filename);
            break;
        }

        if (ch == 224) { // Memanggil fungsi cursor movement dari zidan.c
            ch = _getch();

            if (GetKeyState(VK_SHIFT) & 0x8000) {
                handleSelection(ch, &cursor); // Handle selection jika Shift ditekan
            } else {
                cursor.selAktif = 0; // Nonaktifkan blok jika Shift tidak ditekan
                handleCursorMovement(ch, &cursor); // Handle cursor movement jika Shift tidak ditekan
            }
        } else if (ch == 26) { // CTRL+Z untuk undo
            doUndo(&undoStack, &redoStack, &cursor);
        } else if (ch == 25) { // CTRL+Y untuk redo
            doRedo(&undoStack, &redoStack, &cursor);
        } else {
            if (ch == 3) { // CTRL+C untuk copy, dibuat oleh Rayhan
                copySelection(&cursor);
            } else if (ch == 16) { // CTRL+P untuk paste, dibuat oleh Rayhan
                // Simpan state sebelum paste
                saveUndoState(&undoStack, &redoStack, &cursor);
                pasteClipboard(&cursor);
            } else {
                cursor.selAktif = 0; // Nonaktifkan blok jika tombol lain ditekan
                // Simpan state sebelum mengedit
                saveUndoState(&undoStack, &redoStack, &cursor);
                handleTextEditing(ch, &cursor); // Memanggil fungsi handleTextEditing dari irfan1.c
            }
        }
    }

    freeList(&cursor);
}   