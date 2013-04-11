#ifndef __CODE3ADR__
#define __CODE3ADR__

#include <stdio.h>      // FILE

#include "syntabs.h"

/* Liste des opérations en code à 3 adresses. Toutes les opérations
   arithmétiques de arbre.h y apparaissent à la même place. */
typedef enum {c3a_plus, c3a_moins, c3a_fois, c3a_divise, c3a_modulo,
              c3a_egal, c3a_diff, c3a_inf, c3a_sup, c3a_infeg, c3a_supeg,
              c3a_ou, c3a_et, c3a_non, c3a_negatif,
              lire, ecrire, load, store, ltab, stab, loadimm,
              addimm, jump, jsifaux, param, call, entree, sortie}
  c3a_op;


/* Le code est un tableau de triplets. */
struct triplet {
  c3a_op op;
  int arg1;
  int arg2;
  char *var;
  int adresse;
  int mode;
} *code;

/* taille totale du tableau (utile pour réallocation) */
int taillecode;
/* ligne courante du tableau */
int ligne;

void initialiser_code();
void parcours_prog(n_prog *n);
void ajoute_ligne(c3a_op op, int arg1, int arg2, char *var);
void write_code(FILE *f);

#endif
