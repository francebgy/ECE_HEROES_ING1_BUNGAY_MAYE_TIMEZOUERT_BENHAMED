#include "interface.h"
#include "plateau.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define FICHIER_SAUVEGARDE "sauvegardes.txt"
#define VIES_DEPART 3
#define NB_NIVEAUX  3

// --------- AFFICHAGE FRUITS ----------
static const char* fruitSymbole(int v) {
    switch (v) {
        case 1: return "🍓";
        case 2: return "🍋";
        case 3: return "🍇";
        case 4: return "🍒";
        case 5: return "🍊";
        default:return "  ";
    }
}

static void clearScreen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

// --------- LECTURE CLAVIER (FLECHES + ENTREE) EN MODE RAW ----------
static struct termios g_old;
static bool g_raw_enabled = false;

static void enableRawMode(void) {
    if (g_raw_enabled) return;
    tcgetattr(STDIN_FILENO, &g_old);

    struct termios raw = g_old;
    raw.c_lflag &= ~(ICANON | ECHO);  // pas besoin d'Entrée, pas d'écho
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    g_raw_enabled = true;
}

static void disableRawMode(void) {
    if (!g_raw_enabled) return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_old);
    g_raw_enabled = false;
}

enum {
    KEY_NONE = 0,
    KEY_UP = 1001,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER,
    KEY_ESC
};

static int readKey(void) {
    int c = getchar();

    // Enter peut être '\n' ou '\r'
    if (c == '\n' || c == '\r') return KEY_ENTER;

    // Flèches : ESC [ A/B/C/D
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

    return c; // ex: 'x', ' ', etc.
}

// --------- ENTREES "LENTES" POUR MENU (scanf) ----------

static int lireInt(void) {
    int x;
    while (scanf("%d", &x) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        printf("Entre un nombre valide : ");
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    return x;
}

static void attendreEntree(void) {
    printf("\nAppuie sur ENTREE...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// --------- CONTRAT / AFFICHAGE ----------
static bool contratFini(const int contrat[NB_ITEMS + 1]) {
    for (int t = 1; t <= NB_ITEMS; t++) {
        if (contrat[t] > 0) return false;
    }
    return true;
}

static void afficherContrat(const int contrat[NB_ITEMS + 1]) {
    printf("Contrat restant : ");
    for (int t = 1; t <= NB_ITEMS; t++) {
        printf("%s=%d  ", fruitSymbole(t), contrat[t]);
    }
    printf("\n");
}

static void afficherPlateauIHM(const int plateau[LIGNES][COLONNES],
                               int cx, int cy,
                               bool selectionActive, int sx, int sy) {
    printf("    ");
    for (int j = 0; j < COLONNES; j++) printf("%2d ", j);
    printf("\n");

    for (int i = 0; i < LIGNES; i++) {
        printf("%2d  ", i);
        for (int j = 0; j < COLONNES; j++) {
            const char* sym = fruitSymbole(plateau[i][j]);

            bool isCursor = (i == cx && j == cy);
            bool isSel    = (selectionActive && i == sx && j == sy);

            if (isCursor && isSel)      printf("<%s>", sym);
            else if (isCursor)          printf("[%s]", sym);
            else if (isSel)             printf("{%s}", sym);
            else                        printf(" %s ", sym);
        }
        printf("\n");
    }
}

// --------- REGLES ----------
static void afficherRegles(void) {
    clearScreen();
    printf("=== REGLES (resume) ===\n");
    printf("- Deplace le curseur avec les FLECHES.\n");
    printf("- ENTREE (ou ESPACE) = selection / permutation.\n");
    printf("- Objectif : faire disparaitre les fruits du contrat.\n");
    printf("- Le swap n'est accepte que s'il cree une combinaison.\n");
    printf("- Chaque swap accepte = 1 coup.\n");
    printf("- Le temps est limite.\n");
    attendreEntree();
}

// --------- NIVEAUX ----------
static Niveau niveaux[NB_NIVEAUX] = {
        // numero, coups, temps(sec), contrat[0..5]
        {1, 35, 180, {0, 10, 10,  8,  0,  0}}, // 3 minutes
        {2, 28, 150, {0, 14, 12, 10,  6,  0}},
        {3, 22, 120, {0, 18, 16, 14, 10,  8}}
};

// --------- SAUVEGARDE ----------
static bool pseudoExiste(const char* pseudo) {
    FILE* f = fopen(FICHIER_SAUVEGARDE, "r");
    if (!f) return false;

    char p[64];
    int niv, vies;
    while (fscanf(f, "%63s %d %d", p, &niv, &vies) == 3) {
        if (strcmp(p, pseudo) == 0) {
            fclose(f);
            return true;
        }
    }
    fclose(f);
    return false;
}

static void sauvegarderProgression(int niveauAtteint, int vies) {
    disableRawMode();

    char pseudo[64];
    printf("Pseudo (nouveau, non utilise) : ");
    scanf("%63s", pseudo);
    int c; while ((c = getchar()) != '\n' && c != EOF) {}

    if (pseudoExiste(pseudo)) {
        printf("Pseudo deja utilise. Sauvegarde annulee.\n");
        attendreEntree();
        return;
    }

    FILE* f = fopen(FICHIER_SAUVEGARDE, "a");
    if (!f) {
        perror("Erreur ouverture sauvegarde");
        attendreEntree();
        return;
    }
    fprintf(f, "%s %d %d\n", pseudo, niveauAtteint, vies);
    fclose(f);

    printf("Sauvegarde OK (%s, niveau %d, vies %d).\n", pseudo, niveauAtteint, vies);
    attendreEntree();
}

static bool chargerProgression(int* niveauAtteint, int* vies) {
    disableRawMode();

    char pseudo[64];
    printf("Pseudo : ");
    scanf("%63s", pseudo);
    int c; while ((c = getchar()) != '\n' && c != EOF) {}

    FILE* f = fopen(FICHIER_SAUVEGARDE, "r");
    if (!f) {
        printf("Aucune sauvegarde.\n");
        attendreEntree();
        return false;
    }

    char p[64];
    int niv, v;
    while (fscanf(f, "%63s %d %d", p, &niv, &v) == 3) {
        if (strcmp(p, pseudo) == 0) {
            fclose(f);
            *niveauAtteint = niv;
            *vies = v;
            return true;
        }
    }
    fclose(f);

    printf("Pseudo introuvable.\n");
    attendreEntree();
    return false;
}

// --------- JEU : UN NIVEAU ----------
static void jouerUnNiveau(int* vies, int niveauIndex, int* niveauGagneDernier) {
    Niveau niv = niveaux[niveauIndex];

    int plateau[LIGNES][COLONNES];
    initialiserPlateauSansMatch(plateau);

    int contrat[NB_ITEMS + 1];
    for (int t = 0; t <= NB_ITEMS; t++) contrat[t] = niv.contrat[t];

    int coupsRestants = niv.coupsMax;
    time_t start = time(NULL);

    int cx = 0, cy = 0;
    bool selectionActive = false;
    int sx = -1, sy = -1;

    enableRawMode();

    while (1) {
        int elapsed = (int)(time(NULL) - start);
        int tempsRestant = niv.tempsSecondes - elapsed;
        if (tempsRestant < 0) tempsRestant = 0;

        clearScreen();
        printf("Niveau %d | Vies: %d | Coups: %d | Temps: %ds\n",
               niv.numero, *vies, coupsRestants, tempsRestant);
        afficherContrat(contrat);
        printf("\n");
        afficherPlateauIHM(plateau, cx, cy, selectionActive, sx, sy);

        printf("\nFleches = bouger | ENTREE ou ESPACE = selection/permuter | x = quitter\n");

        if (tempsRestant <= 0 || coupsRestants <= 0) {
            (*vies)--;
            disableRawMode();
            clearScreen();
            printf("ECHEC du niveau %d.\n", niv.numero);
            printf("Il te reste %d vies.\n", *vies);
            attendreEntree();
            return;
        }

        if (contratFini(contrat)) {
            *niveauGagneDernier = niv.numero;
            disableRawMode();
            clearScreen();
            printf("VICTOIRE du niveau %d !\n", niv.numero);
            attendreEntree();
            return;
        }

        int k = readKey();
        if (k == KEY_NONE) continue;

        if (k == 'x' || k == 'X') {
            disableRawMode();
            clearScreen();
            printf("Quitter le niveau. Sauvegarder ?\n1) Oui\n2) Non\nChoix: ");
            int choix = lireInt();
            if (choix == 1) sauvegarderProgression(*niveauGagneDernier, *vies);
            return;
        }

        if (k == KEY_UP && cx > 0) cx--;
        else if (k == KEY_DOWN && cx < LIGNES - 1) cx++;
        else if (k == KEY_LEFT && cy > 0) cy--;
        else if (k == KEY_RIGHT && cy < COLONNES - 1) cy++;

        else if (k == KEY_ENTER || k == ' ') {
            if (!selectionActive) {
                selectionActive = true;
                sx = cx; sy = cy;
            } else {
                if (sx == cx && sy == cy) {
                    selectionActive = false;
                    sx = sy = -1;
                } else {
                    if (sontAdjacents(sx, sy, cx, cy)) {
                        bool ok = permuterEtResoudreSiValide(plateau, sx, sy, cx, cy, contrat);
                        if (ok) coupsRestants--;
                        selectionActive = false;
                        sx = sy = -1;
                    } else {
                        sx = cx; sy = cy;
                    }
                }
            }
        }
    }
}

// --------- PARTIES ----------
static void nouvellePartie(void) {
    int vies = VIES_DEPART;
    int dernierNiveauGagne = 0;

    for (int i = 0; i < NB_NIVEAUX; i++) {
        if (vies <= 0) break;
        jouerUnNiveau(&vies, i, &dernierNiveauGagne);
    }

    disableRawMode();
    clearScreen();

    if (vies <= 0) printf("FIN DE PARTIE : plus de vies.\n");
    else printf("FIN DE PARTIE : niveaux termines.\n");

    printf("Sauvegarder ?\n1) Oui\n2) Non\nChoix: ");
    int choix = lireInt();
    if (choix == 1) sauvegarderProgression(dernierNiveauGagne, vies);
}

static void reprendrePartie(void) {
    int niveauAtteint = 0, vies = VIES_DEPART;
    if (!chargerProgression(&niveauAtteint, &vies)) return;

    int dernierNiveauGagne = niveauAtteint;

    int startIndex = niveauAtteint;
    if (startIndex < 0) startIndex = 0;
    if (startIndex >= NB_NIVEAUX) startIndex = NB_NIVEAUX - 1;

    for (int i = startIndex; i < NB_NIVEAUX; i++) {
        if (vies <= 0) break;
        jouerUnNiveau(&vies, i, &dernierNiveauGagne);
    }

    disableRawMode();
    clearScreen();

    if (vies <= 0) printf("FIN DE PARTIE : plus de vies.\n");
    else printf("FIN DE PARTIE : niveaux termines.\n");

    printf("Sauvegarder ?\n1) Oui\n2) Non\nChoix: ");
    int choix = lireInt();
    if (choix == 1) sauvegarderProgression(dernierNiveauGagne, vies);
}

// --------- MENU ----------
void boucleMenu(void) {
    disableRawMode();

    while (1) {
        clearScreen();
        printf("=========== ECE HEROES ===========\n");
        printf("1. Lire les regles du jeu\n");
        printf("2. Commencer une nouvelle partie\n");
        printf("3. Reprendre une partie\n");
        printf("4. Quitter\n");
        printf("==================================\n");
        printf("Choix: ");

        int choix = lireInt();

        if (choix == 1) afficherRegles();
        else if (choix == 2) nouvellePartie();
        else if (choix == 3) reprendrePartie();
        else if (choix == 4) {
            disableRawMode();
            return;
        }
    }
}