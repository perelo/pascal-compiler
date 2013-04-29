/*
 * @File mips.c
 *
 * @Date 23-04-2013
 *
 */

#include <stdio.h>
#include <string.h>         // strcmp()

#include "mips.h"
#include "registres.h"
#include "dico.h"
#include "code3adr.h"       // ligne
#include "parcours.h"       // MAX_PARAM

int calcul_tab_offset(struct triplet *code, int *dernier, int l);

variable* fonction = NULL;
int params [MAX_PARAM];
int nb_param = 0;

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
        if (code[l].op != param)
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
            printf("\tmflo\t$t%i\n",
                    r=cherche_registre_libre(dernier, l));
            reg[r] = l;
            break;
          case c3a_modulo:
            printf("div\t$t%i, $t%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2));
            printf("\tmfhi\t$t%i\n",
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
            printf("li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            printf("\tbgt\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            printf("\tli\t$t%i, 0\n", r);
            reg[r] = l;
            break;
          case c3a_infeg:
            printf("li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            printf("\tble\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            printf("\tli\t$t%i, 0\n", r);
            reg[r] = l;
            break;
          case c3a_supeg:
            printf("li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            printf("\tbge\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            printf("\tli\t$t%i, 0\n", r);
            reg[r] = l;
            break;
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
            if (code[l].mode == LOCAL) {
                printf("lw\t$t%i, -%i($fp)\n",
                        r=cherche_registre_libre(dernier, l),
                        code[l].adresse * wordsize);
            } else { // GLOBAL
                int reg_adr_var;
                printf("la\t$t%i, var\n",
                        reg_adr_var=cherche_registre_libre(dernier, l));
                reg[reg_adr_var] = l;
                printf("\tlw\t$t%i, %i($t%i)\n",
                        r=cherche_registre_libre(dernier, l),
                        wordsize * code[l].adresse,
                        reg_adr_var);
                reg[reg_adr_var] = -1;
            }
            reg[r] = l;
            break;
          case store:
            if (code[l].mode == LOCAL) {
                if (strcmp(code[l].var, fonction->nom) == 0) {
                    printf("sw\t$t%i, 0($fp)\n",
                            r=trouve_registre_associe(code[l].arg1));
                } else {
                    printf("sw\t$t%i, -%i($fp)\n",
                            r=trouve_registre_associe(code[l].arg1),
                            code[l].adresse * wordsize);
                }
            } else { // GLOBAL
                int reg_adr_var;
                printf("la\t$t%i, var\n",
                        reg_adr_var=cherche_registre_libre(dernier, l));
                reg[reg_adr_var] = l;
                printf("\tsw\t$t%i, %i($t%i)\n",
                        r=trouve_registre_associe(code[l].arg1),
                        wordsize * code[l].adresse,
                        reg_adr_var);
                reg[reg_adr_var] = -1;
            }
            reg[r] = l;
            break;
          case ltab:
            {
            int reg_adr_var, reg_offset;
            reg_offset = calcul_tab_offset(code, dernier, l);
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
            reg[reg_offset] = -1;
            reg[reg_adr_var] = -1;
            reg[r] = l;
            }
            break;
          case stab:
            {
            int reg_adr_var, reg_offset;
            reg_offset = calcul_tab_offset(code, dernier, l);
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
            params[nb_param++] = trouve_registre_associe(code[l].arg1);
            break;
          case call:
            {
            /* empiler tous les registres temporaires et $fp */
            printf("addi\t$sp, $sp, -%i\n", (NB_REGISTRES + 1) * wordsize);
            for (i = 0; i < NB_REGISTRES; ++i) {
                printf("\tsw\t$t%i, %i($sp)\n", i, i * wordsize);
            }
            printf("\tsw\t$fp, %i($sp)\n", i * wordsize);
            /* reserver l'espace pour la valeur de retour et les parametres */
            printf("\taddi\t$sp, $sp, -%i\n", (1+nb_param) * wordsize);
            /* pointer $fp vers l'adr de la valeur de retour */
            printf("\tadd\t$fp, $sp, %i\n", nb_param * wordsize);
            /* empiler les parametres */
            if (nb_param != 0) {
                /* du haut vert le bas a partir de $fp */
                for (i = 0; i < nb_param; ++i) {
                //while (nb_param--) {
                    printf("\tsw\t$t%i, -%i($fp)\n",
                            params[i],
                            (i + 1) * wordsize);
                }
            }

            /* consommer les parametres, preparation au prochain call */
            nb_param = 0;
            /* jump, bunny rabbit !!! */
            printf("\tjal\tj%i\n",
                        symboles.tab[recherche_symbole(code[l].var)].adresse);

            /* restaurer les registres sauvegardes et $fp */
            for (i = 0; i < NB_REGISTRES; ++i) {
                                                    // +1 because return value
                                                    // is still under regs
                printf("\tlw\t$t%i, %i($sp)\n", i, (i+1) * wordsize);
            }
            printf("\tlw\t$fp, %i($sp)\n", (i+1) * wordsize);
            /* recuperer la valeur de retour */
            printf("\tlw\t$t%i, 0($sp)\n",
                    r=cherche_registre_libre(dernier, l));
            /* liberer la pile de la valeur de retour + des registres + $fp */
            printf("\taddi\t$sp, $sp, %i\n", (NB_REGISTRES + 2) * wordsize);

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
            printf("lw\t$ra, 0($sp)\n");
            /* liberer les variables locales les arguments et $ra */
            printf("\taddi\t$sp, $sp, %i\n", (1 + code[l].arg1) * wordsize);
            /* sauter a l'appelant */
            printf("\tjr\t$ra\n");
            fonction = NULL;
            break;
        }
    }
    printf("j%i:\tli $v0, 10\n", l);
    printf("\tsyscall\t\t# exit\n");
}

/* genere le code qui calcule l'offset physique pour acceder au tableau
 * renvoie le numero du registre qui contient cet offset */
int calcul_tab_offset(struct triplet *code, int *dernier, int l) {
    int reg_offset, reg_sizeof_int;
    variable v = symboles.tab[recherche_symbole(code[l].var)];
    /* calcul de l'offset logique (indice + adr - debut) */
    printf("addi\t$t%i, $t%i, %i\n",
            reg_offset=cherche_registre_libre(dernier, l),
            trouve_registre_associe(code[l].arg2),
            v.adresse - v.type->debut);
    reg[reg_offset] = l;
    /* calcul de l'offset physique (offset logique * wordsize) */
    printf("\tli\t$t%i, %i\n",
            reg_sizeof_int=cherche_registre_libre(dernier, l),
            sizeof(int));
    printf("\tmult\t$t%i, $t%i\n",
            reg_offset,
            reg_sizeof_int);
    printf("\tmflo\t$t%i\n", reg_offset);
    return reg_offset;
}
