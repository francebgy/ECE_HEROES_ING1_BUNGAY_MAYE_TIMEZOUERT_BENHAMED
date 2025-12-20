#include "sauvegarde.h"
#include <stdio.h>
#include <string.h>

#define FICHIER_SAUVEGARDE "sauvegardes.txt"
#define MAX_PSEUDO 63
#define MAX_LIGNES 500

typedef struct {
    char pseudo[MAX_PSEUDO + 1];
    int niveau;
    int vies;
    int score;
} Save;

static void vider_ligne(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int sauvegarde_demander_oui_non(const char *message) {
    int choix = 0;
    while (1) {
        printf("%s (1=Oui / 2=Non) : ", message);
        if (scanf("%d", &choix) != 1) {
            vider_ligne();
            continue;
        }
        vider_ligne();
        if (choix == 1) return 1;
        if (choix == 2) return 0;
    }
}

void sauvegarde_demander_pseudo(char *pseudo, size_t taille) {
    if (!pseudo || taille == 0) return;

    while (1) {
        printf("Pseudo (sans espaces) : ");
        if (scanf("%63s", pseudo) != 1) {
            vider_ligne();
            continue;
        }
        vider_ligne();

        if (strlen(pseudo) == 0) continue;
        return;
    }
}

bool sauvegarde_charger(const char *pseudo, int *niveau_atteint, int *vies, int *score) {
    FILE *f = fopen(FICHIER_SAUVEGARDE, "r");
    if (!f) return false;

    char p[MAX_PSEUDO + 1];
    int n, v, s;

    while (fscanf(f, "%63s %d %d %d", p, &n, &v, &s) == 4) {
        if (strcmp(p, pseudo) == 0) {
            fclose(f);
            if (niveau_atteint) *niveau_atteint = n;
            if (vies) *vies = v;
            if (score) *score = s;
            return true;
        }
    }

    fclose(f);
    return false;
}

bool sauvegarde_enregistrer(const char *pseudo, int niveau_atteint, int vies, int score) {
    Save tab[MAX_LIGNES];
    int nb = 0;

    // 1) Charger toutes les lignes si fichier existe
    FILE *f = fopen(FICHIER_SAUVEGARDE, "r");
    if (f) {
        while (nb < MAX_LIGNES) {
            Save s;
            if (fscanf(f, "%63s %d %d %d", s.pseudo, &s.niveau, &s.vies, &s.score) != 4) break;
            tab[nb++] = s;
        }
        fclose(f);
    }

    // 2) Chercher pseudo
    int idx = -1;
    for (int i = 0; i < nb; i++) {
        if (strcmp(tab[i].pseudo, pseudo) == 0) {
            idx = i;
            break;
        }
    }

    // 3) Mettre à jour ou ajouter
    if (idx >= 0) {
        tab[idx].niveau = niveau_atteint;
        tab[idx].vies = vies;
        tab[idx].score = score;
    } else {
        if (nb >= MAX_LIGNES) return false;
        strncpy(tab[nb].pseudo, pseudo, MAX_PSEUDO);
        tab[nb].pseudo[MAX_PSEUDO] = '\0';
        tab[nb].niveau = niveau_atteint;
        tab[nb].vies = vies;
        tab[nb].score = score;
        nb++;
    }

    // 4) Réécrire tout le fichier
    FILE *out = fopen(FICHIER_SAUVEGARDE, "w");
    if (!out) return false;

    for (int i = 0; i < nb; i++) {
        fprintf(out, "%s %d %d %d\n", tab[i].pseudo, tab[i].niveau, tab[i].vies, tab[i].score);
    }
    fclose(out);
    return true;