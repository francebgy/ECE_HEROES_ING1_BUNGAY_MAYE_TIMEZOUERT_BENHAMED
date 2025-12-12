#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "plateau.h"

void initialiserPlateau(int plateau[LIGNES][COLONNES]) {
    srand(time(NULL));

    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            plateau[i][j] = rand() % NB_ITEMS;
        }
    }
}

void afficherPlateau(int plateau[LIGNES][COLONNES]) {

    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {

            switch (plateau[i][j]) {
                case 0:
                    printf("\033[31m■ \033[0m"); // rouge
                    break;
                case 1:
                    printf("\033[32m■ \033[0m"); // vert
                    break;
                case 2:
                    printf("\033[34m■ \033[0m"); // bleu
                    break;
                case 3:
                    printf("\033[33m■ \033[0m"); // jaune
                    break;
                case 4:
                    printf("\033[35m■ \033[0m"); // violet
                    break;
                default:
                    printf("  ");
            }
        }
        printf("\n");
    }
}