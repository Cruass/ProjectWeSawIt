#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void openFile() {
    char filename[100];
    FILE *file;

    printf("\n\033[1;36m=== OPEN FILE ===\033[0m\n");
    printf("Enter the filename to open: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("\033[1;31mFile '%s' not found!\033[0m\n", filename);
        return;
    }

    printf("\033[1;33mFile contents:\033[0m\n");
    printf("----------------------------------------\n");
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }
    printf("\n----------------------------------------\n");

    fclose(file);

    char choice;
    printf("\nApakah kamu ingin mengedit file ini? (y/n): ");
    scanf(" %c", &choice);
    getchar(); 

    if (choice == 'y' || choice == 'Y') {

        file = fopen(filename, "r");
        printf("\n\033[1;33mCurrent content:\033[0m\n");
        printf("----------------------------------------\n");

        while ((ch = fgetc(file)) != EOF) {
            putchar(ch);
        }

        fclose(file);
        printf("\n----------------------------------------\n");

        file = fopen(filename, "a");
        if (file == NULL) {
            printf("\033[1;31mError opening file!\033[0m\n");
            return;
        }

        char text[1000];
        printf("Add new content (end with blank line):\n");
        printf("\033[1;32m");

        fputs("\n", file);

        while (1) {
            fgets(text, sizeof(text), stdin);
            if (strcmp(text, "\n") == 0) {
                break;
            }
            fputs(text, file);
        }

        printf("\033[0m");
        fclose(file);

        printf("\033[1;32mFile '%s' updated successfully!\033[0m\n", filename);
    }
}