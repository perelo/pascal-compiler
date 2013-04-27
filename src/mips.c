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
#include "parcours.h"       // MAX_PARAM

variable* fonction = NULL;
int params [MAX_PARAM];
int index_param = 0;

void genere_mips() {
    int i, l, r;
    int wordsize = sizeof(int);
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
        for (i = symboles.base; i < symboles.sommet; ++i) {
            variable v = symboles.tab[i];
            if (v.mode == FONCTION) break;
            printf("\t.word\t");
            int j = (v.type->type == t_array) ? v.type->fin - v.type->debut
                                              : 1;
            while (j--) printf(" 0");
            printf("\t# %s\n", v.nom);
        }
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
            printf("li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            printf("\tbeq\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            printf("\tli\t$t%i, 0\n", r);
            reg[r] = l;
            break;
          case c3a_diff:
            printf("li\t$t%i, 0\n",
                    r=cherche_registre_libre(dernier, l));
            printf("\tbeq\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            printf("\tli\t$t%i, 1\n", r);
            reg[r] = l;
            break;
          case c3a_inf:
            printf("li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            printf("\tblt\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            printf("\tli\t$t%i, 0\n", r);
            reg[r] = l;
            break;
          case c3a_sup:
            break;
            printf("li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            printf("\tbgt\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            printf("\tli\t$t%i, 0\n", r);
            reg[r] = l;
          case c3a_infeg:
            break;
            printf("li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            printf("\tble\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            printf("\tli\t$t%i, 0\n", r);
            reg[r] = l;
          case c3a_supeg:
            break;
            printf("li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            printf("\tbge\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            printf("\tli\t$t%i, 0\n", r);
            reg[r] = l;
          case c3a_ou:
            printf("or\t$t%i, $t%i, $t%i\n",
                r=cherche_registre_libre(dernier, l),
                trouve_registre_associe(code[l].arg1),
                trouve_registre_associe(code[l].arg2));
            reg[r] = l;
            break;
          case c3a_et:
            printf("and\t$t%i, $t%i, $t%i\n",
                r=cherche_registre_libre(dernier, l),
                trouve_registre_associe(code[l].arg1),
                trouve_registre_associe(code[l].arg2));
            reg[r] = l;
            break;
          case c3a_non: // XOR with 1
            {
            int reg_one;
            printf("li\t$t%i, 1\n",
                    reg_one=cherche_registre_libre(dernier, l));
            reg[reg_one] = l;
            printf("\txor\t$t%i, $t%i, $t%i\n",
                    r=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg1),
                    reg_one);
            reg[reg_one] = -1;
            reg[r] = l;
            }
            break;
          case c3a_negatif:
            printf("sub\t$t%i, $zero, $t%i\n",
                    r=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg1));
            reg[r] = l;
            break;
          case lire:
            printf("li\t$v0, 5\n");
            printf("\tsyscall\t\t# read int\n");
            printf("\tmove\t$t%i, $v0\n",
                    r=cherche_registre_libre(dernier, l));
            reg[r] = l;
            break;
          case ecrire:
            printf("li\t$v0, 1\n");
            printf("\tmove\t$a0, $t%i\n",
                trouve_registre_associe(code[l].arg1));
            printf("\tsyscall\t\t# write int\n");
            break;
          case load:
            {
            // TODO si c'est local
            int reg_adr_var;
            printf("la\t$t%i, var\n",
                    reg_adr_var=cherche_registre_libre(dernier, l));
            reg[reg_adr_var] = l;
            printf("\tlw\t$t%i, %i($t%i)\n",
                    r=cherche_registre_libre(dernier, l),
                    wordsize * symboles.tab[recherche_symbole(code[l].var)].adresse,
                    reg_adr_var);
            reg[reg_adr_var] = -1;
            reg[r] = l;
            }
            break;
          case store:
            {
            // TODO si c'est local
            int reg_adr_var;
            printf("la\t$t%i, var\n",
                    reg_adr_var=cherche_registre_libre(dernier, l));
            reg[reg_adr_var] = l;
            printf("\tsw\t$t%i, %i($t%i)\n",
                    r=trouve_registre_associe(code[l].arg1),
                    wordsize * symboles.tab[recherche_symbole(code[l].var)].adresse,
                    reg_adr_var);
            reg[reg_adr_var] = -1;
            reg[r] = l;
            }
            break;
          case ltab:
            {
            variable v = symboles.tab[recherche_symbole(code[l].var)];
            int reg_adr_var, reg_offset, reg_sizeof_int;
            printf("addi\t$t%i, $t%i, %i\n",
                    reg_offset=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg2),
                    v.adresse - v.type->debut);
            reg[reg_offset] = l;
            printf("\tli\t$t%i, %i\n",
                    reg_sizeof_int=cherche_registre_libre(dernier, l),
                    wordsize);
            reg[reg_sizeof_int] = l;
            printf("\tmult\t$t%i, $t%i\n",
                    reg_offset,
                    reg_sizeof_int);
            printf("\tmflo\t$t%i\n", reg_offset);
            printf("\tla\t$t%i, var\n",
                    reg_adr_var=cherche_registre_libre(dernier, l));
            reg[reg_adr_var] = l;
            printf("\tadd\t$t%i, $t%i, $t%i\n",
                    reg_adr_var,
                    reg_adr_var,
                    reg_offset);
            printf("\tlw\t$t%i, 0($t%i)\n",
                    r=cherche_registre_libre(dernier, l),
                    reg_adr_var);
            reg[reg_sizeof_int] = -1;
            reg[reg_offset] = -1;
            reg[reg_adr_var] = -1;
            reg[r] = l;
            }
            break;
          case stab:
            {
            variable v = symboles.tab[recherche_symbole(code[l].var)];
            int reg_adr_var, reg_offset, reg_sizeof_int;
            printf("addi\t$t%i, $t%i, %i\n",
                    reg_offset=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg2),
                    v.adresse - v.type->debut);
            reg[reg_offset] = l;
            printf("\tli\t$t%i, %i\n",
                    reg_sizeof_int=cherche_registre_libre(dernier, l),
                    wordsize);
            reg[reg_sizeof_int] = l;
            printf("\tmult\t$t%i, $t%i\n",
                    reg_offset,
                    reg_sizeof_int);
            printf("\tmflo\t$t%i\n", reg_offset);
            printf("\tla\t$t%i, var\n",
                    reg_adr_var=cherche_registre_libre(dernier, l));
            reg[reg_adr_var] = l;
            printf("\tadd\t$t%i, $t%i, $t%i\n",
                    reg_adr_var,
                    reg_adr_var,
                    reg_offset);
            printf("\tsw\t$t%i, 0($t%i)\n",
                    trouve_registre_associe(code[l].arg1),
                    reg_adr_var);
            reg[reg_sizeof_int] = -1;
            reg[reg_offset] = -1;
            reg[reg_adr_var] = -1;
            }
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
            printf("j\tj%i\n", code[l].arg2);
            break;
          case jsifaux:
            printf("beq\t$t%i, $zero, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    code[l].arg2);
            break;
          case param:
            // TODO in sem, check max param
            params[index_param++] = trouve_registre_associe(code[l].arg1);
            break;
          case call:
            {
            /* empiler tous les registres temporaires */
            printf("addi\t$sp, $sp, -%i\n", NB_REGISTRES * wordsize);
            for (i = 0; i < NB_REGISTRES; ++i) {
                printf("\tsw\t$t%i, %i($sp)\n", i, i * wordsize);
            }
            /* reserver l'espace pour la valeur de retour et les parametres */
            printf("\taddi\t$sp, $sp, -%i\n", (1+index_param) * wordsize);
            /* empiler les parametres */
            if (index_param != 0) {
                while (index_param--) {
                    printf("\tsw\t$t%i, %i($sp)\n",
                            params[index_param],
                            index_param * wordsize);
                }
            }

            /* jump, bunny rabbit !!! */
            printf("\tjal\tj%i\n",
                        symboles.tab[recherche_symbole(code[l].var)].adresse);

            /* depiler la valeur de retour */
            printf("\tlw\t$t%i, 0($sp)\n",
                    r=cherche_registre_libre(dernier, l));
            printf("\taddi\t$sp, $sp, %i\n", wordsize);
            /* depiler les registres sauvegardes */
            for (i = 0; i < NB_REGISTRES; ++i) {
                printf("\tlw\t$t%i, %i($sp)\n", i, i * wordsize);
            }
            printf("addi\t$sp, $sp, %i\n", NB_REGISTRES * wordsize);

            reg[r] = l;
            }
            break;
          case entree:
            fonction = &symboles.tab[recherche_symbole(code[l].var)];
            fonction->adresse = l;
            /* reserver l'espace pour les variables locales et $ra */
            printf("addi\t$sp, $sp, -%i\n", (1 + code[l].arg1) * wordsize);
            /* sauver $ra au dessous des variables locales */
            printf("\tsw\t$ra, 0($sp)\n");
            break;
          case sortie:
            /* recuperer $ra */
            printf("\tlw\t$ra, 0($sp)\n");
            /* liberer les variables locales les arguments et $ra */
            printf("addi\t$sp, $sp, %i\n", (1 + code[l].arg1) * wordsize);
            /* sauter a l'appelant */
            printf("\tjr\t$ra\n");
            fonction = NULL;
            break;
        }
    }
    printf("j%i:\tli $v0, 10\n", l);
    printf("\tsyscall\t\t# exit\n");
}
