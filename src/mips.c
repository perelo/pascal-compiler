/*
 * @File mips.c
 *
 * @Date 23-04-2013
 *
 */

#include <stdio.h>

#include "mips.h"
#include "registres.h"
#include "dico.h"
#include "code3adr.h"       // ligne

void genere_mips() {
    int i, l, r;
    int *dernier = creer_derniers_appels();
    initialiser_regs();

    /* ajouter les variables globales dans .data s'il y en a :
     * comme le variables sont ajoutes AVANT les fonctions,
     * si le premier element de symboles n'en est pas une,
     * alors il existe au moins une variable globale */
    if (symboles.sommet - symboles.base > 0 &&
        symboles.tab[symboles.base].mode != FONCTION) {
        printf("\t.data\n");
        printf("var:");
        for (i = symboles.base; i < symboles.sommet; ++i)
            if (symboles.tab[i].mode != FONCTION)
                printf("\t.word\t0\t# %s\n", symboles.tab[i].nom);
        printf("\n");
    }

    printf("\t.text\n");
    printf("\t.globl main\n");
    printf("main:\tnop\n");
    for (l = 0; l < ligne; ++l) {
        printf("j%i:\t", l);
        switch(code[l].op) {
          case c3a_plus:
            printf("add\t$t%i, $t%i, $t%i\n",
                    r=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2));
            reg[r] = l;
            break;
          case c3a_moins:
            printf("sub\t$t%i, $t%i, $t%i\n",
                    r=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2));
            reg[r] = l;
            break;
          case c3a_fois:
            printf("mult\t$t%i, $t%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2));
            printf("\tmflo\t$t%i\n",
                    r=cherche_registre_libre(dernier, l));
            reg[r] = l;
            break;
          case c3a_divise:
            printf("div\t$t%i, $t%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2));
            printf("\tmflo\t$t%i",
                    r=cherche_registre_libre(dernier, l));
            reg[r] = l;
            break;
          case c3a_modulo:
            printf("div\t$t%i, $t%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2));
            printf("\tmfhi\t$t%i",
                    r=cherche_registre_libre(dernier, l));
            reg[r] = l;
            break;
          case c3a_egal:
            break;
          case c3a_diff:
            break;
          case c3a_inf:
            break;
          case c3a_sup:
            break;
          case c3a_infeg:
            break;
          case c3a_supeg:
            break;
          case c3a_ou:
            break;
          case c3a_et:
            break;
          case c3a_non:
            break;
          case c3a_negatif:
            break;
          case lire:
            break;
          case ecrire:
            break;
          case load:
            {
            int reg_adr_var;
            printf("la\t$t%i, var\n",
                    reg_adr_var=cherche_registre_libre(dernier, l));
            reg[reg_adr_var] = l;
            printf("\tlw\t$t%i, %i($t%i)\n",
                    r=cherche_registre_libre(dernier, l),
                    sizeof(int) * recherche_symbole(code[l].var),
                    reg_adr_var);
            reg[reg_adr_var] = -1;
            reg[r] = l;
            }
            break;
          case store:
            {
            int reg_adr_var;
            printf("la\t$t%i, var\n",
                    reg_adr_var=cherche_registre_libre(dernier, l));
            reg[reg_adr_var] = l;
            printf("\tsw\t$t%i, %i($t%i)\n",
                    r=trouve_registre_associe(code[l].arg1),
                    sizeof(int) * recherche_symbole(code[l].var),
                    reg_adr_var);
            reg[reg_adr_var] = -1;
            reg[r] = l;
            }
            break;
          case ltab:
            break;
          case stab:
            break;
          case loadimm:
            printf("li\t$t%i, %i\n",
                    r=cherche_registre_libre(dernier, l),
                    code[l].arg1);
            reg[r] = l;
            break;
          case addimm:
            printf("addi\t$t%i, $t%i, $t%i\n",
                    r=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg1),
                    code[l].arg1);
            reg[r] = l;
            break;
          case jump:
            printf("j\t$t%i\n",
                trouve_registre_associe(code[l].arg1));
            break;
          case jsifaux:
            break;
          case param:
            break;
          case call:
            break;
          case entree:
            break;
          case sortie:
            break;
        }
    }
    printf("j%i:\tli $v0, 10\n", l);
    printf("\tsyscall       # exit\n");
}
