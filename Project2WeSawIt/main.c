#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "irfan1.h"
#include "zidan.h"
#include "rere1.h"
#include "edit_cursor.h"

int main() {
    int choice;

    do {
        system("cls");

        printf("\033[1;34m");
        printf("            \033[1;33mTEXT EDITOR MENU\033[1;34m             \n");
        printf("===========================================\n");
        printf(" \033[1;32m1. Create New File\033[1;34m                      \n");
        printf(" \033[1;32m2. Open File\033[1;34m                            \n");
        printf(" \033[1;32m3. Find and Replace\033[1;34m                     \n");
        printf(" \033[1;31m4. Exit Editor\033[1;34m                          \n");
        printf("\033[1;37mEnter your choice: \033[0m");

        scanf("%d", &choice);
        while (getchar() != '\n'); 

        switch(choice) {
            case 1:
                createNewFile();
                break;

            case 2:
                openFile(); 
                break;

            case 3:
                findAndReplace();
                break;

            case 4:
                exitEditor();
                break;

            default:
                printf("\033[1;31mInvalid choice! Please try again.\033[0m\n");
        }

        if (choice != 4) {
            printf("\033[1;33mPress Enter to continue...\033[0m");
            getchar();
        }

    } while(choice != 4);

    return 0;
}