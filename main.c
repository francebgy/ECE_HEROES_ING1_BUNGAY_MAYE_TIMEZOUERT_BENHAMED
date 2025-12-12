#include <stdio.h>
#include "interface.h"

int main() {

    int choix = 0;

    while (choix != 4) {
        clearScreen();
        afficherMenu();
        choix = lireChoixMenu();
        traiterChoix(choix);

        if (choix != 4) {
            printf("\nAppuyez sur Entrée pour revenir au menu...");
            getchar();
            getchar();
        }
    }

    return 0;
}