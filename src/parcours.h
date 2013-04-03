/*
 * @File parcours.h
 *
 * @Date 21-03-2013
 *
 * @Brief Declarations des fonctions pour le parcours
 *        semantique de l'arbre abstrait
 *
 */
#ifndef __SEMANTIQUE_H__
#define __SEMANTIQUE_H__

#include <stdio.h>

#include "syntabs.h"

FILE *sortie_semantique;

void sem_n_l_instr(n_l_instr *l_instr);
void sem_n_instr(n_instr *instr);
void sem_n_l_exp(n_l_exp *l_exp);
n_type *sem_n_exp(n_exp *exp);
void sem_operation(operation op);
n_type *sem_n_var(n_var *var);
void sem_n_l_dec(n_l_dec *l_dec);
void sem_n_dec(n_dec *dec);
void sem_n_type(n_type *type);
void sem_n_l_fun_dec(n_l_fun_dec *l_fun_dec);
void sem_n_fun_dec(n_fun_dec *fun_dec);
void sem_n_prog(n_prog *prog);
n_type *sem_n_appel(n_appel *appel);

/* instructions specifiques */
void sem_incrInst(n_instr *instr);
void sem_affecteInst(n_instr *instr);
void sem_siInst(n_instr *instr);
void sem_tantqueInst(n_instr *instr);
void sem_appelInst(n_instr *instr);
void sem_ecrireInst(n_instr *instr);
void sem_videInst(n_instr *instr);
void sem_blocInst(n_instr *instr);

/* expressions specifiques */
n_type *sem_varExp(n_exp *exp);
n_type *sem_intExp(n_exp *exp);
n_type *sem_opExp(n_exp *exp);
n_type *sem_trueExp(n_exp *exp);
n_type *sem_falseExp(n_exp *exp);
n_type *sem_appelExp(n_exp *exp);
n_type *sem_lireExp(n_exp *exp);

void sem_typeArray(n_type *type);

/* verifie si les types t1 et t2 sont compatibles pour l'operation op
 * si op vaut null, c'est une affectation */
void assert_types_sont_compatibles(const char *call_from,
                                   n_type *t1, n_type *t2, operation *op);

#endif /* __SEMANTIQUE_H__ */
