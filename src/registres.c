/*
 * @File registres.c
 *
 * @Date 11-04-2013
 *
 */
#include <stdlib.h>

#include "registres.h"
#include "code3adr.h"
#include "syntaxique.h"     // erreur()

int *creer_derniers_appels() {
    int l;
    int *tab = malloc(ligne * sizeof(int));
    for (l = 0; l < ligne; ++l) {
        tab[l] = l;
        if (code[l].op == c3a_plus   ||
            code[l].op == c3a_moins  ||
            code[l].op == c3a_fois   ||
            code[l].op == c3a_divise ||
            code[l].op == c3a_modulo ||
            code[l].op == c3a_egal   ||
            code[l].op == c3a_diff   ||
            code[l].op == c3a_inf    ||
            code[l].op == c3a_sup    ||
            code[l].op == c3a_infeg  ||
            code[l].op == c3a_supeg  ||
            code[l].op == c3a_ou     ||
            code[l].op == c3a_et) {
            tab[code[l].arg1] = l;
            tab[code[l].arg2] = l;
        }
        else if (code[l].op == c3a_non     ||
                 code[l].op == c3a_negatif ||
                 code[l].op == ecrire      ||
                 code[l].op == store       ||
                 code[l].op == stab        ||
                 code[l].op == addimm      ||
                 code[l].op == jsifaux     ||
                 code[l].op == param) {
            tab[code[l].arg1] = l;
        }
    }
    return tab;
}

void initialiser_regs() {
    memset(reg, -1, NB_REGISTRES);
}

int cherche_registre_libre(int *dernier, int ligne) {
    int i;
    for (i = 0; i < NB_REGISTRES; ++i) {
        if (reg[i] == -1 ||
            dernier[reg[i]] < ligne)
            return i;
    }
    erreur(__FUNCTION__, "err: no more available registers");
    return -1;
}

int trouve_registre_associe(int ligne) {
    int i;
    for (i = 0; i < NB_REGISTRES; ++i) {
        if (reg[i] == ligne)
            return ligne;
    }
    return -1;
}

