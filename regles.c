#include "regles.h"
#include <stdlib.h>

static int rand_item(void) {
    return 1 + (rand() % NB_ITEMS);
}

bool regles_existe_figure(const Plateau *p) {
    // horizontal
    for (int i = 0; i < LIGNES; i++) {
        int run = 1;
        for (int j = 1; j < COLONNES; j++) {
            if (p->g[i][j] != 0 && p->g[i][j] == p->g[i][j-1]) run++;
            else run = 1;
            if (run >= 3) return true;
        }
    }
    // vertical
    for (int j = 0; j < COLONNES; j++) {
        int run = 1;
        for (int i = 1; i < LIGNES; i++) {
            if (p->g[i][j] != 0 && p->g[i][j] == p->g[i-1][j]) run++;
            else run = 1;
            if (run >= 3) return true;
        }
    }
    return false;
}

bool regles_case_dans_figure(const Plateau *p, int x, int y) {
    int v = p->g[x][y];
    if (v == 0) return false;

    // check horizontal around (x,y)
    int left = y;
    while (left > 0 && p->g[x][left-1] == v) left--;
    int right = y;
    while (right < COLONNES-1 && p->g[x][right+1] == v) right++;
    if (right - left + 1 >= 3) return true;

    // check vertical
    int up = x;
    while (up > 0 && p->g[up-1][y] == v) up--;
    int down = x;
    while (down < LIGNES-1 && p->g[down+1][y] == v) down++;
    if (down - up + 1 >= 3) return true;

    return false;
}

static void marquer_suppressions(const Plateau *p, bool sup[LIGNES][COLONNES]) {
    for (int i = 0; i < LIGNES; i++)
        for (int j = 0; j < COLONNES; j++)
            sup[i][j] = false;

    // horizontal
    for (int i = 0; i < LIGNES; i++) {
        int j = 0;
        while (j < COLONNES) {
            int v = p->g[i][j];
            if (v == 0) { j++; continue; }
            int k = j;
            while (k < COLONNES && p->g[i][k] == v) k++;
            int len = k - j;
            if (len >= 3) {
                for (int t = j; t < k; t++) sup[i][t] = true;
            }
            j = k;
        }
    }

    // vertical
    for (int j = 0; j < COLONNES; j++) {
        int i = 0;
        while (i < LIGNES) {
            int v = p->g[i][j];
            if (v == 0) { i++; continue; }
            int k = i;
            while (k < LIGNES && p->g[k][j] == v) k++;
            int len = k - i;
            if (len >= 3) {
                for (int t = i; t < k; t++) sup[t][j] = true;
            }
            i = k;
        }
    }
}

static bool appliquer_suppressions(Plateau *p,
                                   bool sup[LIGNES][COLONNES],
                                   int contrat[NB_ITEMS + 1],
                                   int *score) {
    bool a_supprime = false;

    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            if (sup[i][j] && p->g[i][j] != 0) {
                int item = p->g[i][j];
                p->g[i][j] = 0;
                a_supprime = true;

                if (item >= 1 && item <= NB_ITEMS && contrat[item] > 0) {
                    contrat[item]--;
                }
                if (score) (*score) += 10;
            }
        }
    }
    return a_supprime;
}

static void gravite(Plateau *p) {
    for (int j = 0; j < COLONNES; j++) {
        int write = LIGNES - 1;
        for (int i = LIGNES - 1; i >= 0; i--) {
            if (p->g[i][j] != 0) {
                p->g[write][j] = p->g[i][j];
                if (write != i) p->g[i][j] = 0;
                write--;
            }
        }
        // remplir le haut en 0 pour l'instant
        for (int i = write; i >= 0; i--) p->g[i][j] = 0;
    }
}

static void remplir(Plateau *p) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            if (p->g[i][j] == 0) p->g[i][j] = rand_item();
        }
    }
}

void regles_stabiliser(Plateau *p, int contrat[NB_ITEMS + 1], int *score) {
    while (1) {
        bool sup[LIGNES][COLONNES];
        marquer_suppressions(p, sup);

        if (!appliquer_suppressions(p, sup, contrat, score)) break;

        gravite(p);
        remplir(p);
    }
}

bool regles_swap_et_resoudre_si_valide(Plateau *p,
                                       int x1, int y1, int x2, int y2,
                                       int contrat[NB_ITEMS + 1],
                                       int *score) {
    plateau_swap(p, x1, y1, x2, y2);

    // validité = ça doit créer une figure
    if (!regles_existe_figure(p)) {
        plateau_swap(p, x1, y1, x2, y2);
        return false;
    }

    // si valide -> on résout tout
    regles_stabiliser(p, contrat, score);
    return true; }