#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zidan.h"

void findAndReplace(char filename[]) {
    char find[100], replace[100];
    char buffer[1000];
    char result[5000] = "";

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("File tidak ditemukan!\n");
        return;
    }

    printf("Kata yang ingin dicari: ");
    scanf("%s", find);

    printf("Kata pengganti: ");
    scanf("%s", replace);

    while (fgets(buffer, sizeof(buffer), fp)) {
        char temp[1000];
        char *pos, *start = buffer;

        while ((pos = strstr(start, find)) != NULL) {
            strncpy(temp, start, pos - start);
            temp[pos - start] = '\0';

            strcat(result, temp);
            strcat(result, replace);

            start = pos + strlen(find);
        }

        strcat(result, start);
    }

    fclose(fp);

    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Gagal membuka file!\n");
        return;
    }

    fputs(result, fp);
    fclose(fp);

    printf("Berhasil replace kata!\n");
}