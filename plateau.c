#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "plateau.h"

int estDansPlateau(int valeur) {
    return valeur >= 0 && valeur < NB_ITEMS;
}

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
                case -1:
                printf("\033[37mX \033[0m"); // blanc/gris pour combinaisons
                    break;
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

void detecterCombinaisons(int plateau[LIGNES][COLONNES]) {

    // Détection horizontale
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES - 2; j++) {

            int val = plateau[i][j];

            if (estDansPlateau(val) &&
                plateau[i][j + 1] == val &&
                plateau[i][j + 2] == val) {

                plateau[i][j]     = -1;
                plateau[i][j + 1] = -1;
                plateau[i][j + 2] = -1;
            }
        }
    }

    // Détection verticale
    for (int j = 0; j < COLONNES; j++) {
        for (int i = 0; i < LIGNES - 2; i++) {

            int val = plateau[i][j];

            if (estDansPlateau(val) &&
                plateau[i + 1][j] == val &&
                plateau[i + 2][j] == val) {

                plateau[i][j]     = -1;
                plateau[i + 1][j] = -1;
                plateau[i + 2][j] = -1;
            }
        }
    }
}