#include <stdio.h>
#include "interface.h"

int main() {

    int choix = 0;

    do {
        afficherMenu();
        choix = lireChoixMenu();
        traiterChoix(choix);

    } while (choix != 4);

    return 0;
}