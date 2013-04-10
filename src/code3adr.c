#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "syntabs.h"
#include "code3adr.h"

#define TAILLE_CODE_INIT 20

/* Fonctions utilitaires */

void initialiser_code() {
    taillecode = TAILLE_CODE_INIT;
    code = (struct triplet *) malloc (taillecode * sizeof(struct triplet));
    ligne = 0;
}

void ajoute_ligne(c3a_op op, int arg1, int arg2, char *var) {
    if (ligne >= taillecode){
        taillecode *= 2;
        code = realloc (code, taillecode * sizeof(struct triplet));
    }
    code[ligne].op = op;
    code[ligne].arg1 = arg1;
    code[ligne].arg2 = arg2;
    code[ligne++].var = var;
}

void write_code(FILE *f) {
    /* Affiche le code, s'il respecte la table du TP. L'ordre dans le
       tableau op2string doit correspondre à celui des opérations
       dans code3adr.h. */
    char *op2string[] = { "plus", "moins", "fois", "divise", "modulo",
                          "egal", "diff", "inf", "sup", "infeg", "supeg",
                          "ou", "et", "non", "negatif",
                          "lire", "ecrire", "load", "store", "ltab", "stab",
                          "loadimm", "addimm", "jump", "jsifaux",
                          "param", "call", "entree", "sortie" };
    int l;
    for (l=0; l<ligne; l++) {
        fprintf(f, "%i:\t%s\t", l, op2string[code[l].op]);
        if (code[l].op < 15) {
            fprintf(f, "%i", code[l].arg1);
            if (code[l].op < 13)
                fprintf(f, ", %i", code[l].arg2);
        }
        else switch(code[l].op) {
          case store:
            fprintf(f, "%i, ", code[l].arg1);
          case load:
          case call:
          case entree:
          case sortie:
            fprintf(f, "%s", code[l].var);
            break;
          case stab:
            fprintf(f, "%i, ", code[l].arg1);
          case ltab:
            fprintf(f, "%s[%i]", code[l].var, code[l].arg2);
            break;
          case loadimm:
          case ecrire:
          case param:
            fprintf(f, "%i", code[l].arg1);
            break;
          case addimm:
            fprintf(f, "%i, %i", code[l].arg1, code[l].arg2);
            break;
          case jsifaux:
            fprintf(f, "%i, ", code[l].arg1);
          case jump:
            fprintf(f, "%i", code[l].arg2);
          default: break;
        }
        fputc('\n', f);
    }
}
