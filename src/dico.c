/*
 * @File dico.c
 *
 * @Date 21-03-2013
 *
 * @Brief Definit l'implementation la table des symboles
 *
 */
#include <string.h>

#include "dico.h"

int ajoute_variable(char *nom, n_type *type) {
    if (symboles.sommet >= MAX_DICO)
        return -1;

    int* adresse = adresse_courante();
    variable v;
    v.nom = nom;
    v.type = type;
    v.param = NULL;
    v.adresse = (*adresse)++;
    v.mode = contexte;
    symboles.tab[symboles.sommet++] = v;

    return v.adresse;
}

int ajoute_fonction(char *nom, n_type *type, n_l_dec *param) {
    if (symboles.sommet >= MAX_DICO)
        return -1;

    variable f;
    f.nom = nom;
    f.type = type;
    f.param = param;
    f.adresse = -1;     // inconnu pour l'instant
    f.mode = FONCTION;
    symboles.tab[symboles.sommet++] = f;

    return 0;
}

int ajoute_tableau(char *nom, n_type *type, int debut, int fin) {
    if (symboles.sommet >= MAX_DICO)
        return -1;

    int *adresse = adresse_courante();
    variable t;
    t.nom = nom;
    t.type = type;
    t.param = NULL;
    t.adresse = *adresse;
    *adresse += fin - debut;
    t.mode = contexte;
    symboles.tab[symboles.sommet++] = t;

    return t.adresse;
}

/* cherche une variable; renvoie
    - l'indice si trouve
    - (-1) sinon. */
int cherche_locale(char *nom) {
    if (contexte == GLOBAL)
        return -1;  // la partie "table locale" n'existe pas

    int i;
    for (i = symboles.base; i < symboles.sommet; ++i) {
        if (strcmp(nom, symboles.tab[i].nom) == 0)
            return i;
    }
    return -1;
}

int cherche_globale(char *nom) {
    int i, bottom, top;
    if (contexte == GLOBAL) {
        bottom = symboles.base;
        top = symboles.sommet;
    } else { // LOCAL
        bottom = 0;
        top = symboles.base;
    }
    for (i = bottom; i < top; ++i) {
        if (strcmp(nom, symboles.tab[i].nom) == 0)
            return i;
    }
    return -1;
}

int recherche_symbole(char *nom) {
    int indice = -1;
    if (contexte == LOCAL) {
        indice = cherche_locale(nom);
    }
    return indice != -1 ? indice : cherche_globale(nom);
}

int* adresse_courante(void) {
    if (contexte == GLOBAL) {
        return &adresse_globale_courante;
    } else { // LOCAL
        return &adresse_locale_courante;
    }
}

/* renvoie l'adresse (locale ou globale) de la variable */
int adresse(int v) {
    // TODO
    return -1;
}

/* passe en contexte local / global */
void entree_fonction(void) {
    contexte = LOCAL;
    symboles.base = symboles.sommet;
}

void sortie_fonction(void) {
    contexte = GLOBAL;
    symboles.sommet = symboles.base;
    symboles.base = 0;
}
