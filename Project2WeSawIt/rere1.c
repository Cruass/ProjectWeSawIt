#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Prototipe fungsi
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
}

void editFile() {
    char filename[100];
    char text[1000];
    FILE *file;

    printf("\n\033[1;36m=== EDIT FILE ===\033[0m\n");
    printf("Enter filename to edit: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    file = fopen(filename, "w");
    if (file == NULL) {
        printf("\033[1;31mError saving file!\033[0m\n");
        return;
    }

    printf("Enter your text (end with a blank line):\n");
    printf("\033[1;32m");
    while (1) {
        fgets(text, sizeof(text), stdin);
        if (strcmp(text, "\n") == 0) {
            break;
        }
        fputs(text, file);
    }
    printf("\033[0m");

    fclose(file);
    printf("\033[1;32mFile '%s' saved successfully!\033[0m\n", filename);
}