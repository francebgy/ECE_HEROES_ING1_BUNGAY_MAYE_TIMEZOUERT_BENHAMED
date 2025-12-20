#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <stdbool.h>
#include "plateau.h"

void affichage_clear(void);
void affichage_menu(void);
void affichage_regles(void);
void affichage_message(const char *msg);
void affichage_pause(void);
void affichage_fondRose(void);
void affichage_resetCouleur(void);

int  affichage_lire_int_menu(void);

// clavier jeu
void affichage_enable_raw(void);
void affichage_disable_raw(void);

enum {
    KEY_NONE = 0,
    KEY_UP = 1001,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER,
    KEY_ESC
};

int affichage_lire_touche(void);

const char* affichage_symbole_item(int v);

void affichage_plateau(const Plateau *p,
                       int cx, int cy,
                       bool selection, int sx, int sy,
                       const int contrat[NB_ITEMS + 1],
                       int vies, int coups, int temps,
                       int niveau, int score);

#endif