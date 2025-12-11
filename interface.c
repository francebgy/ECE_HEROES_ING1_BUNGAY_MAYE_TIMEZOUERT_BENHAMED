#include <stdio.h>
#include "interface.h"

void afficherMenu() {
    printf("\n");
    printf("==========================================\n");
    printf("              ECE HEROES - MENU           \n");
    printf("==========================================\n");
    printf("1. Lire les règles du jeu\n");
    printf("2. Commencer une nouvelle partie\n");
    printf("3. Reprendre une partie\n");
    printf("4. Quitter\n");
    printf("------------------------------------------\n");
    printf("Votre choix : ");
}

int lireChoixMenu() {
    int choix;
    scanf("%d", &choix);

    while (choix < 1 || choix > 4) {
        printf("Choix invalide. Entrez un nombre entre 1 et 4 : ");
        scanf("%d", &choix);
    }

    return choix;
}

void traiterChoix(int choix) {

    switch (choix) {
        case 1:
            printf("\n--- REGLES DU JEU ---\n");
            printf("But : remplir le contrat en eliminant des items par combinaison.\n");
            printf("(Contenu complet ajouté plus tard.)\n");
            break;

        case 2:
            printf("\n--- NOUVELLE PARTIE ---\n");
            printf("(Appel futur a demarrerNouvellePartie();)\n");
            break;

        case 3:
            printf("\n--- REPRENDRE PARTIE ---\n");
            printf("(Appel futur a chargerPartie();)\n");
            break;

        case 4:
            printf("\nMerci d'avoir joué !\n");
            break;

        default:
            printf("Erreur interne.\n");
    }
}