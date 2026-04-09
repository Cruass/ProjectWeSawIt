#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "irfan1.h"  
void createNewFile() {
    char filename[100];
    FILE *file;
    char lines[100][256];  
    int lineCount = 0;     

    printf("Masukkan Nama File : ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0'; 

    file = fopen(filename, "w");
    if (file == NULL) {
        printf("File Error!\n");
        return;
    }

    printf("Masukkan teks :\n");
    while (lineCount < 100) { 
        fgets(lines[lineCount], sizeof(lines[lineCount]), stdin);
        if (strcmp(lines[lineCount], "\n") == 0) { 
            break;
        }
        lineCount++;
    }

    for (int i = 0; i < lineCount; i++) {
        fputs(lines[i], file);
    }

    fclose(file);
    printf("File berhasil dibuat.\n");
}

void exitEditor() {
    printf("Terimakasih!\n");
    exit(0);
}