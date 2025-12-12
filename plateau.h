#ifndef PLATEAU_H
#define PLATEAU_H

#define LIGNES 25
#define COLONNES 45
#define NB_ITEMS 5

void initialiserPlateau(int plateau[LIGNES][COLONNES]);
void afficherPlateau(int plateau[LIGNES][COLONNES]);
void detecterCombinaisons(int plateau[LIGNES][COLONNES]);
int estDansPlateau(int valeur);

#endif