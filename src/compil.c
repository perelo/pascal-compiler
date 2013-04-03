/*
 * @File compil.c
 *
 * @Date 21-03-2013
 *
 * @Brief Programme principal pour le projet de compilation
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "lexical.h"
#include "syntaxique.h"     // MANGER_CC
#include "parcours.h"

int main(int argc, char **argv) {
    if (argc != 1 + 3) {
        fprintf(stderr, "Usage : %s <input> <output_syntaxique> "
                                           "<output_lexical>\n", argv[0]);
        return 1;
    }

    sortie_syntaxique = fopen(argv[2], "w");
    sortie_semantique = fopen(argv[3], "w");

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "failed to open <input>\n");
        return 1;
    }

    MANGER_CC;
    n_prog *prog = ProgramPascal(); // semantique + arbre abstrait
    sem_n_prog (prog);  // table des symboles

    return 0;
}
