#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Prototipe fungsi
void createNewFile();
void openFile();
void editFile();
void exitEditor();
void mainMenu();

void mainMenu() {
    int choice;
    do {
        #ifdef _WIN32
        system("cls"); // Untuk Windows
        #else
        system("clear"); // Untuk Linux/Mac
        #endif
        
        printf("\033[1;34m"); // Warna biru
        printf("===========================================\n");
        printf("|            \033[1;33mTEXT EDITOR MENU\033[1;34m             |\n");
        printf("===========================================\n");
        printf("| \033[1;32m1. Create New File\033[1;34m                      |\n");
        printf("| \033[1;32m2. Open File\033[1;34m                            |\n");
        printf("| \033[1;32m3. Edit File\033[1;34m                            |\n");
        printf("| \033[1;31m4. Exit Editor\033[1;34m                          |\n");
        printf("===========================================\n");
        printf("\033[1;37mEnter your choice: \033[0m");
        scanf("%d", &choice);

        // Bersihkan buffer input setelah scanf
        while (getchar() != '\n');

        switch(choice) {
            case 1:
                createNewFile();
                break;
            case 2:
                openFile();
                break;
            case 3:
                editFile();
                break;
            case 4:
                exitEditor();
                break;
            default:
                printf("\033[1;31mInvalid choice! Please try again.\033[0m\n");
        }

        if (choice != 4) {
            printf("\033[1;33mPress Enter to continue...\033[0m");
            getchar(); // Tunggu Enter
        }
    } while(choice != 4);
}

void createNewFile() {
    char filename[100];
    FILE *file;
    char buffer[256];

    printf("\n\033[1;36m=== CREATE NEW FILE ===\033[0m\n");
    printf("Enter filename to create: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0'; // Hapus newline di akhir

    file = fopen(filename, "w");
    if (file == NULL) {
        printf("\033[1;31mError creating file!\033[0m\n");
        return;
    }

    printf("Enter your text (type a blank line to finish):\n");
    printf("\033[1;32m");
    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        // Jika baris kosong (hanya newline), berhenti
        if (strcmp(buffer, "\n") == 0) {
            break;
        }
        fputs(buffer, file);
    }
    printf("\033[0m");

    fclose(file);
    printf("\033[1;32mFile '%s' created successfully.\033[0m\n", filename);
}

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

void exitEditor() {
    printf("\n\033[1;35mExiting the editor. Goodbye!\033[0m\n");
    exit(0);
}