/*
 * @File dico.h
 *
 * @Date 21-03-2013
 *
 * @Brief Declaration de la table des symboles
 *
 */
#ifndef __DICO_H__
#define __DICO_H__

#include "syntabs.h"

/* taille du dico (reajustable) */
#define MAX_DICO 100

/* modes d'ecriture de la table
 * et contexte d'execution */
#define GLOBAL      0
#define LOCAL       1
#define FONCTION    2

int contexte;       /* uniquement GLOBAL ou LOCAL */

/* adresses relatives */
int adresse_locale_courante;
int adresse_globale_courante;

/* variable */
typedef struct {
    char *nom;
    n_type *type;   /* type de retour pour les fonctions */
    n_l_dec *param; /* parametres pour les fonctions */
    int adresse;    /* en memoire */
    int mode;
} variable;

/* dico
                <- MAX_DICO
  -----------
  |         |
  | (vide)  |   <- sommet
  -----------
  |variables|
  |locales  |   <- base
  -----------
  |variables|
  |globales |   <- 0
  -----------
*/

typedef struct {
    variable tab[MAX_DICO];
    int base;
    int sommet;
} dico;

dico symboles;

/* ajoute des noms dans la table */
int ajoute_variable(char *nom, n_type *type);
int ajoute_fonction(char *nom, n_type *type, n_l_dec *param);
int ajoute_tableau(char *nom, n_type *type, int debut, int fin);

/* cherche une variable; renvoie
    - l'indice si trouve
    - (-1) sinon. */
int cherche_locale(char *nom);
int cherche_globale(char *nom);
int recherche_symbole(char *nom);

/* renvoie l'adresse (locale ou globale) de la variable */
int adresse(int v);
int* adresse_courante(void);

/* passe en contexte local / global */
void entree_fonction(void);
void sortie_fonction(void);

#endif /* __DICO_H__ */
