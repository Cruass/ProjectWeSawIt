#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "edit_cursor.h"
#include "zidan.h"

void findAndReplace() {
    char filename[20];
    char find[100], replace[100];
    char buffer[1000];
    char result[10000] = "";

    printf("Masukkan nama file: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("File tidak ditemukan!\n");
        return;
    }

    printf("Kata yang ingin dicari: ");
    fgets(find, sizeof(find), stdin);
    find[strcspn(find, "\n")] = 0;

    if (strlen(find) == 0) {
        printf("Input tidak boleh kosong!\n");
        fclose(fp);
        return;
    }

    printf("Kata pengganti: ");
    fgets(replace, sizeof(replace), stdin);
    replace[strcspn(replace, "\n")] = 0;

    if (strlen(replace) > 20) {
         printf("\n[!] Peringatan: kata pengganti maksimal 20 karakter!\n");
        return;
    }

    if (strchr(replace, '\n') == NULL) {
        while (getchar() != '\n');
    }

    int found = 0;

    while (fgets(buffer, sizeof(buffer), fp)) {
        char temp[1000];
        char *pos, *start = buffer;

    while ((pos = strstr(start, find)) != NULL) {
        found++; 

        strncpy(temp, start, pos - start);
        temp[pos - start] = '\0';

        strcat(result, temp);
        strcat(result, replace);

        start = pos + strlen(find);
    }

    strcat(result, start);
}

    fclose(fp);
    if(found == 0) {
        printf("Kata tidak ditemukan dalam file!\n");
        return;
    }

    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Gagal membuka file!\n");
        return;
    }

    fputs(result, fp);
    fclose(fp);

    printf("Berhasil replace kata!\n");
}

void handleCursorMovement(int ch,Cursor *cur)
{
    // ATAS
    if (ch == 72) {

        if (cur->current->prev != NULL) {

            cur->current = cur->current->prev;

            cur->cursorRow--;

            int len = strlen(cur->current->data);

            if (cur->cursorCol > len) {

                cur->cursorCol = len;
            }
        }
    }

    // BAWAH
    else if (ch == 80) {

        if (cur->current->next != NULL) {

            cur->current = cur->current->next;

            cur->cursorRow++;

            int len = strlen(cur->current->data);

            if (cur->cursorCol > len) {

                cur->cursorCol = len;
            }
        }
    }

    // KIRI
    else if (ch == 75) {

        if (cur->cursorCol > 0) {

            cur->cursorCol--;
        }

        else if (cur->current->prev != NULL) {

            cur->current = cur->current->prev;

            cur->cursorRow--;

            cur->cursorCol = strlen(cur->current->data);
        }
    }

    // KANAN
    else if (ch == 77) {

        int len = strlen(cur->current->data);

        if (cur->cursorCol < len) {

            cur->cursorCol++;
        }

        else if (cur->current->next != NULL) {

            cur->current = cur->current->next;

            cur->cursorRow++;

            cur->cursorCol = 0;
        }
    }
}