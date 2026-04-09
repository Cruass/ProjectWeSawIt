#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "edit_cursor.h"

#define MAX_ROWS 100
#define MAX_COLS 256

static char text[MAX_ROWS][MAX_COLS];
static int rowCount = 1;
static int cursorRow = 0, cursorCol = 0;
static HANDLE hConsole;

// Memuat file ke buffer (jika isNew == 0)
static void loadFile(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    rowCount = 0;
    while (rowCount < MAX_ROWS && fgets(text[rowCount], MAX_COLS, f)) {
        // hilangkan newline di akhir (karena editor tidak pakai newline internal)
        text[rowCount][strcspn(text[rowCount], "\n")] = '\0';
        rowCount++;
    }
    fclose(f);
    if (rowCount == 0) rowCount = 1;
}