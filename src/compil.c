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
#include <string.h>

#include "lexical.h"
#include "syntaxique.h"     // MANGER_CC
#include "parcours.h"
#include "code3adr.h"

int main(int argc, char **argv) {
    if (argc != 2 + 1) {
        fprintf(stderr, "Usage : %s <input> <output_dir>\n", argv[0]);
        return 1;
    }

    /* ouvrir le fichier d'entree */
    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "failed to open <input>\n");
        return 1;
    }

    /* recuperer le nom du fichier sans extension */
    char *ifname = NULL;
    char *pch = strtok(argv[1], "/");
    while (pch != NULL) {
        ifname = pch;
        pch = strtok(NULL, "/");
    }
    ifname = strtok(ifname, ".");

    /* ouvrir les fichiers de sorties */
    char fname[64];
    sprintf(fname, "%s/%s.synt.xml", argv[2], ifname);
    sortie_syntaxique = fopen(fname, "w");
    sprintf(fname, "%s/%s.sem.xml", argv[2], ifname);
    sortie_semantique = fopen(fname, "w");
    sprintf(fname, "%s/%s.c3a", argv[2], ifname);
    FILE *sortie_c3a  = fopen(fname, "w");

    MANGER_CC;
    n_prog *prog = ProgramPascal(); // semantique + arbre abstrait
    initialiser_code();
    sem_n_prog (prog);  // table des symboles
    write_code(sortie_c3a);

    fclose(yyin);
    fclose(sortie_syntaxique);
    fclose(sortie_semantique);
    fclose(sortie_c3a);

    return 0;
}
