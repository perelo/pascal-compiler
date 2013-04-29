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

int calcul_tab_offset(FILE *f, struct triplet *code, int *dernier, int l);

variable* fonction = NULL;
int params [MAX_PARAM];
int nb_param = 0;

void genere_mips(FILE *f) {
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
        fprintf(f, "\t.data\n");
        fprintf(f, "var:");
        for (i = symboles.base; i < symboles.sommet; ++i) {
            variable v = symboles.tab[i];
            if (v.mode == FONCTION) break;
            fprintf(f, "\t.word\t");
            int j = (v.type->type == t_array) ? v.type->fin - v.type->debut
                                              : 1;
            while (j--) fprintf(f, " 0");
            fprintf(f, "\t# %s\n", v.nom);
        }
        fprintf(f, "\n");
    }

    fprintf(f, "\t.text\n");
    fprintf(f, "\t.globl main\n");
    fprintf(f, "main:\tnop\n");
    for (l = 0; l < ligne; ++l) {
        if (code[l].op != param)
            fprintf(f, "j%i:\t", l);
        switch(code[l].op) {
          case c3a_plus:
            fprintf(f, "add\t$t%i, $t%i, $t%i\n",
                    r=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2));
            reg[r] = l;
            break;
          case c3a_moins:
            fprintf(f, "sub\t$t%i, $t%i, $t%i\n",
                    r=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2));
            reg[r] = l;
            break;
          case c3a_fois:
            fprintf(f, "mult\t$t%i, $t%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2));
            fprintf(f, "\tmflo\t$t%i\n",
                    r=cherche_registre_libre(dernier, l));
            reg[r] = l;
            break;
          case c3a_divise:
            fprintf(f, "div\t$t%i, $t%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2));
            fprintf(f, "\tmflo\t$t%i\n",
                    r=cherche_registre_libre(dernier, l));
            reg[r] = l;
            break;
          case c3a_modulo:
            fprintf(f, "div\t$t%i, $t%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2));
            fprintf(f, "\tmfhi\t$t%i\n",
                    r=cherche_registre_libre(dernier, l));
            reg[r] = l;
            break;
          case c3a_egal:
            fprintf(f, "li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            fprintf(f, "\tbeq\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            fprintf(f, "\tli\t$t%i, 0\n", r);
            reg[r] = l;
            break;
          case c3a_diff:
            fprintf(f, "li\t$t%i, 0\n",
                    r=cherche_registre_libre(dernier, l));
            fprintf(f, "\tbeq\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            fprintf(f, "\tli\t$t%i, 1\n", r);
            reg[r] = l;
            break;
          case c3a_inf:
            fprintf(f, "li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            fprintf(f, "\tblt\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            fprintf(f, "\tli\t$t%i, 0\n", r);
            reg[r] = l;
            break;
          case c3a_sup:
            fprintf(f, "li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            fprintf(f, "\tbgt\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            fprintf(f, "\tli\t$t%i, 0\n", r);
            reg[r] = l;
            break;
          case c3a_infeg:
            fprintf(f, "li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            fprintf(f, "\tble\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            fprintf(f, "\tli\t$t%i, 0\n", r);
            reg[r] = l;
            break;
          case c3a_supeg:
            fprintf(f, "li\t$t%i, 1\n",
                    r=cherche_registre_libre(dernier, l));
            fprintf(f, "\tbge\t$t%i, $t%i, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    trouve_registre_associe(code[l].arg2),
                    l+1);
            fprintf(f, "\tli\t$t%i, 0\n", r);
            reg[r] = l;
            break;
          case c3a_ou:
            fprintf(f, "or\t$t%i, $t%i, $t%i\n",
                r=cherche_registre_libre(dernier, l),
                trouve_registre_associe(code[l].arg1),
                trouve_registre_associe(code[l].arg2));
            reg[r] = l;
            break;
          case c3a_et:
            fprintf(f, "and\t$t%i, $t%i, $t%i\n",
                r=cherche_registre_libre(dernier, l),
                trouve_registre_associe(code[l].arg1),
                trouve_registre_associe(code[l].arg2));
            reg[r] = l;
            break;
          case c3a_non: // XOR with 1
            {
            int reg_one;
            fprintf(f, "li\t$t%i, 1\n",
                    reg_one=cherche_registre_libre(dernier, l));
            reg[reg_one] = l;
            fprintf(f, "\txor\t$t%i, $t%i, $t%i\n",
                    r=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg1),
                    reg_one);
            reg[reg_one] = -1;
            reg[r] = l;
            }
            break;
          case c3a_negatif:
            fprintf(f, "sub\t$t%i, $zero, $t%i\n",
                    r=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg1));
            reg[r] = l;
            break;
          case lire:
            fprintf(f, "li\t$v0, 5\n");
            fprintf(f, "\tsyscall\t\t# read int\n");
            fprintf(f, "\tmove\t$t%i, $v0\n",
                    r=cherche_registre_libre(dernier, l));
            reg[r] = l;
            break;
          case ecrire:
            fprintf(f, "li\t$v0, 1\n");
            fprintf(f, "\tmove\t$a0, $t%i\n",
                trouve_registre_associe(code[l].arg1));
            fprintf(f, "\tsyscall\t\t# write int\n");
            break;
          case load:
            if (code[l].mode == LOCAL) {
                fprintf(f, "lw\t$t%i, -%i($fp)\n",
                        r=cherche_registre_libre(dernier, l),
                        code[l].adresse * wordsize);
            } else { // GLOBAL
                int reg_adr_var;
                fprintf(f, "la\t$t%i, var\n",
                        reg_adr_var=cherche_registre_libre(dernier, l));
                reg[reg_adr_var] = l;
                fprintf(f, "\tlw\t$t%i, %i($t%i)\n",
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
                    fprintf(f, "sw\t$t%i, 0($fp)\n",
                            r=trouve_registre_associe(code[l].arg1));
                } else {
                    fprintf(f, "sw\t$t%i, -%i($fp)\n",
                            r=trouve_registre_associe(code[l].arg1),
                            code[l].adresse * wordsize);
                }
            } else { // GLOBAL
                int reg_adr_var;
                fprintf(f, "la\t$t%i, var\n",
                        reg_adr_var=cherche_registre_libre(dernier, l));
                reg[reg_adr_var] = l;
                fprintf(f, "\tsw\t$t%i, %i($t%i)\n",
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
            reg_offset = calcul_tab_offset(f, code, dernier, l);
            fprintf(f, "\tla\t$t%i, var\n",
                    reg_adr_var=cherche_registre_libre(dernier, l));
            reg[reg_adr_var] = l;
            fprintf(f, "\tadd\t$t%i, $t%i, $t%i\n",
                    reg_adr_var,
                    reg_adr_var,
                    reg_offset);
            fprintf(f, "\tlw\t$t%i, 0($t%i)\n",
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
            reg_offset = calcul_tab_offset(f, code, dernier, l);
            fprintf(f, "\tla\t$t%i, var\n",
                    reg_adr_var=cherche_registre_libre(dernier, l));
            reg[reg_adr_var] = l;
            fprintf(f, "\tadd\t$t%i, $t%i, $t%i\n",
                    reg_adr_var,
                    reg_adr_var,
                    reg_offset);
            fprintf(f, "\tsw\t$t%i, 0($t%i)\n",
                    trouve_registre_associe(code[l].arg1),
                    reg_adr_var);
            reg[reg_offset] = -1;
            reg[reg_adr_var] = -1;
            }
            break;
          case loadimm:
            fprintf(f, "li\t$t%i, %i\n",
                    r=cherche_registre_libre(dernier, l),
                    code[l].arg1);
            reg[r] = l;
            break;
          case addimm:
            fprintf(f, "addi\t$t%i, $t%i, $t%i\n",
                    r=cherche_registre_libre(dernier, l),
                    trouve_registre_associe(code[l].arg1),
                    code[l].arg1);
            reg[r] = l;
            break;
          case jump:
            fprintf(f, "j\tj%i\n", code[l].arg2);
            break;
          case jsifaux:
            fprintf(f, "beq\t$t%i, $zero, j%i\n",
                    trouve_registre_associe(code[l].arg1),
                    code[l].arg2);
            break;
          case param:
            params[nb_param++] = trouve_registre_associe(code[l].arg1);
            break;
          case call:
            {
            /* empiler tous les registres temporaires et $fp */
            fprintf(f, "addi\t$sp, $sp, -%i\n", (NB_REGISTRES + 1) * wordsize);
            for (i = 0; i < NB_REGISTRES; ++i) {
                fprintf(f, "\tsw\t$t%i, %i($sp)\n", i, i * wordsize);
            }
            fprintf(f, "\tsw\t$fp, %i($sp)\n", i * wordsize);
            /* reserver l'espace pour la valeur de retour et les parametres */
            fprintf(f, "\taddi\t$sp, $sp, -%i\n", (1+nb_param) * wordsize);
            /* pointer $fp vers l'adr de la valeur de retour */
            fprintf(f, "\tadd\t$fp, $sp, %i\n", nb_param * wordsize);
            /* empiler les parametres */
            if (nb_param != 0) {
                /* du haut vert le bas a partir de $fp */
                for (i = 0; i < nb_param; ++i) {
                //while (nb_param--) {
                    fprintf(f, "\tsw\t$t%i, -%i($fp)\n",
                            params[i],
                            (i + 1) * wordsize);
                }
            }

            /* consommer les parametres, preparation au prochain call */
            nb_param = 0;
            /* jump, bunny rabbit !!! */
            fprintf(f, "\tjal\tj%i\n",
                        symboles.tab[recherche_symbole(code[l].var)].adresse);

            /* restaurer les registres sauvegardes et $fp */
            for (i = 0; i < NB_REGISTRES; ++i) {
                                                    // +1 because return value
                                                    // is still under regs
                fprintf(f, "\tlw\t$t%i, %i($sp)\n", i, (i+1) * wordsize);
            }
            fprintf(f, "\tlw\t$fp, %i($sp)\n", (i+1) * wordsize);
            /* recuperer la valeur de retour */
            fprintf(f, "\tlw\t$t%i, 0($sp)\n",
                    r=cherche_registre_libre(dernier, l));
            /* liberer la pile de la valeur de retour + des registres + $fp */
            fprintf(f, "\taddi\t$sp, $sp, %i\n", (NB_REGISTRES + 2) * wordsize);

            reg[r] = l;
            }
            break;
          case entree:
            fonction = &symboles.tab[recherche_symbole(code[l].var)];
            fonction->adresse = l;
            /* reserver l'espace pour les variables locales et $ra */
            fprintf(f, "addi\t$sp, $sp, -%i\n", (1 + code[l].arg1) * wordsize);
            /* sauver $ra au dessous des variables locales */
            fprintf(f, "\tsw\t$ra, 0($sp)\n");
            break;
          case sortie:
            /* recuperer $ra */
            fprintf(f, "lw\t$ra, 0($sp)\n");
            /* liberer les variables locales les arguments et $ra */
            fprintf(f, "\taddi\t$sp, $sp, %i\n", (1 + code[l].arg1) * wordsize);
            /* sauter a l'appelant */
            fprintf(f, "\tjr\t$ra\n");
            fonction = NULL;
            break;
        }
    }
    fprintf(f, "j%i:\tli $v0, 10\n", l);
    fprintf(f, "\tsyscall\t\t# exit\n");
}

/* genere le code qui calcule l'offset physique pour acceder au tableau
 * renvoie le numero du registre qui contient cet offset */
int calcul_tab_offset(FILE *f, struct triplet *code, int *dernier, int l) {
    int reg_offset, reg_sizeof_int;
    variable v = symboles.tab[recherche_symbole(code[l].var)];
    /* calcul de l'offset logique (indice + adr - debut) */
    fprintf(f, "addi\t$t%i, $t%i, %i\n",
            reg_offset=cherche_registre_libre(dernier, l),
            trouve_registre_associe(code[l].arg2),
            v.adresse - v.type->debut);
    reg[reg_offset] = l;
    /* calcul de l'offset physique (offset logique * wordsize) */
    fprintf(f, "\tli\t$t%i, %i\n",
            reg_sizeof_int=cherche_registre_libre(dernier, l),
            sizeof(int));
    fprintf(f, "\tmult\t$t%i, $t%i\n",
            reg_offset,
            reg_sizeof_int);
    fprintf(f, "\tmflo\t$t%i\n", reg_offset);
    return reg_offset;
}
