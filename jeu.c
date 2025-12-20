#include "jeu.h"
#include "sauvegarde.h"
#include "affichage.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

// ===================== PARAMETRES JEU =====================
#define LIGNES 10
#define COLONNES 14
#define NB_ITEMS 5

#define VIES_DEPART 6
#define NB_NIVEAUX 3

// ===================== NIVEAUX / CONTRATS =====================
// Mapping items:
// 1: 🍓 Fraise
// 2: 🍋 Citron
// 3: 🫐 Framboise
// 4: 🍒 Cerise
// 5: 🍊 Orange

typedef struct {
    int numero;
    int coupsMax;
    int tempsSecondes;
    int contrat[NB_ITEMS + 1]; // index 1..5
} Niveau;

static Niveau g_niveaux[NB_NIVEAUX] = {
        // Niveau 1 (facile) : 12 fraises + 12 oranges
        {1, 35, 150, {0, 12, 0, 0, 0, 12}}, // 2min30

        // Niveau 2 : 16 fraises + 14 cerises + 10 citrons
        {2, 32, 135, {0, 16, 10, 0, 14, 0}}, // 2min15

        // Niveau 3 : 18 fraises + 16 oranges + 14 cerises + 12 framboises + 8 citrons
        {3, 30, 120, {0, 18, 8, 12, 14, 16}} // 2min00
};

// ===================== AFFICHAGE FRUITS =====================
static const char* fruitSymbole(int v) {
    switch (v) {
        case 1: return "🍓";
        case 2: return "🍋";
        case 3: return "🫐";
        case 4: return "🍒";
        case 5: return "🍊";
        default:return "  ";
    }
}

static void clearScreen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}
static void fondSelonNiveau(int niveau) {
    // Texte blanc
    // Niveau 1 = violet, Niveau 2 = rose, Niveau 3 = vert
    switch (niveau) {
        case 1:  // violet (magenta)
            printf("\033[45m\033[97m");
            break;
        case 2:  // rose (magenta clair)
            printf("\033[105m\033[97m");
            break;
        case 3:  // vert
            printf("\033[42m\033[97m");
            break;
        default: // fallback
            printf("\033[0m");
            break;
    }
}

static void resetCouleurs(void) {
    // Reset couleurs terminal
    printf("\033[0m");
    fflush(stdout);
}

static void viderLigne(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

static int lireInt(void) {
    int x;
    while (scanf("%d", &x) != 1) {
        viderLigne();
        printf("Entre un nombre valide : ");
    }
    viderLigne();
    return x;
}

static void attendreEntree(void) {
    printf("\nAppuie sur ENTREE...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// ===================== CLAVIER RAW (FLECHES) =====================
static struct termios g_old;
static bool g_raw_enabled = false;

static void enableRawMode(void) {
    if (g_raw_enabled) return;
    tcgetattr(STDIN_FILENO, &g_old);

    struct termios raw = g_old;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    g_raw_enabled = true;
}

static void disableRawMode(void) {
    if (!g_raw_enabled) return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_old);
    g_raw_enabled = false;
}



static int readKey(void) {
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

// ===================== PLATEAU / MATCH =====================
static int randItem(void) {
    return 1 + (rand() % NB_ITEMS);
}

static void initialiserPlateauSansMatch(int p[LIGNES][COLONNES]) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            int v;
            do {
                v = randItem();
                // évite 3 alignés init horizontal
                if (j >= 2 && p[i][j-1] == v && p[i][j-2] == v) continue;
                // évite 3 alignés init vertical
                if (i >= 2 && p[i-1][j] == v && p[i-2][j] == v) continue;
                break;
            } while (1);
            p[i][j] = v;
        }
    }
}

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
    //
    printf("     ");
    for (int j = 0; j < COLONNES; j++) printf("%2d  ", j);
    printf("\n");

    for (int i = 0; i < LIGNES; i++) {
        printf("%2d   ", i);
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

static bool sontAdjacents(int x1, int y1, int x2, int y2) {
    int dx = x1 - x2; if (dx < 0) dx = -dx;
    int dy = y1 - y2; if (dy < 0) dy = -dy;
    return (dx + dy) == 1;
}

static void mark_clear(bool mark[LIGNES][COLONNES]) {
    for (int i = 0; i < LIGNES; i++)
        for (int j = 0; j < COLONNES; j++)
            mark[i][j] = false;
}

static bool detect_square_4x4(const int a[LIGNES][COLONNES], bool mark[LIGNES][COLONNES]) {
    bool found = false;
    for (int i = 0; i <= LIGNES - 4; i++) {
        for (int j = 0; j <= COLONNES - 4; j++) {
            int v = a[i][j];
            if (v == 0) continue;
            bool ok = true;
            for (int di = 0; di < 4 && ok; di++) {
                for (int dj = 0; dj < 4; dj++) {
                    if (a[i+di][j+dj] != v) { ok = false; break; }
                }
            }
            if (ok) {
                found = true;
                for (int di = 0; di < 4; di++)
                    for (int dj = 0; dj < 4; dj++)
                        mark[i+di][j+dj] = true;
            }
        }
    }
    return found;
}

static bool detect_cross_9(const int a[LIGNES][COLONNES], bool mark[LIGNES][COLONNES]) {
    bool found = false;
    for (int i = 2; i <= LIGNES - 3; i++) {
        for (int j = 2; j <= COLONNES - 3; j++) {
            int v = a[i][j];
            if (v == 0) continue;
            if (a[i-1][j]==v && a[i-2][j]==v &&
                a[i+1][j]==v && a[i+2][j]==v &&
                a[i][j-1]==v && a[i][j-2]==v &&
                a[i][j+1]==v && a[i][j+2]==v) {

                found = true;

                for (int col = 0; col < COLONNES; col++) {
                    if (a[i][col] == v) mark[i][col] = true;
                }
                for (int row = 0; row < LIGNES; row++) {
                    if (a[row][j] == v) mark[row][j] = true;
                }
            }
        }
    }
    return found;
}

// Ligne de 6+ : supprime tous les items de type v sur TOUT le tableau
static bool detect_line_6_global(const int a[LIGNES][COLONNES], bool mark[LIGNES][COLONNES]) {
    bool found = false;

    for (int i = 0; i < LIGNES; i++) {
        int j = 0;
        while (j < COLONNES) {
            int v = a[i][j];
            if (v == 0) { j++; continue; }
            int k = j;
            while (k < COLONNES && a[i][k] == v) k++;
            int len = k - j;
            if (len >= 6) {
                found = true;
                for (int r = 0; r < LIGNES; r++)
                    for (int c = 0; c < COLONNES; c++)
                        if (a[r][c] == v) mark[r][c] = true;
            }
            j = k;
        }
    }

    for (int j = 0; j < COLONNES; j++) {
        int i = 0;
        while (i < LIGNES) {
            int v = a[i][j];
            if (v == 0) { i++; continue; }
            int k = i;
            while (k < LIGNES && a[k][j] == v) k++;
            int len = k - i;
            if (len >= 6) {
                found = true;
                for (int r = 0; r < LIGNES; r++)
                    for (int c = 0; c < COLONNES; c++)
                        if (a[r][c] == v) mark[r][c] = true;
            }
            i = k;
        }
    }

    return found;
}

static bool detect_match_3(const int a[LIGNES][COLONNES], bool mark[LIGNES][COLONNES]) {
    bool found = false;

    for (int i = 0; i < LIGNES; i++) {
        int j = 0;
        while (j < COLONNES) {
            int v = a[i][j];
            if (v == 0) { j++; continue; }
            int k = j;
            while (k < COLONNES && a[i][k] == v) k++;
            int len = k - j;
            if (len >= 3) {
                found = true;
                for (int c = j; c < k; c++) mark[i][c] = true;
            }
            j = k;
        }
    }

    for (int j = 0; j < COLONNES; j++) {
        int i = 0;
        while (i < LIGNES) {
            int v = a[i][j];
            if (v == 0) { i++; continue; }
            int k = i;
            while (k < LIGNES && a[k][j] == v) k++;
            int len = k - i;
            if (len >= 3) {
                found = true;
                for (int r = i; r < k; r++) mark[r][j] = true;
            }
            i = k;
        }
    }

    return found;
}

static bool detect_any_figure(const int a[LIGNES][COLONNES]) {
    bool mark[LIGNES][COLONNES];
    mark_clear(mark);

    bool ok = false;
    ok |= detect_square_4x4(a, mark);
    ok |= detect_cross_9(a, mark);
    ok |= detect_line_6_global(a, mark);
    ok |= detect_match_3(a, mark);

    return ok;
}

static void appliquer_gravite_et_remplir(int a[LIGNES][COLONNES]) {
    for (int j = 0; j < COLONNES; j++) {
        int write = LIGNES - 1;
        for (int i = LIGNES - 1; i >= 0; i--) {
            if (a[i][j] != 0) {
                a[write][j] = a[i][j];
                if (write != i) a[i][j] = 0;
                write--;
            }
        }
        while (write >= 0) {
            a[write][j] = randItem();
            write--;
        }
    }
}

static bool resoudre_et_stabiliser(int a[LIGNES][COLONNES], int contrat[NB_ITEMS + 1], int *score) {
    bool auMoinsUneSuppression = false;

    while (1) {
        bool mark[LIGNES][COLONNES];
        mark_clear(mark);

        bool found = false;
        found |= detect_square_4x4(a, mark);
        found |= detect_cross_9(a, mark);
        found |= detect_line_6_global(a, mark);
        found |= detect_match_3(a, mark);

        if (!found) break;

        auMoinsUneSuppression = true;

        for (int i = 0; i < LIGNES; i++) {
            for (int j = 0; j < COLONNES; j++) {
                if (mark[i][j] && a[i][j] != 0) {
                    int v = a[i][j];
                    if (v >= 1 && v <= NB_ITEMS) {
                        if (contrat[v] > 0) contrat[v]--;
                        if (score) (*score) += 1;
                    }
                    a[i][j] = 0;
                }
            }
        }

        appliquer_gravite_et_remplir(a);
    }

    return auMoinsUneSuppression;
}

static bool permuterEtResoudreSiValide(int a[LIGNES][COLONNES],
                                       int x1, int y1, int x2, int y2,
                                       int contrat[NB_ITEMS + 1], int *score) {
    int tmp = a[x1][y1];
    a[x1][y1] = a[x2][y2];
    a[x2][y2] = tmp;

    if (!detect_any_figure(a)) {
        tmp = a[x1][y1];
        a[x1][y1] = a[x2][y2];
        a[x2][y2] = tmp;
        return false;
    }

    resoudre_et_stabiliser(a, contrat, score);
    return true;
}

// ===================== REGLES (AFFICHAGE) =====================
void jeu_afficherRegles(void) {
    resetCouleurs();
    clearScreen();
    printf("=========== REGLES DU JEU ===========\n\n");

    printf("🎯 Objectif : eliminer les fruits demandes dans le contrat du niveau.\n\