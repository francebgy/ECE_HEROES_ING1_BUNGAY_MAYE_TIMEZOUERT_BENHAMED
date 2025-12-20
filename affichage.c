#include "affichage.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>


void affichage_fondRose(void) {
    /* Fond magenta (rose/violet) + texte blanc */
    printf("\033[45m\033[97m");
}

void affichage_resetCouleur(void) {
    /* Reset couleurs */
    printf("\033[0m");
}
static struct termios g_old;
static bool g_raw = false;

void affichage_clear(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void affichage_message(const char *msg) {
    printf("%s\n", msg);
}

void affichage_pause(void) {
    printf("\nAppuyez sur ENTREE pour continuer...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int affichage_lire_int_menu(void) {
    affichage_disable_raw();
    int x;
    while (scanf("%d", &x) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        printf("Entrez un nombre: ");
        fflush(stdout);
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    return x;
}

void affichage_menu(void) {
    printf("=========== ECE HEROES ===========\n");
    printf("1) Lire les regles du jeu\n");
    printf("2) Nouvelle partie\n");
    printf("3) Reprendre une partie\n");
    printf("4) Quitter\n");
    printf("==================================\n");
    printf("Choix: ");
    fflush(stdout);
}

void affichage_regles(void) {
    affichage_clear();
    affichage_fondRose();
    printf("=========== REGLES DU JEU ===========\n\n");

    printf("Objectif :\n");
    printf("- Eliminer les fruits demandes par le contrat du niveau.\n\n");

    printf("Structure du jeu :\n");
    printf("- 1 partie contient 3 niveaux.\n");
    printf("- Difficulté croissante.\n\n");

    printf("Contrats (PARTIE 1) :\n");
    printf("- Niveau 1 (facile) : 12 fraises + 12 oranges | Coups: 35 | Temps: 2 min 30\n");
    printf("- Niveau 2 (interm.): 16 fraises + 14 cerises + 10 citrons | Coups: 32 | Temps: 2 min 15\n");
    printf("- Niveau 3 (plus dur): 18 fraises + 16 oranges + 14 cerises + 12 framboises + 8 citrons | Coups: 30 | Temps: 2 min 00\n\n");

    printf("Commandes (jeu en console) :\n");
    printf("- Deplacez le curseur avec les fleches : <- -> ^ v\n");
    printf("- Selection d'un fruit : ENTREE (sur le fruit)\n");
    printf("- Deplacement du fruit : apres ENTREE, utiliser les fleches vers un voisin (swap)\n");
    printf("- x : quitter (et proposer sauvegarde)\n\n");

    printf("Victoire :\n");
    printf("- Contrat rempli AVANT fin du temps et AVANT d'avoir utilise tous les coups.\n\n");

    printf("Defaite :\n");
    printf("- Temps ecoule OU coups epuises OU plus de vies.\n\n");

    printf("====================================\n");
}

void affichage_enable_raw(void) {
    if (g_raw) return;
    tcgetattr(STDIN_FILENO, &g_old);

    struct termios raw = g_old;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    g_raw = true;
}

void affichage_disable_raw(void) {
    if (!g_raw) return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_old);
    g_raw = false;
}

int affichage_lire_touche(void) {
    int c = getchar();

    if (c == '\n' || c == '\r') return KEY_ENTER;

    if (c == 27) { // ESC
        int c1 = getchar();
        if (c1 == '[') {
            int c2 = getchar();
            if (c2 == 'A') return KEY_UP;
            if (c2 == 'B') return KEY_DOWN;
            if (c2 == 'C') return KEY_RIGHT;
            if (c2 == 'D') return KEY_LEFT;
        }
        return KEY_ESC;
    }

    return c;
}

const char* affichage_symbole_item(int v) {
    switch (v) {
        case 1: return "🍓"; // fraise
        case 2: return "🍋"; // citron
        case 3: return "🍇"; // framboise (placeholder)
        case 4: return "🍒"; // cerise
        case 5: return "🍊"; // orange
        default: return "  ";
    }
}

static bool contrat_fini(const int contrat[NB_ITEMS + 1]) {
    for (int t = 1; t <= NB_ITEMS; t++) {
        if (contrat[t] > 0) return false;
    }
    return true;
}

void affichage_plateau(const Plateau *p,
                       int cx, int cy,
                       bool selection, int sx, int sy,
                       const int contrat[NB_ITEMS + 1],
                       int vies, int coups, int temps,
                       int niveau, int score) {

    printf("Fleches = bouger | ENTREE = selection/permuter | x = quitter\n");
    printf("Niveau %d | Vies: %d | Coups: %d | Temps: %ds | Score: %d\n",
           niveau, vies, coups, temps, score);

    printf("Contrat restant : ");
    for (int t = 1; t <= NB_ITEMS; t++) {
        printf("%s=%d  ", affichage_symbole_item(t), contrat[t]);
    }
    printf("\n");
    if (contrat_fini(contrat)) printf("✅ Contrat termine !\n");
    printf("\n");

    printf("    ");
    for (int j = 0; j < COLONNES; j++) printf("%2d ", j);
    printf("\n");

    for (int i = 0; i < LIGNES; i++) {
        printf("%2d  ", i);
        for (int j = 0; j < COLONNES; j++) {
            const char *sym = affichage_symbole_item(p->g[i][j]);
            bool isC = (i == cx && j == cy);
            bool isS = (selection && i == sx && j == sy);

            if (isC && isS)      printf("<%s>", sym);
            else if (isC)        printf("[%s]", sym);
            else if (isS)        printf("{%s}", sym);
            else                 printf(" %s ", sym);
        }
        printf("\n");
    }
}