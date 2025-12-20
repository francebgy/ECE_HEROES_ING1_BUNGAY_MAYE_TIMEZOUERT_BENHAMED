#include "plateau.h"
#include "regles.h"     // pour tester s'il y a déjà une figure (match)
#include <stdlib.h>

bool plateau_sont_adjacents(int x1, int y1, int x2, int y2) {
    int dx = x1 - x2; if (dx < 0) dx = -dx;
    int dy = y1 - y2; if (dy < 0) dy = -dy;
    return (dx + dy) == 1;
}

static int rand_item(void) {
    return 1 + (rand() % NB_ITEMS);
}

void plateau_init_aleatoire(Plateau *p) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            p->g[i][j] = rand_item();
        }
    }
}

void plateau_swap(Plateau *p, int x1, int y1, int x2, int y2) {
    int tmp = p->g[x1][y1];
    p->g[x1][y1] = p->g[x2][y2];
    p->g[x2][y2] = tmp;
}

void plateau_init_sans_match(Plateau *p) {
    // On remplit, puis on corrige jusqu'à ce qu'il n'y ait aucune figure
    plateau_init_aleatoire(p);

    // boucle de "nettoyage" simple
    while (regles_existe_figure(p)) {
        for (int i = 0; i < LIGNES; i++) {
            for (int j = 0; j < COLONNES; j++) {
                // si cette case fait partie d'une figure, on la change
                if (regles_case_dans_figure(p, i, j)) {
                    p->g[i][j] = rand_item();
                }
            }
        }
    }
}