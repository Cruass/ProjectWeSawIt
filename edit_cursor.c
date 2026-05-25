#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "edit_cursor.h"
#include "zidan.h"
#include "irfan1.h"
#include "linkedlist.h"

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

static void gotoxy(int x, int y) {
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hConsole, coord);
}

static void render(Cursor *cursor) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int screenWidth  = csbi.dwSize.X;
    int screenHeight = csbi.dwSize.Y - 1; 

    // Update scrollOffset agar kursor tetap dalam pandangan
    static int scrollOffset = 0;
    if (cursor->cursorRow < scrollOffset) scrollOffset = cursor->cursorRow;
    if (cursor->cursorRow >= scrollOffset + screenHeight) 
        scrollOffset = cursor->cursorRow - screenHeight + 1;

    // Arahkan ke node awal berdasarkan offset
    Node *cur = cursor->head;
    for (int i = 0; cur != NULL && i < scrollOffset; i++) cur = cur->next;

    // Render baris ke layar
    int row = 0;
    while (cur != NULL && row < screenHeight) {
        gotoxy(0, row);
        printf("%s", cur->data);

        int textLen = (int)strlen(cur->data);
        int sisa    = screenWidth - textLen;

        // Bersihkan sisa karakter di ujung kanan
        if (sisa > 0) {
            DWORD written;
            FillConsoleOutputCharacter(hConsole, ' ', sisa, (COORD){(SHORT)textLen, (SHORT)row}, &written);
        }
        cur = cur->next;
        row++;
    }

    // Bersihkan baris di bawah konten (area kosong)
    for (int r = row; r < screenHeight; r++) {
        DWORD written;
        FillConsoleOutputCharacter(hConsole, ' ', screenWidth, (COORD){0, (SHORT)r}, &written);
    }

    // Set kursor ke posisi visual
    gotoxy(cursor->cursorCol, cursor->cursorRow - scrollOffset);
}



/* Run editor */
void runEditor(const char *filename) { // Fungsi dibuat oleh Rayhan
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Inisialisasi cursor
    Cursor cursor = {NULL, NULL, 0, 0, 0};

    // Load file ke linked list
    loadFile(&cursor, filename);

    cursor.cursorRow = 0;
    cursor.cursorCol = 0;

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
            handleCursorMovement(ch, &cursor);
        } else {
            handleTextEditing(ch, &cursor); // Memanggil fungsi handleTextEditing dari irfan1.c
        }
    }

    freeList(&cursor);
}