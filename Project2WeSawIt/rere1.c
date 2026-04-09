#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "edit_cursor.h"   // tambahkan


void openFile() {
    char filename[100];
    printf("\n\033[1;36m=== OPEN FILE ===\033[0m\n");
    printf("Enter the filename to open: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("\033[1;31mFile '%s' not found!\033[0m\n", filename);
        return;
    }

    printf("\033[1;33mFile contents:\033[0m\n");
    printf("----------------------------------------\n");
    char ch;
    while ((ch = fgetc(file)) != EOF) putchar(ch);
    printf("\n----------------------------------------\n");
    fclose(file);

    char choice;
    printf("\nApakah kamu ingin mengedit file ini dengan editor kursor? (y/n): ");
    scanf(" %c", &choice);
    getchar(); 

    if (choice == 'y' || choice == 'Y') {
        runEditor(filename, 0);   // 0 = file sudah ada, muat isinya
        printf("\033[1;32mFile '%s' telah disimpan.\033[0m\n", filename);
    } else {
        printf("File tidak diedit.\n");
    }
}