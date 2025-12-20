#ifndef PLATEAU_H
#define PLATEAU_H

#include <stdbool.h>

#define LIGNES   10
#define COLONNES 14
#define NB_ITEMS 5   // 1..5

typedef struct {
    int g[LIGNES][COLONNES];
} Plateau;

bool plateau_sont_adjacents(int x1, int y1, int x2, int y2);

void plateau_init_aleatoire(Plateau *p);
void plateau_init_sans_match(Plateau *p);

void plateau_swap(Plateau *p, int x1, int y1, int x2, int y2);

#endif