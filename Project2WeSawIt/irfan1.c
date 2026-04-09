#include <stdio.h>
#include <string.h>
#include <stdlib.h>   // <-- tambahkan ini
#include "irfan1.h"
#include "edit_cursor.h"   // tambahkan header editor kursor

void createNewFile() {
    char filename[100];
    printf("Masukkan Nama File : ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';
    
    // Langsung jalankan editor dengan file kosong
    runEditor(filename, 1);   // 1 = file baru
    printf("File berhasil disimpan.\n");
}

void exitEditor() {
    printf("Terimakasih!\n");
    exit(0);
}

void handleTextEditing(int ch, char text[][256], int *cursorRow, int *cursorCol, int *rowCount) {
    
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
        for (int i = *rowCount; i > *cursorRow + 1; i--)
            strcpy(text[i], text[i-1]);
        strcpy(text[*cursorRow + 1], &text[*cursorRow][*cursorCol]);
        text[*cursorRow][*cursorCol] = '\0';
        (*rowCount)++;
        (*cursorRow)++;
        (*cursorCol) = 0;
    }

    else if (ch >= 32 && ch <= 126) { // input karakter
        int len = strlen(text[*cursorRow]);
        for (int i = len; i >= *cursorCol; i--)
            text[*cursorRow][i+1] = text[*cursorRow][i];
        text[*cursorRow][*cursorCol] = ch;
        (*cursorCol)++;
    }

    else if (ch == 83) { // delete
        if (*cursorCol < (int)strlen(text[*cursorRow])) {
            int len = strlen(text[*cursorRow]);
            for (int i = *cursorCol + 1; i <= len; i++)
                text[*cursorRow][i-1] = text[*cursorRow][i];
        }
        else if (*cursorRow < *rowCount - 1) {
            strcat(text[*cursorRow], text[*cursorRow+1]);
            for (int i = *cursorRow + 1; i < *rowCount - 1; i++)
                strcpy(text[i], text[i+1]);
            (*rowCount)--;
        }
    }
}