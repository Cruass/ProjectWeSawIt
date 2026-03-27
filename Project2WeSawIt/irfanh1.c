#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "irfan1.h"   // include header buatan sendiri

void createNewFile() {
    char filename[100];
    FILE *file;
    char lines[100][256];  // Array 2D: maksimal 100 baris, masing-masing 256 karakter
    int lineCount = 0;      // Penghitung jumlah baris yang diinput

    printf("Masukkan Nama File : ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0'; // Hapus newline

    file = fopen(filename, "w");
    if (file == NULL) {
        printf("File Error!\n");
        return;
    }

    printf("Masukkan teks :\n");
    while (lineCount < 100) { // Batasi maksimal 100 baris
        fgets(lines[lineCount], sizeof(lines[lineCount]), stdin);
        if (strcmp(lines[lineCount], "\n") == 0) { // Baris kosong berhenti
            break;
        }
        lineCount++;
    }

    // Tulis semua baris yang tersimpan ke file
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

void mainMenu() {
    int choice;
    do {
        // Hapus layar (gunakan clear jika di Linux/Mac)
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        printf("\033[1;34m");
        printf("===========================================\n");
        printf("|            \033[1;33mTEXT EDITOR MENU\033[1;34m             |\n");
        printf("===========================================\n");
        printf("| \033[1;32m1. Create New File\033[1;34m                      |\n");
        printf("| \033[1;32m2. exit\033[1;34m                                 |\n");
        printf("===========================================\n");
        printf("Masukkan Pilihan : ");
        scanf("%d", &choice);

        while (getchar() != '\n'); // bersihkan buffer

        switch(choice) {
            case 1: createNewFile(); break;
            case 2: exitEditor(); break;
            default:
                printf("\033[1;31mInvalid.\033[0m\n");
        }

      
        getchar();
    } while(choice != 4);
}