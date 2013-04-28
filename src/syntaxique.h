/*
 * @File
 *
 * @Date 14-02-2013
 *
 * @Brief Declarations des fonctions pour l'analyse syntaxique
 *
 */
#ifndef __SYNTAX_H__
#define __SYNTAX_H__

#include <stdio.h>

#include "syntabs.h"
#include "lexemes.h"

#define TEST_CC(c) (cc == c)
#define MANGER_CC (cc = yylex())

FILE *sortie_syntaxique;

void balise_ouvrante (FILE *f, const char *func);
void balise_fermante (FILE *f, const char *func);
void balise_text     (FILE *f, const char *func, const char *text);

int cc;

n_prog *ProgramPascal (void);
n_prog *BlockBody (void);
n_l_dec *VariableDefinitionList (void);
n_l_dec *VariableDefinition (void);
n_l_dec *VariableGroup (void);
n_type  *Type (void);
n_fun_dec *ProcFunDefinition (void);
n_fun_dec *ProcedureDefinition (void);
n_fun_dec *FunctionDefinition (void);
n_l_dec *ArgDefinitionList (void);
n_l_dec *ArgDefinition (void);
n_instr *Statement (void);
n_instr *StatementId (void);
n_instr *AssignmentStatement (void);
n_appel *ProcedureCall (void);
n_appel *FunctionCall (void);
n_l_exp *ParameterList (void);
n_instr *IfStatement (void);
n_instr *WhileStatement (void);
n_instr *CompoundStatement (void);
n_instr *WriteStatement (void);
n_exp   *Expression (void);
n_exp   *AndExpression (void);
n_exp   *RelationExpression (void);
operation Relation (void);
n_exp   *SimpleExpression (void);
operation AddingOperator (void);
n_exp   *Factor (void);
operation UnaryRelation (void);
operation MultiplyingOperator (void);
n_exp   *Atom (void);
n_exp   *AtomId (void);
n_var   *VariableAccess (void);
void erreur (const char *func, const char *format, ...);

#endif /* __SYNTAX_H__ */
