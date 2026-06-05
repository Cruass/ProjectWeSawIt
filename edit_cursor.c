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
static void loadFile(Cursor *cursor, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        // Jika file tidak ada, buat satu baris kosong agar editor tetap hidup
        appendNode(cursor, "");
        return;
    }

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
void hideCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &info);
}

void showCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &info);
}

void render(Cursor *cursor) {
    hideCursor();
    CONSOLE_SCREEN_BUFFER_INFO infoLayar;
    GetConsoleScreenBufferInfo(hConsole, &infoLayar);
    int lebarLayar  = infoLayar.dwSize.X;
    int tinggiLayar = infoLayar.dwSize.Y - 1;

    static int scrollOffset = 0;
    if (cursor->cursorRow < scrollOffset) 
    {
        scrollOffset = cursor->cursorRow;
    }
    if (cursor->cursorRow >= scrollOffset + tinggiLayar)
    {
        scrollOffset = cursor->cursorRow - tinggiLayar + 1;
    }

    Node *cur = cursor->head;
    int i = 0;
    while (cur != NULL && i < scrollOffset) {
        cur = cur->next;
        i++;
    }

    WORD warnaNormal  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    WORD warnaSeleksi = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    int baris = 0;
    while (cur != NULL && baris < tinggiLayar) {
    int barisDokumen = scrollOffset + baris;
    int panjang = strlen(cur->data);

    // Tulis seluruh baris 
    DWORD ditulis;
    COORD posAwal = {0, (SHORT)baris};
    WriteConsoleOutputCharacterA(hConsole, cur->data, panjang, posAwal, &ditulis);

    // Bersihkan sisa kanan baris
    int sisaKanan = lebarLayar - panjang;
    if (sisaKanan > 0) {
        FillConsoleOutputCharacter(hConsole, ' ', sisaKanan,
            (COORD){(SHORT)panjang, (SHORT)baris}, &ditulis);
        }

    // Reset warna seluruh baris ke normal 
    FillConsoleOutputAttribute(hConsole, warnaNormal, lebarLayar, posAwal, &ditulis);

    // Terapkan warna seleksi hanya di karakter yang terseleksi
    if (cursor->selAktif) {
        int kolom = 0;
        while (kolom < panjang) {
            if (cekPosisiBlok(cursor, barisDokumen, kolom)) {
                // Cari ujung blok seleksi yang berurutan
                int mulai = kolom;
                while (kolom < panjang && cekPosisiBlok(cursor, barisDokumen, kolom))
                    kolom++;
                // Warnai blok seleksi sekaligus
                COORD posSeleksi = {(SHORT)mulai, (SHORT)baris};
                FillConsoleOutputAttribute(hConsole, warnaSeleksi, kolom - mulai, posSeleksi, &ditulis);
                } else {
                     kolom++;
                }
            }
        }

        cur = cur->next;
        baris++;
    }

    // Bersihkan baris kosong di bawah dokumen
    while (baris < tinggiLayar) {
        DWORD ditulis;
        FillConsoleOutputCharacter(hConsole, ' ', lebarLayar,
            (COORD){0, (SHORT)baris}, &ditulis);
        FillConsoleOutputAttribute(hConsole, warnaNormal, lebarLayar,
            (COORD){0, (SHORT)baris}, &ditulis);
        baris++;
    }

    gotoxy(cursor->cursorCol, cursor->cursorRow - scrollOffset);
    showCursor();
}


/* Run editor */
void runEditor(const char *filename) {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    //Fix bug: tambah FlushConsoleInputBuffer setelah SetConsoleMode
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD dwOriginalMode;
    GetConsoleMode(hInput, &dwOriginalMode);
    SetConsoleMode(hInput, 0);
    FlushConsoleInputBuffer(hInput);
    Cursor cursor = {NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0};

    // BUG FIX 3: deklarasi undo/redo stack di sini
    UndoStack *undoStack = (UndoStack *)malloc(sizeof(UndoStack));  //Fix bug: mengganti deklarasi
    UndoStack *redoStack = (UndoStack *)malloc(sizeof(UndoStack));
    initStack(undoStack);
    initStack(redoStack);

    loadFile(&cursor, filename);

    // BUG FIX 2: pastikan current menunjuk ke head setelah load
    cursor.current  = cursor.head;
    cursor.cursorRow = 0;
    cursor.cursorCol = 0;

    int ch;
    while (1) {
        render(&cursor);
        ch = _getch();

        if (ch == 27) {
            saveFile(&cursor, filename);
            break;
        }

        if (ch == 224) {
            ch = _getch();
            if (GetKeyState(VK_SHIFT) & 0x8000) {
                handleSelection(ch, &cursor);
            } else {
                cursor.selAktif = 0; // Nonaktifkan blok jika Shift tidak ditekan
                handleCursorMovement(ch, &cursor); // Handle cursor movement jika Shift tidak ditekan

            }
        } else {
            if (ch == 3) {
                copySelection(&cursor);
            } else if (ch == 16) {
                pasteClipboard(&cursor);
            } else if (ch == 26) { // CTRL+Z undo       //Fix Bug: karena sudah pake pointer jadi gaperlu pake &cursor lagi
                doUndo(undoStack, redoStack, &cursor);
            } else if (ch == 25) { // CTRL+Y redo
                doRedo(undoStack, redoStack, &cursor);
            } else {
                // BUG FIX 1: panggil handleTextEditing!
                cursor.selAktif = 0;
                saveUndoState(undoStack, redoStack, &cursor); // simpan state sebelum edit
                handleTextEditing(ch, &cursor);
            }
        }
    }
    //Fix bug: pastikan semua resource dibersihkan sebelum keluar
    // Bersihkan semua resource
    freeList(&cursor);
    freeUndoStack(undoStack);   // bebaskan node-node di dalam stack
    freeUndoStack(redoStack);
    free(undoStack);
    free(redoStack);
    SetConsoleMode(hInput, dwOriginalMode);
    system("cls");
}