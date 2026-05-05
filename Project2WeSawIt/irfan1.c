#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>          // tambahan untuk Beep
#include "irfan1.h"
#include "edit_cursor.h"      // MAX_ROWS ada di sini

void createNewFile() {
    char filename[100];
    printf("Masukkan Nama File : ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';
    
    runEditor(filename, 1);   // 1 = file baru
    printf("File berhasil disimpan.\n");
}

void exitEditor() {
    printf("Terimakasih!\n");
    exit(0);
}

void handleTextEditing(int ch, char text[][MAX_COLS], int *cursorRow, int *cursorCol, int *rowCount) {
    
    if (ch == 8) { // backspace
        if (*cursorCol > 0) {
            int len = strlen(text[*cursorRow]);
            for (int i = *cursorCol; i <= len; i++)
                text[*cursorRow][i-1] = text[*cursorRow][i];
            (*cursorCol)--;
        }
        else if (*cursorRow > 0) {
            int prevLen = strlen(text[*cursorRow-1]);
            strcat(text[*cursorRow-1], text[*cursorRow]);
            for (int i = *cursorRow; i < *rowCount - 1; i++)
                strcpy(text[i], text[i+1]);
            (*rowCount)--;
            (*cursorRow)--;
            (*cursorCol) = prevLen;
        }
    }

    else if (ch == 13) { // enter
        if (*rowCount >= MAX_ROWS) {
            printf("Sudah mencapai batas");   
            return;           
        }

        for (int i = *rowCount; i > *cursorRow + 1; i--)
            strcpy(text[i], text[i-1]);
        strcpy(text[*cursorRow + 1], &text[*cursorRow][*cursorCol]);
        text[*cursorRow][*cursorCol] = '\0';
        (*rowCount)++;
        (*cursorRow)++;
        (*cursorCol) = 0;
    }

    else if (ch >= 32 && ch <= 126) { // input karakter
    if (*cursorCol >= MAX_COLS - 1) {          
        for (int i = *rowCount; i > *cursorRow + 1; i--)
            strcpy(text[i], text[i-1]);
        strcpy(text[*cursorRow + 1], &text[*cursorRow][*cursorCol]);
        text[*cursorRow][*cursorCol] = '\0';
        (*rowCount)++;
        (*cursorRow)++;
        *cursorCol = 0;
    }
        int len = strlen(text[*cursorRow]);
        for (int i = len; i >= *cursorCol; i--)
            text[*cursorRow][i+1] = text[*cursorRow][i];
        text[*cursorRow][*cursorCol] = ch;
        (*cursorCol)++;
    }
}