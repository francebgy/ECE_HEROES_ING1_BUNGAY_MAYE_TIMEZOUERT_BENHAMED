#ifndef PLATEAU_H
#define PLATEAU_H

#define LIGNES 25
#define COLONNES 45
#define NB_ITEMS 5

void initialiserPlateau(int plateau[LIGNES][COLONNES]);
void afficherPlateau(int plateau[LIGNES][COLONNES]);
void detecterCombinaisons(int plateau[LIGNES][COLONNES]);
int estDansPlateau(int valeur);
void supprimerCombinaisons(int plateau[LIGNES][COLONNES]);
void appliquerGravite(int plateau[LIGNES][COLONNES]);
void remplirPlateau(int plateau[LIGNES][COLONNES]);
int plateauContientCombinaisons(int plateau[LIGNES][COLONNES]);
void resoudrePlateau(int plateau[LIGNES][COLONNES]);
void afficherPlateauAvecCurseur(
    int plateau[LIGNES][COLONNES],
    int curseurX,
    int curseurY
);

#endif