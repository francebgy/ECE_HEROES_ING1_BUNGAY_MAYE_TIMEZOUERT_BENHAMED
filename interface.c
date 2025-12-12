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
            afficher_regles();
            break;

        case 2:
            demarrerNouvellePartie();
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

void afficher_regles() {
    printf("\n========================================\n");
    printf("              REGLES DU JEU\n");
    printf("========================================\n\n");

    printf("ECE Heroes est un jeu de reflexion dans lequel le joueur doit\n");
    printf("trouver des combinaisons de blocs afin d'obtenir le score le plus eleve.\n\n");

    printf("1. Objectif du jeu\n");
    printf("- Aligner des formes similaires pour marquer des points.\n");
    printf("- Plus la combinaison est grande, plus le score gagne augmente.\n\n");

    printf("2. Systeme de niveaux\n");
    printf("- Chaque niveau augmente la difficulte et la vitesse du jeu.\n");
    printf("- Les grilles deviennent plus complexes.\n");
    printf("- Le joueur doit s'adapter rapidement.\n\n");

    printf("3. Vies du joueur\n");
    printf("- Le joueur commence avec 3 vies.\n");
    printf("- Une erreur fait perdre une vie.\n");
    printf("- Quand toutes les vies sont perdues, la partie est terminee.\n\n");

    printf("4. Commandes\n");
    printf("- Utilisez les touches pour vous deplacer dans les menus.\n");
    printf("- Appuyez sur Entree pour valider un choix.\n\n");

    printf("Appuyez sur Entree pour revenir au menu...");
    getchar(); getchar(); // pause pour attendre que l'utilisateur appuie sur Entrée
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void demarrerNouvellePartie() {
    int vies = 3;
    int niveau = 1;

    printf("\n==============================\n");
    printf("   NOUVELLE PARTIE DEMARREE\n");
    printf("==============================\n");
    printf("Niveau actuel : %d\n", niveau);
    printf("Vies restantes : %d\n", vies);
    printf("\n(Bientot : affichage du plateau, contrat, coups...)\n");
}