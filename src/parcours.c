/*
 * @File parcours.c
 *
 * @Date 21-03-2013
 *
 * @Brief Parcours semantique de l'arbre arbstrait
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntabs.h"
#include "parcours.h"
#include "syntaxique.h"     // fonctions de balise
#include "dico.h"
#include "code3adr.h"

void sem_parametres(n_l_exp *args, n_l_dec *params);

int dico_indice_var;

void sem_n_l_instr(n_l_instr *l_instr) {
    if (l_instr == NULL) return;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    if (l_instr->queue != NULL) {
        sem_n_l_instr(l_instr->queue);
    }
    sem_n_instr(l_instr->tete);
    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_n_instr(n_instr *instr) {
    if (instr == NULL) return;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    switch (instr->type) {
    case affecteInst:
        sem_affecteInst(instr);
        break;
    case siInst:
        sem_siInst(instr);
        break;
    case tantqueInst:
        sem_tantqueInst(instr);
        break;
    case appelInst:
        sem_appelInst(instr);
        break;
    case ecrireInst:
        sem_ecrireInst(instr);
        break;
    case videInst:
        sem_videInst(instr);
        break;
    case blocInst:
        sem_blocInst(instr);
        break;
    }
    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_affecteInst(n_instr *instr) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    n_type *type_var = NULL;
    n_type *type_exp = NULL;

    type_exp = sem_n_exp(instr->u.affecte_.exp);
    int ligneexp = ligne-1;
    type_var = sem_n_var(instr->u.affecte_.var);

    if (type_var->type == t_array) {
        ajoute_ligne(stab, ligneexp, ligne-1,
                                    instr->u.affecte_.var->nom);
        type_var = type_var->arrayof;
    }
    else {
        ajoute_ligne(store, ligne-1, 0, instr->u.affecte_.var->nom);
    }
    code[ligne-1].adresse = symboles.tab[dico_indice_var].adresse;
    code[ligne-1].mode = symboles.tab[dico_indice_var].mode;

    assert_types_sont_compatibles(__FUNCTION__, type_var, type_exp, NULL);

    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_siInst(n_instr *instr) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    n_type *type_test = NULL;

    type_test = sem_n_exp(instr->u.si_.test);
    int lignetmp = ligne;
    ajoute_ligne(jsifaux, ligne-1, 0, NULL);
    if (type_test == NULL || type_test->type != t_bool)
        erreur(__FUNCTION__, "err : \"if\" test must be a boolean");

    sem_n_instr(instr->u.si_.alors);
    code[lignetmp].arg2 = ligne;
    if (instr->u.si_.sinon != NULL) {
        ++code[lignetmp].arg2;
        int ligneelse = ligne;
        ajoute_ligne(jump, 0, 0, NULL);
        sem_n_instr(instr->u.si_.sinon);
        code[ligneelse].arg2 = ligne;
    }

    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_tantqueInst(n_instr *instr) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    n_type *type_test = NULL;

    int lignetest = ligne;
    type_test = sem_n_exp(instr->u.tantque_.test);
    int lignetmp = ligne;
    ajoute_ligne(jsifaux, ligne-1, 0, NULL);
    if (type_test == NULL || type_test->type != t_bool)
        erreur(__FUNCTION__, "err : \"while\" test must be a boolean");

    sem_n_instr(instr->u.tantque_.faire);
    ajoute_ligne(jump, 0, lignetest, NULL);
    code[lignetmp].arg2 = ligne;

    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_appelInst(n_instr *instr) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    sem_n_appel(instr->u.appel);

    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_ecrireInst(n_instr *instr) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    sem_n_exp(instr->u.ecrire_.expression);
    ajoute_ligne(ecrire, ligne-1, 0, NULL);

    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_videInst(n_instr *instr) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);
    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_blocInst(n_instr *instr) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    sem_n_l_instr(instr->u.liste);

    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_n_l_exp(n_l_exp *l_exp) {
    if (l_exp == NULL) return;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    if (l_exp->queue != NULL) {
        sem_n_l_exp(l_exp->queue);
    }
    sem_n_exp(l_exp->tete);
    balise_fermante(sortie_semantique, __FUNCTION__);
}

n_type *sem_n_exp(n_exp *exp) {
    if (exp == NULL) return NULL;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    n_type *type = NULL;

    switch (exp->type) {
    case varExp :
        type = sem_varExp(exp);
        break;
    case intExp :
        type = sem_intExp(exp);
        break;
    case opExp :
        type = sem_opExp(exp);
        break;
    case trueExp :
        type = sem_trueExp(exp);
        break;
    case falseExp :
        type = sem_falseExp(exp);
        break;
    case appelExp :
        type = sem_appelExp(exp);
        break;
    case lireExp :
        type = sem_lireExp(exp);
        break;
    }

    balise_fermante(sortie_semantique, __FUNCTION__);
    return type;
}

n_type *sem_varExp(n_exp *exp) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    n_type *type_var = NULL;

    type_var = sem_n_var(exp->u.var);

    if (type_var->type == t_array) {
        ajoute_ligne(ltab, 0, ligne-1, exp->u.var->nom);
        type_var = type_var->arrayof;   // l'expression est du type du tableau
    }
    else {
        ajoute_ligne(load, 0, 0, exp->u.var->nom);
    }
    code[ligne-1].adresse = symboles.tab[dico_indice_var].adresse;
    code[ligne-1].mode = symboles.tab[dico_indice_var].mode;

    balise_fermante(sortie_semantique, __FUNCTION__);
    return type_var;
}

n_type *sem_intExp(n_exp *exp) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    char* entier = malloc(exp->u.entier / 10 + 1);
    sprintf(entier, "%d", exp->u.entier);
    balise_text(sortie_semantique, "val", entier);
    free(entier);

    ajoute_ligne(loadimm, exp->u.entier, 0, NULL);

    balise_fermante(sortie_semantique, __FUNCTION__);
    return cree_n_type_int();
}

n_type *sem_opExp(n_exp *exp) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    n_type *type_exp = NULL;
    n_type *type_op1 = NULL;
    n_type *type_op2 = NULL;

    type_op1 = sem_n_exp(exp->u.opExp_.op1);
    sem_operation(exp->u.opExp_.op);
    int lignetmp = ligne-1;
    type_op2 = sem_n_exp(exp->u.opExp_.op2);
    ajoute_ligne((c3a_op) exp->u.opExp_.op, lignetmp, ligne-1, NULL);

    assert_types_sont_compatibles(__FUNCTION__,
                                  type_op1, type_op2, &exp->u.opExp_.op);

    operation op = exp->u.opExp_.op;
    if (op == plus   || op == moins ||
        op == fois   || op == divise ||
        op == modulo || op == negatif)
        type_exp = cree_n_type_int();
    else
        type_exp = cree_n_type_bool();

    balise_fermante(sortie_semantique, __FUNCTION__);
    return type_exp;
}

n_type *sem_trueExp(n_exp *exp) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);
    ajoute_ligne(loadimm, 1, 0, NULL);
    balise_fermante(sortie_semantique, __FUNCTION__);
    return cree_n_type_bool();
}

n_type *sem_falseExp(n_exp *exp) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);
    ajoute_ligne(loadimm, 0, 0, NULL);
    balise_fermante(sortie_semantique, __FUNCTION__);
    return cree_n_type_bool();
}

n_type *sem_appelExp(n_exp *exp) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    n_type *type_fonc = NULL;

    type_fonc = sem_n_appel(exp->u.appel);

    balise_fermante(sortie_semantique, __FUNCTION__);
    return type_fonc;
}

n_type *sem_lireExp(n_exp *exp) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);
    ajoute_ligne(lire, 0, 0, NULL);
    balise_fermante(sortie_semantique, __FUNCTION__);
    return cree_n_type_int();
}

void sem_operation(operation op) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);
    operation ops [] = {plus, moins, fois, divise, modulo, egal, diff,
                        inf, sup, infeg, supeg, ou, et, non, negatif};
    char* charOps [] = {"+", "-", "*", "div", "mod", "=", "<>",
                        "<", ">", "<=", ">=", "or", "and", "not", "-"};
    int nbOps = sizeof(ops) / sizeof(operation);
    int i;
    for (i = 0; i < nbOps && ops[i] != op; ++i);
    if (i < nbOps) {
        balise_text(sortie_semantique, "operation", charOps[i]);
    }   // else shouldn't happen

    balise_fermante(sortie_semantique, __FUNCTION__);
}

n_type *sem_n_var(n_var *var) {
    if (var == NULL) return NULL;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    n_type *type_var;
    n_type *type_indice;

    dico_indice_var = recherche_symbole(var->nom);
    if (dico_indice_var == -1) {
        char *msg_init = "err : undeclared variable %s";
        char *real_msg = malloc(strlen(msg_init) + strlen(var->nom));
        sprintf(real_msg, msg_init, var->nom);
        erreur(__FUNCTION__, real_msg);
    }

    type_var = symboles.tab[dico_indice_var].type;

    if (var->type == indicee) {
        if (type_var->type != t_array) {
            char *msg_init = "err : variable %s is not an array";
            char *real_msg = malloc(strlen(msg_init) + strlen(var->nom));
            sprintf(real_msg, msg_init, var->nom);
            erreur(__FUNCTION__, real_msg);
        }
        /*type_var = type_var->arrayof;*/
        balise_text(sortie_semantique, "var_indicee", var->nom);
        type_indice = sem_n_exp(var->indice);
        if (type_indice == NULL || type_indice->type != t_int) {
            erreur(__FUNCTION__, "err : index of array must be an int");
        }
    } else {
        balise_text(sortie_semantique, "var_simple", var->nom);
    }

    balise_fermante(sortie_semantique, __FUNCTION__);
    return type_var;
}

void sem_n_l_dec(n_l_dec *l_dec) {
    if (l_dec == NULL) return;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    sem_n_l_dec(l_dec->queue);
    sem_n_dec(l_dec->tete);

    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_n_dec(n_dec *dec) {
    if (dec == NULL) return;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    if (recherche_symbole(dec->nom) != -1) {
        char *msg_init = "err : variable %s declared previously";
        char *real_msg = malloc(strlen(msg_init) + strlen(dec->nom));
        sprintf(real_msg, msg_init, dec->nom);
        erreur(__FUNCTION__, real_msg);
    }

    balise_text(sortie_semantique, "nom", dec->nom);
    sem_n_type(dec->type);

    if (dec->type->type == t_array) {
        ajoute_tableau(dec->nom, dec->type, dec->type->debut, dec->type->fin);
    } else {
        ajoute_variable(dec->nom, dec->type);
    }

    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_n_type(n_type *type) {
    if (type == NULL) return;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    switch (type->type) {
    case t_int :
        balise_text(sortie_semantique, "type_simple", "int");
        break;
    case t_bool :
        balise_text(sortie_semantique, "type_simple", "bool");
        break;
    case t_array :
        sem_typeArray(type);
        break;
    }
    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_typeArray(n_type *type) {
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    if (type->debut >= type->fin) {
        erreur(__FUNCTION__, "err : lower boundary greater than higher");
    }

    char *debut = malloc(type->debut / 10 + 1);
    sprintf(debut, "%d", type->debut);
    char *fin = malloc(type->fin / 10 + 1);
    sprintf(fin, "%d", type->fin);
    balise_text(sortie_semantique, "debut", debut);
    balise_text(sortie_semantique, "fin", fin);
    free(debut);
    free(fin);
    sem_n_type(type->arrayof);

    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_n_l_fun_dec(n_l_fun_dec *l_fun_dec) {
    if (l_fun_dec == NULL) return;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    sem_n_l_fun_dec(l_fun_dec->queue);
    sem_n_fun_dec(l_fun_dec->tete);

    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_n_fun_dec(n_fun_dec *fun_dec) {
    if (fun_dec == NULL) return;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    ajoute_fonction(fun_dec->nom, fun_dec->type, fun_dec->param);

    /* nombre a empiler quand on entre dans une fonction,
     * et a depiler quand on sort */
    int nb_pile = 0;
    n_l_dec *l_dec;
    for (l_dec = fun_dec->variables; l_dec != NULL; l_dec = l_dec->queue) {
        ++nb_pile;
    }

    entree_fonction();
    ajoute_ligne(entree, nb_pile, 0, fun_dec->nom);
    ajoute_variable(fun_dec->nom, fun_dec->type);

    sem_n_l_dec(fun_dec->param);
    sem_n_type(fun_dec->type);
    sem_n_l_dec(fun_dec->variables);
    sem_n_instr(fun_dec->corps);

    for (l_dec = fun_dec->param; l_dec != NULL; l_dec = l_dec->queue) {
        ++nb_pile;
    }

    sortie_fonction();
    ajoute_ligne(sortie, nb_pile, 0, fun_dec->nom);

    balise_fermante(sortie_semantique, __FUNCTION__);
}

void sem_n_prog(n_prog *prog) {
    if (prog == NULL) return;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    int lignetmp = ligne;
    ajoute_ligne(jump, 0, 0, NULL);
    sem_n_l_dec(prog->variables);
    sem_n_l_fun_dec(prog->fonctions);
    if (lignetmp == ligne-1) {
        initialiser_code();
    }
    else {
        code[lignetmp].arg2 = ligne;
    }
    sem_n_instr(prog->corps);

    balise_fermante(sortie_semantique, __FUNCTION__);
}

n_type *sem_n_appel(n_appel *appel) {
    if (appel == NULL) return NULL;
    balise_ouvrante(sortie_semantique, __FUNCTION__);

    n_type *type_fonc = NULL;

    dico_indice_var = recherche_symbole(appel->fonction);
    if (dico_indice_var == -1) {
        char *msg_init = "err : undeclared function %s";
        char *real_msg = malloc(strlen(msg_init) + strlen(appel->fonction));
        sprintf(real_msg, msg_init, appel->fonction);
        erreur(__FUNCTION__, real_msg);
    }

    type_fonc = symboles.tab[dico_indice_var].type;
    balise_text(sortie_semantique, "fonction", appel->fonction);
    //sem_n_l_exp(appel->args);

    sem_parametres(appel->args, symboles.tab[dico_indice_var].param);
    ajoute_ligne(call, 0, 0, appel->fonction);

    balise_fermante(sortie_semantique, __FUNCTION__);
    return type_fonc;
}

void sem_parametres(n_l_exp *args, n_l_dec *params) {
    if (args == NULL || params == NULL) {
        if (args != NULL || params != NULL) {
            erreur(__FUNCTION__, "err: wrong number of arg");
        }
        return;
    }
    sem_parametres(args->queue, params->queue);
    assert_types_sont_compatibles(__FUNCTION__, sem_n_exp(args->tete),
                                                params->tete->type, NULL);
    ajoute_ligne(param, ligne-1, 0, NULL);
}

void assert_types_sont_compatibles(const char *call_from,
                                   n_type *t1, n_type *t2, operation *op) {
    if (op == NULL) {   // affectation
        if (t1 == NULL || t2 == NULL) {
            erreur(call_from, "err : unable to get a (both) type(s) "
                                 "of affectation");
        }
        else if (t1->type != t2->type) {
            erreur(call_from, "err : uncompatible types for affectation");
        }
        return; // yay
    }

    // expression op
    if (t1 == NULL && t2 == NULL) {
        erreur(call_from, "err : failed to get both types");
    }
    else if (*op == non) {
        if ((t1 == NULL && t2->type != t_bool) ||
            (t2 == NULL && t1->type != t_bool)) {
            erreur(call_from, "err : not can only apply on a single boolean exp");
        }
    }
    else if (*op == negatif) {
        if ((t1 == NULL && t2->type != t_int) ||
            (t2 == NULL && t1->type != t_int)) {
            erreur(call_from, "err : negation can only apply on a single int exp");
        }
    }
    else if (t1 == NULL || t2 == NULL) {
        erreur(call_from, "err: failed to get one type of op");
    }
    else if (t1->type == t_array || t2->type == t_array) {
        erreur(call_from, "err : no operation possible with array(s)");
    }
    else if (t1->type != t2->type) {
        erreur(call_from, "err : non compatibles types");
    }
    return; // yay
}
