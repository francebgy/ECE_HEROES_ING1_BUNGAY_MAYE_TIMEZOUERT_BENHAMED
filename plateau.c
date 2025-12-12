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
            printf("%d ", plateau[i][j]);
        }
        printf("\n");
    }
}