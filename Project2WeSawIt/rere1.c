#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Prototipe fungsi (deklarasi) agar bisa dipanggil sebelum didefinisikan
void createNewFile();
void openFile();
void saveFile();
void exitEditor();

void mainMenu() {
    int choice;
    do {
        system("cls"); // Untuk Windows. Ganti dengan system("clear") jika di Linux/Mac
        printf("\033[1;34m"); // Warna biru
        printf("===========================================\n");
        printf("|            \033[1;33mTEXT EDITOR MENU\033[1;34m             |\n");
        printf("===========================================\n");
        printf("| \033[1;32m1. Create New File\033[1;34m                      |\n");
        printf("| \033[1;32m2. Open Existing File\033[1;34m                   |\n");
        printf("| \033[1;32m3. Save Current File\033[1;34m                    |\n");
        printf("| \033[1;31m4. Exit Editor\033[1;34m                          |\n");
        printf("===========================================\n");
        printf("Enter your choice: ");
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
                saveFile();
                break;
            case 4:
                exitEditor();
                break;
            default:
                printf("\033[1;31mInvalid choice! Please try again.\033[0m\n");
        }

        printf("\033[0m"); // Reset warna
        printf("Press Enter to continue...");
        getchar(); // Tunggu Enter
    } while(choice != 4);
}

void createNewFile() {
    char filename[100];
    FILE *file;
    char buffer[256];

    printf("Enter filename to create: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0'; // Hapus newline di akhir

    file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error creating file!\n");
        return;
    }

    printf("Enter your text (type a blank line to finish):\n");
    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        // Jika baris kosong (hanya newline), berhenti
        if (strcmp(buffer, "\n") == 0) {
            break;
        }
        fputs(buffer, file);
    }

    fclose(file);
    printf("File created successfully.\n");
}

void openFile() {
    char filename[100];
    FILE *file;

    printf("Enter the filename to open: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("File not found!\n");
        return;
    }

    printf("File contents:\n");
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }
    printf("\n"); // Tambah newline setelah konten file

    fclose(file);
}

void saveFile() {
    char filename[100];
    char text[1000];
    FILE *file;

    printf("Enter filename to save: ");
    scanf("%s", filename);
    file = fopen(filename, "w");

    if (file == NULL) {
        printf("Error saving file!\n");
        return;
    }

    printf("Enter your text (end with a blank line):\n");
    getchar(); // To consume the newline character from the previous input
    fgets(text, sizeof(text), stdin);

    fprintf(file, "%s", text);
    fclose(file);
    printf("File saved successfully!\n");
}

void exitEditor() {
    printf("Exiting the editor. Goodbye!\n");
    exit(0);
}

int main() {
    mainMenu();
    return 0;
}
