#ifndef REGLES_H
#define REGLES_H

#include <stdbool.h>
#include "plateau.h"

// détection (utile pour init sans match)
bool regles_existe_figure(const Plateau *p);
bool regles_case_dans_figure(const Plateau *p, int x, int y);

// swap + résolution
bool regles_swap_et_resoudre_si_valide(Plateau *p,
                                       int x1, int y1, int x2, int y2,
                                       int contrat[NB_ITEMS + 1],
                                       int *score);

// stabilisation (gravité + remplissage jusqu'à plus de figure)
void regles_stabiliser(Plateau *p, int contrat[NB_ITEMS + 1], int *score);

#endif