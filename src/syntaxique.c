/*
 * @File syntax.c
 *
 * @Date 14-02-2013
 *
 * @Brief Analyseur syntaxique
 *
 * ProgramPascal          -> PROGRAM ID ';' [ VariableDefinitionList ] { ProcFunDefinition } CompoundStatement '.'
 * BlockBody              -> [ VariableDefinitionList ] CompoundStatement
 * VariableDefinitionList -> VAR VariableDefinition { VariableDefinition }
 * VariableDefinition     -> VariableGroup ';'
 * VariableGroup          -> ID { ',' ID } ':' Type
 * Type                   -> INTEGER | BOOLEAN | ARRAY '[' NUMERAL DOTDOT NUMERAL ']' OF Type
 * ProcFunDefinition      -> ProcedureDefinition | FunctionDefinition
 * ProcedureDefinition    -> PROCEDURE ID [ '(' ArgDefinitionList ')' ] ';' BlockBody ';'
 * FunctionDefinition     -> FUNCTION ID '(' ArgDefinitionList ')' ':' Type ';' BlockBody ';'
 * ArgDefinitionList      -> ArgDefinition { ';' ArgDefinition }
 * ArgDefinition          -> [ VAR ] VariableGroup
 * Statement              -> StatementId | IfStatement | WhileStatement | CompoundStatement | WriteStatement | Empty
 * StatementId            -> ID ProcedureCall | AssignmentStatement
 * AssignmentStatement    -> VariableAccess AFFECT Expression
 * ProcedureCall          -> '(' [ ParameterList ] ')'
 * FunctionCall           -> '(' [ ParameterList ] ')'
 * ParameterList          -> Expression { ',' Expression }
 * IfStatement            -> IF Expression THEN Statement [ ELSE Statement ]
 * WhileStatement         -> WHILE Expression DO Statement
 * CompoundStatement      -> BEGIN { Statement ';' } END
 * WriteStatement         -> WRITE '(' Expression ')'
 * Expression             -> AndExpression [ OR AndExpression ]
 * AndExpression          -> RelationExpression [ AND RelationExpression ]
 * RelationExpression     -> SimpleExpression [ Relation SimpleExpression ]
 * Relation               -> '<' || '=' || '>' || INFEG || DIFF || SUPEG
 * SimpleExpression       -> Factor [ AddingOperator SimpleExpression ]
 * AddingOperator         -> '+' | '-'
 * Factor                 -> [ UnaryRelation ] Atom [ MultiplyingOperator Factor ]
 * UnaryRelation          -> '-' | NOT
 * MultiplyingOperator    -> '*' | DIV | MOD
 * Atom                   -> AtomId | READ '(' ')' | NUMERAL | TRUE | FALSE | '(' Expression ')'
 * AtomId                 -> ID FunctionCall | ID VariableAccess
 * VariableAccess         -> '[' Expression ']' | Empty
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // strcpy()
#include <stdarg.h>      // va_list, va_start, va_arg, va_end

#include "lexical.h"     // yytext()
#include "lexemes.h"
#include "syntaxique.h"
#include "premiers.h"

#define DEBUG

void balise_ouvrante (FILE *f, const char* func) {
#ifdef DEBUG
    fprintf(f, "<%s>\n", func);
#endif /* DEBUG */
}

void balise_fermante (FILE *f, const char* func) {
#ifdef DEBUG
    fprintf(f, "</%s>\n", func);
#endif /* DEBUG */
}

void balise_text(FILE *f, const char* balise, const char *text) {
#ifdef DEBUG
    fprintf(f, "<%s>%s</%s>\n", balise, text, balise);
#endif /* DEBUG */
}

// ProgramPascal -> PROGRAM ID ';' [ VariableDefinitionList ] { ProcFunDefinition } CompoundStatement '.'
n_prog *ProgramPascal (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_prog      *S0 = NULL;
    n_l_dec     *S4 = NULL;
    n_l_fun_dec *S5 = NULL;
    n_instr     *S6 = NULL;

    if (!TEST_CC(PROGRAM)) {
        erreur(__FUNCTION__, "expected 'program' instead of '%s'",
                             yytext);
    }
    MANGER_CC;
    if (!TEST_CC(ID)) {
        erreur(__FUNCTION__, "expected identifier after 'program' "
                             "instead of '%s'", yytext);
    }
    balise_text(sortie_syntaxique, "id", yytext);
    MANGER_CC;
    if (!TEST_CC(';')) {
        erreur(__FUNCTION__, "exprected ';' after program name instead of '%s'",
                             yytext);
    }
    MANGER_CC;
    if (TEST_CC(VAR)) {
        S4 = VariableDefinitionList();
    }
    while (TEST_CC(PROCEDURE) || TEST_CC(FUNCTION)) {
        n_fun_dec *tmp_fun_dec = ProcFunDefinition();
        S5 = cree_n_l_fun_dec(tmp_fun_dec, S5);
    }
    if (!TEST_CC(BEGIN)) {
        erreur(__FUNCTION__, "expected 'begin' instead of '%s'", yytext);
    }
    S6 = CompoundStatement();
    if (!TEST_CC('.')) {
        erreur(__FUNCTION__, "expected '.' at end of program before %s",
                              yytext);
    }
    S0 = cree_n_prog(S4, S5, S6);
    MANGER_CC;
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// BlockBody -> [ VariableDefinitionList ] CompoundStatement
n_prog *BlockBody (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_prog *S0 = NULL;
    n_l_dec     *S1 = NULL;
    n_instr     *S2 = NULL;

    if (TEST_CC(VAR)) {
        S1 = VariableDefinitionList();
    }
    if (TEST_CC(!BEGIN)) {
        erreur(__FUNCTION__, "expected 'begin' instead of '%s'", yytext);
    }
    S2 = CompoundStatement();
    S0 = cree_n_prog(S1, NULL, S2);
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// VariableDefinitionList -> VAR VariableDefinition { VariableDefinition }
n_l_dec *VariableDefinitionList (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_l_dec *S0 = NULL;
    n_l_dec *S2 = NULL;
    n_l_dec *S3 = NULL;

    if (!TEST_CC(VAR)) {
        erreur(__FUNCTION__, "expected 'var' instead of '%s'", yytext);
    }
    MANGER_CC;
    if (!TEST_CC(ID)) {
        erreur(__FUNCTION__, "expected identifier instead of '%s' "
                             "when defining variables", yytext);
    }
    S2 = VariableDefinition();
    S0 = S2;
    while (TEST_CC(ID)) {
        S3 = VariableDefinition();
        S0 = fusionne_n_l_dec(S3, S0);
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// VariableDefinition -> VariableGroup ';'
n_l_dec *VariableDefinition (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_l_dec *S0 = NULL;

    if (!TEST_CC(ID)) {
        erreur(__FUNCTION__, "expected identifier instead of '%s' "
                             "when defining variables", yytext);
    }
    S0 = VariableGroup();
    if (!TEST_CC(';')) {
        erreur(__FUNCTION__, "expected ';' instead of '%s' "
                             "after variable(s) definition", yytext);
    }
    MANGER_CC;
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// VariableGroup -> ID { ',' ID } ':' Type
n_l_dec *VariableGroup (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_l_dec *S0 = NULL;
    n_dec   *S1 = NULL;
    n_dec   *S3 = NULL;
    n_type  *S5 = NULL;

    if (!TEST_CC(ID)) {
        erreur(__FUNCTION__, "expected identifier instead of '%s' "
                             "when defining variables", yytext);
    }
    balise_text(sortie_syntaxique, "id", yytext);
    char *id = malloc(strlen(yytext));
    strcpy(id, yytext);
    S1 = cree_n_dec_var(id, NULL);  // type specifie apres
    S0 = cree_n_l_dec(S1, S0);
    MANGER_CC;
    while (TEST_CC(',')) {
        MANGER_CC;
        if (!TEST_CC(ID)) {
            erreur(__FUNCTION__, "expected identifier instead of '%s' after ','",
                                 yytext);
        }
        balise_text(sortie_syntaxique, "id", yytext);
        id = malloc(strlen(yytext));
        strcpy(id, yytext);
        S3 = cree_n_dec_var(id, NULL);  // type specifie apres
        S0 = cree_n_l_dec(S3, S0);
        MANGER_CC;
    }
    if (!TEST_CC(':')) {
        erreur(__FUNCTION__, "expected ':' instead of '%s' "
                             "between idententifiers and type", yytext);
    }
    MANGER_CC;
    if (!(TEST_CC(INTEGER) || TEST_CC(BOOLEAN) || TEST_CC(ARRAY))) {
        erreur(__FUNCTION__, "unknown type %s, available types are : "
                             "'integer', 'boolean' and 'array'", yytext);
    }
    S5 = Type();
    n_l_dec *cur_n_l_dec = S0;
    while (cur_n_l_dec != NULL) {
        cur_n_l_dec->tete->type = S5;
        cur_n_l_dec = cur_n_l_dec->queue;
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// Type -> INTEGER | BOOLEAN | ARRAY '[' NUMERAL DOTDOT NUMERAL ']' OF Type
n_type *Type (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_type *S0 = NULL;
    n_type *S10 = NULL;
    int S5, S7;

    if (TEST_CC(INTEGER)) {
        balise_text(sortie_syntaxique, "typesimple", yytext);
        S0 = cree_n_type_int();
        MANGER_CC;
        balise_fermante(sortie_syntaxique, __FUNCTION__);
        return S0;
    } else if (TEST_CC(BOOLEAN)) {
        balise_text(sortie_syntaxique, "typesimple", yytext);
        S0 = cree_n_type_bool();
        MANGER_CC;
        balise_fermante(sortie_syntaxique, __FUNCTION__);
        return S0;
    }

    if (!TEST_CC(ARRAY)) {
        erreur(__FUNCTION__, "unknown type %s, available types are : "
                             "'integer', 'boolean' and 'array'", yytext);
    }
    MANGER_CC;
    if (!TEST_CC('[')) {
        erreur(__FUNCTION__, "expected '[' instead of '%s' "
                             "when declaring array", yytext);
    }
    MANGER_CC;
    if (!TEST_CC(NUMERAL)) {
        erreur(__FUNCTION__, "expected a numeral instead of '%s' "
                             "in array's interval declaration", yytext);
    }
    S5 = atoi(yytext);
    MANGER_CC;
    if (!TEST_CC(DOTDOT)) {
        erreur(__FUNCTION__, "expected '..' instead of '%s' "
                             "in array's interval declaration", yytext);
    }
    MANGER_CC;
    if (!TEST_CC(NUMERAL)) {
        erreur(__FUNCTION__, "expected a numeral instead of '%s' "
                             "in array's interval declaration", yytext);
    }
    S7 = atoi(yytext);
    MANGER_CC;
    if (!TEST_CC(']')) {
        erreur(__FUNCTION__, "expected '[' instead of '%s' "
                             "when declaring array", yytext);
    }
    MANGER_CC;
    if (!TEST_CC(OF)) {
        erreur(__FUNCTION__, "expected 'of' instead of '%s' "
                             "when declaring array, to specify type of elems",
                             yytext);
    }
    MANGER_CC;
    if (!(TEST_CC(INTEGER) || TEST_CC(BOOLEAN) || TEST_CC(ARRAY))) {
        erreur(__FUNCTION__, "unknown type '%s' for array elements, "
                             "available types are : "
                             "'integer, 'boolean' and 'array'", yytext);
    }
    S10 = Type();
    S0 = cree_n_type_array(S10, S5, S7);
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// ProcFunDefinition -> ProcedureDefinition | FunctionDefinition
n_fun_dec *ProcFunDefinition (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_fun_dec *S0 = NULL;

    if (TEST_CC(PROCEDURE)) {
        S0 = ProcedureDefinition();
        balise_fermante(sortie_syntaxique, __FUNCTION__);
        return S0;
    }
    if (TEST_CC(FUNCTION)) {
        S0 = FunctionDefinition();
        balise_fermante(sortie_syntaxique, __FUNCTION__);
        return S0;
    }
    erreur(__FUNCTION__, "expected 'procedure' or 'function' instead of '%s'",
                         yytext);
    return NULL;
}

// ProcedureDefinition -> PROCEDURE ID [ '(' ArgDefinitionList ')' ] ';' BlockBody ';'
n_fun_dec *ProcedureDefinition (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_fun_dec *S0 = NULL;
    char      *S2 = NULL;
    n_l_dec   *S4 = NULL;
    n_prog    *S7 = NULL;

    if (!TEST_CC(PROCEDURE)) {
        erreur(__FUNCTION__, "expected 'procedure' instead of '%s'",
                             yytext);
    }
    MANGER_CC;
    if (!TEST_CC(ID)) {
        erreur(__FUNCTION__, "expected identifier instead of '%s' "
                             "in procedure definition", yytext);
    }
    S2 = malloc(strlen(yytext));
    strcpy(S2, yytext);
    balise_text(sortie_syntaxique, "id", yytext);
    MANGER_CC;
    if (TEST_CC('(')) {
        MANGER_CC;
        if (!(TEST_CC(VAR) || TEST_CC(ID))) {
            erreur(__FUNCTION__, "expected 'var' or identifier instead of '%s' "
                                 "inside procedure's argument definition",
                                 yytext);
        }
        S4 = ArgDefinitionList();
        if (!TEST_CC(')')) {
            erreur(__FUNCTION__, "expected ')' instead of '%s' after "
                                 "declaring procedure's arguments", yytext);
        }
    }
    if (!TEST_CC(';')) {
        erreur(__FUNCTION__, "expected ';' instead of '%s' after procedure "
                             "header definition", yytext);
    }
    MANGER_CC;
    if (!(TEST_CC(VAR) || TEST_CC(BEGIN))) {
        erreur(__FUNCTION__, "expected 'var' or 'begin' instead of '%s' "
                              "in body of procedure", yytext);
    }
    S7 = BlockBody();
    if (!TEST_CC(';')) {
        erreur(__FUNCTION__, "expected ';' instead of '%s' "
                              "at end of procedure definition", yytext);
    }
    S0 = cree_n_dec_fonc(S2, NULL, S4, S7->variables, S7->corps);
    MANGER_CC;
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// FunctionDefinition -> FUNCTION ID '(' ArgDefinitionList ')' ':' Type ';' BlockBody ';'
n_fun_dec *FunctionDefinition (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_fun_dec *S0 = NULL;
    char      *S2 = NULL;
    n_l_dec   *S4 = NULL;
    n_type    *S7 = NULL;
    n_prog    *S9 = NULL;

    if (!TEST_CC(FUNCTION)) {
        erreur(__FUNCTION__, "expected 'function' instead of '%s'");
    }
    MANGER_CC;
    if (!TEST_CC(ID)) {
        erreur(__FUNCTION__, "expected identifier instead of '%s' "
                             "in function definition", yytext);
    }
    S2 = malloc(strlen(yytext));
    strcpy(S2, yytext);
    balise_text(sortie_syntaxique, "id", yytext);
    MANGER_CC;
    if (!TEST_CC('(')) {
        erreur(__FUNCTION__, "expected '(' instead of '%s' in function header "
                             "to declare it's arguments",
                             yytext);
    }
    MANGER_CC;
    if (TEST_CC(VAR) || TEST_CC(ID)) {
        S4 = ArgDefinitionList();
    }
    if (!TEST_CC(')')) {
        erreur(__FUNCTION__, "expected ')' instead of '%s' after "
                             "declaring function's arguments", yytext);
    }
    MANGER_CC;
    if (!TEST_CC(':')) {
        erreur(__FUNCTION__, "expected ';' instead of '%s' in header "
                             "to declare it's return type", yytext);
    }
    MANGER_CC;
    if (!(TEST_CC(INTEGER) || TEST_CC(BOOLEAN) || TEST_CC(ARRAY))) {
        erreur(__FUNCTION__, "unknown type '%s' for function return type",
                             yytext);
    }
    S7 = Type();
    if (!TEST_CC(';')) {
        erreur(__FUNCTION__, "expected ';' instead of '%s' after function "
                             "header definition", yytext);
    }
    MANGER_CC;
    if (!(TEST_CC(VAR) || TEST_CC(BEGIN))) {
        erreur(__FUNCTION__, "expected 'var' or 'begin' instead of '%s' "
                              "in body of definition", yytext);
    }
    S9 = BlockBody();
    if (!TEST_CC(';')) {
        erreur(__FUNCTION__, "expected ';' instead of '%s' "
                              "at end of function definition", yytext);
    }
    S0 = cree_n_dec_fonc(S2, S7, S4, S9->variables, S9->corps);
    MANGER_CC;
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// ArgDefinitionList -> ArgDefinition { ';' ArgDefinition }
n_l_dec *ArgDefinitionList (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_l_dec *S0 = NULL;
    n_l_dec *S1 = NULL;
    n_l_dec *S3 = NULL;

    if (!(TEST_CC(VAR) || TEST_CC(ID))) {
        erreur(__FUNCTION__, "expected 'var' or identifier instead of '%s' "
                             "in argument definition list", yytext);
    }
    S1 = ArgDefinition();
    S0 = S1;
    while (TEST_CC(';')) {
        MANGER_CC;
        if (!(TEST_CC(VAR) || TEST_CC(ID))) {
            erreur(__FUNCTION__, "expected 'var' or identifier instead of '%s'"
                                 "in argument definition list", yytext);
        }
        S3 = ArgDefinition();
        S0 = fusionne_n_l_dec(S0, S3);
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// ArgDefinition -> [ VAR ] VariableGroup
n_l_dec *ArgDefinition (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_l_dec *S0 = NULL;

    if (TEST_CC(VAR)) {
        MANGER_CC;
    }
    if (!TEST_CC(ID)) {
        erreur(__FUNCTION__, "expected identifier instead of '%s' when "
                             "defining argument", yytext);
    }
    S0 = VariableGroup();
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// Statement -> StatementId | IfStatement | WhileStatement | CompoundStatement | WriteStatement | Empty
n_instr *Statement (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_instr *S0 = NULL;

    if (TEST_CC(ID)) {
        S0 = StatementId();
        balise_fermante(sortie_syntaxique, __FUNCTION__);
        return S0;
    }
    if (TEST_CC(IF)) {
        S0 = IfStatement();
        balise_fermante(sortie_syntaxique, __FUNCTION__);
        return S0;
    }
    if (TEST_CC(WHILE)) {
        S0 = WhileStatement();
        balise_fermante(sortie_syntaxique, __FUNCTION__);
        return S0;
    }
    if (TEST_CC(BEGIN)) {
        S0 = CompoundStatement();
        balise_fermante(sortie_syntaxique, __FUNCTION__);
        return S0;
    }
    if (TEST_CC(WRITE)) {
        S0 = WriteStatement();
        balise_fermante(sortie_syntaxique, __FUNCTION__);
        return S0;
    }
    S0 = cree_n_instr_vide();
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// StatementId -> ID ProcedureCall | AssignmentStatement
n_instr *StatementId (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_instr *S0 = NULL;
    n_appel *S4 = NULL;
    n_instr *S5 = NULL;

    if (!(TEST_CC(ID))) {
        erreur(__FUNCTION__, "expected identifier instead of '%s' "
                             "in a procedure call or assignment statement",
                             yytext);
    }
    balise_text(sortie_syntaxique, "id", yytext);
    char *id = malloc(strlen(yytext));
    strcpy(id, yytext);
    MANGER_CC;
    if (TEST_CC('(')) {
        S4 = ProcedureCall();
        S4->fonction = id;
        S0 = cree_n_instr_appel(S4);
    } else if (TEST_CC('[') || TEST_CC(AFFECT)) {
        S5 = AssignmentStatement();
        // au retour de AssignmentStatement(), on sait qu'on a une instr
        // de type affectInstr
        S5->u.affecte_.var->nom = id;
        S0 = S5;
    } else {
        erreur(__FUNCTION__, "expected '(', '[' or ':=' instead of '%s' "
                             "after %s", id, yytext);
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// AssignmentStatement -> VariableAccess AFFECT Expression
n_instr *AssignmentStatement (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_instr *S0 = NULL;
    n_var   *S1 = NULL;
    n_exp   *S3 = NULL;

    if (!(TEST_CC('[') || TEST_CC(AFFECT))) {
        erreur(__FUNCTION__, "expected '[' or ':=' instead of '%s' "
                             "in assignment statement", yytext);
    }
    S1 = VariableAccess();
    if (!TEST_CC(AFFECT)) {
        erreur(__FUNCTION__, "expected ':=' instead of '%s' "
                             "in assignment statement", yytext);
    }
    MANGER_CC;
    if (!TEST_PREM_EXPR) {
        erreur(__FUNCTION__, "expected an expression's premier instead of '%s' "
                             "in assignment statement", yytext);
    }
    S3 = Expression();
    S0 = cree_n_instr_affect(S1, S3);
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// ProcedureCall -> '(' [ ParameterList ] ')'
n_appel *ProcedureCall (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_appel *S0 = NULL;
    n_l_exp *S2 = NULL;

    if (!TEST_CC('(')) {
        erreur(__FUNCTION__, "expected '(' instead of '%s' to define the "
                             "procedure's parameter when calling",
                             yytext);
    }
    MANGER_CC;
    if (TEST_PREM_EXPR) {
        S2 = ParameterList();
    }
    if (!TEST_CC(')')) {
        erreur(__FUNCTION__, "expected ')' instead of '%s' after "
                             "procedure's parameters when calling", yytext);
    }
    MANGER_CC;
    S0 = cree_n_appel(NULL, S2);    // id mis a de la remontee
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// FunctionCall -> '(' [ ParameterList ] ')'
n_appel *FunctionCall (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_appel *S0 = NULL;
    n_l_exp *S2 = NULL;

    if (!TEST_CC('(')) {
        erreur(__FUNCTION__, "expected '(' instead of '%s' to define the "
                             "function's parameter when calling",
                             yytext);
    }
    MANGER_CC;
    if (TEST_PREM_EXPR) {
        S2 = ParameterList();
    }
    if (!TEST_CC(')')) {
        erreur(__FUNCTION__, "expected ')' instead of '%s' after "
                             "function's parameters when calling", yytext);
    }
    MANGER_CC;
    S0 = cree_n_appel(NULL, S2);
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// ParameterList -> Expression { ',' Expression }
n_l_exp *ParameterList (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_l_exp *S0 = NULL;
    n_exp   *S1 = NULL;
    n_exp   *S3 = NULL;

    // liste remplie a l'envers
    if (!TEST_PREM_EXPR) {
        erreur(__FUNCTION__, "expected an expression's premier instead of '%s' "
                             "in parameter list", yytext);
    }
    S1 = Expression();
    S0 = cree_n_l_exp(S1, S0);
    while (TEST_CC(',')) {
        MANGER_CC;
        if (!TEST_PREM_EXPR) {
            erreur(__FUNCTION__, "expected an expression's premier instead "
                                 "of '%s' in parameter list", yytext);
        }
        S3 = Expression();
        S0 = cree_n_l_exp(S3, S0);
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// IfStatement -> IF Expression THEN Statement [ ELSE Statement ]
n_instr *IfStatement (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_instr *S0 = NULL;
    n_exp   *S2 = NULL;
    n_instr *S4 = NULL;
    n_instr *S6 = NULL;

    if (!TEST_CC(IF)) {
        erreur(__FUNCTION__, "expected 'if' instead of '%s' in if statement",
                             yytext);
    }
    MANGER_CC;
    if (!TEST_PREM_EXPR) {
        erreur(__FUNCTION__, "expected an expression's premier instead of '%s' "
                             "in if condition", yytext);
    }
    S2 = Expression();
    if (!TEST_CC(THEN)) {
        erreur(__FUNCTION__, "expected 'then' after if condition", yytext);
    }
    MANGER_CC;
    if (!TEST_PREM_STMT) {
        erreur(__FUNCTION__, "expected a statement's premier instead of '%s' "
                             "in 'if' block", yytext);
    }
    S4 = Statement();
    if (TEST_CC(ELSE)) {
        MANGER_CC;
        if (!TEST_PREM_STMT) {
            erreur(__FUNCTION__, "expected a statement's premier instead "
                                 "of '%s' in 'then' block", yytext);
        }
        S6 = Statement();
    }
    S0 = cree_n_instr_si(S2, S4, S6);
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// WhileStatement -> WHILE Expression DO Statement
n_instr *WhileStatement (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_instr *S0 = NULL;
    n_exp   *S1 = NULL;
    n_instr *S3 = NULL;

    if (!TEST_CC(WHILE)) {
        erreur(__FUNCTION__, "expected 'while' instead of '%s' in 'while' "
                             "statement", yytext);
    }
    MANGER_CC;
    if (!TEST_PREM_EXPR) {
        erreur(__FUNCTION__, "expected an expression's premier instead of '%s' "
                             "in while condition", yytext);
    }
    S1 = Expression();
    if (!TEST_CC(DO)) {
        erreur(__FUNCTION__, "exprected 'do' instead of '%s', in while "
                             "statement", yytext);
    }
    MANGER_CC;
    if (!TEST_PREM_STMT) {
        erreur(__FUNCTION__, "expected a statement's premier instead of '%s' "
                             "in 'while' block", yytext);
    }
    S3 = Statement();
    S0 = cree_n_instr_tantque(S1, S3);
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// CompoundStatement -> BEGIN { Statement ';' } END
n_instr *CompoundStatement (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_instr *S0 = NULL;
    n_instr *S1 = NULL;
    n_l_instr *n_l_instr = NULL;

    if (!TEST_CC(BEGIN)) {
        erreur(__FUNCTION__, "expected 'begin' instead of '%s' in compound "
                             "statement", yytext);
    }
    MANGER_CC;
    while (TEST_PREM_STMT) {
        S1 = Statement();
        n_l_instr = cree_n_l_instr(S1, n_l_instr);
        if (!TEST_CC(';')) {
            erreur(__FUNCTION__, "expected ';' instead of '%s' after statement "
                                 "in compound statement", yytext);
        }
        MANGER_CC;
    }
    if (!TEST_CC(END)) {
        erreur(__FUNCTION__, "expected 'end' instead of '%s' at end of "
                             "compound statement", yytext);
    }
    S0 = cree_n_instr_bloc(n_l_instr);
    MANGER_CC;
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// WriteStatement -> WRITE '(' Expression ')'
n_instr *WriteStatement (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_instr *S0 = NULL;
    n_exp   *S3 = NULL;

    if (!TEST_CC(WRITE)) {
        erreur(__FUNCTION__, "expected 'write' instead of '%s' in write "
                             "statement", yytext);
    }
    MANGER_CC;
    if (!TEST_CC('(')) {
        erreur(__FUNCTION__, "expected '(' instead of '%s' for write argument",
                             yytext);
    }
    MANGER_CC;
    if (!TEST_PREM_EXPR) {
        erreur(__FUNCTION__, "expected an expression's premier instead of '%s' "
                             "for write argument", yytext);
    }
    S3 = Expression();
    if (!TEST_CC(')')) {
        erreur(__FUNCTION__, "expected ')' instead of '%s' for write argument",
                             yytext);
    }
    S0 = cree_n_instr_ecrire(S3);
    MANGER_CC;
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// Expression -> AndExpression [ OR AndExpression ]
n_exp *Expression (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_exp *S0 = NULL;
    n_exp *S1 = NULL;
    n_exp *S3 = NULL;

    if (!TEST_PREM_EXPR) {
        erreur(__FUNCTION__, "expected an expression's premier instead of '%s' "
                             "in expression", yytext);
    }
    S1 = AndExpression();
    if (TEST_CC(OR)) {
        MANGER_CC;
        if (!TEST_PREM_EXPR) {
            erreur(__FUNCTION__, "expected an expression's premier instead of "
                                 "%s after or", yytext);
        }
        S3 = AndExpression();
        S0 = cree_n_exp_op(ou, S1, S3);
    } else {
        S0 = S1;
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// AndExpression -> RelationExpression [ AND RelationExpression ]
n_exp *AndExpression (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_exp *S0 = NULL;
    n_exp *S1 = NULL;
    n_exp *S3 = NULL;

    if (!TEST_PREM_EXPR) {
        erreur(__FUNCTION__, "expected an expression's premier instead of '%s' "
                             "in expression", yytext);
    }
    S1 = RelationExpression();
    if (TEST_CC(AND)) {
        MANGER_CC;
        if (!TEST_PREM_EXPR) {
            erreur(__FUNCTION__, "expected an expression's premier instead of "
                                 "%s after and", yytext);
        }
        S3 = RelationExpression();
        S0 = cree_n_exp_op(et, S1, S3);
    } else {
        S0 = S1;
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// RelationExpression -> SimpleExpression [ Relation SimpleExpression ]
n_exp *RelationExpression (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_exp *S0 = NULL;
    n_exp *S1 = NULL;
    operation S2;
    n_exp *S3 = NULL;

    if (!TEST_PREM_EXPR) {
        erreur(__FUNCTION__, "expected an expression's premier instead of '%s' "
                             "in a relation expression", yytext);
    }
    S1 = SimpleExpression();
    if (TEST_CC('<')   || TEST_CC('=')  || TEST_CC('>') ||
        TEST_CC(INFEG) || TEST_CC(DIFF) || TEST_CC(SUPEG)) {
        char *op_text = malloc(strlen(yytext));
        strcpy(op_text, yytext);
        S2 = Relation();
        if (!TEST_PREM_EXPR) {
            erreur(__FUNCTION__, "expected an expression's premier instead of "
                                 "'%s' after relation '%s'", yytext, op_text);
        }
        free (op_text);
        S3 = SimpleExpression();
        S0 = cree_n_exp_op(S2, S1, S3);
    } else {
        S0 = S1;
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// Relation -> '<' || '=' || '>' || INFEG || DIFF || SUPEG
operation Relation (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    int nbOps = 6;
    int rels[] = { '<', '=', '>', INFEG, DIFF, SUPEG };
    operation ops[] = { inf, egal, sup, infeg, diff, supeg };

    int i;
    for (i = 0; i < nbOps; ++i) {
        if (TEST_CC(rels[i])) {
            balise_text(sortie_syntaxique, "REL", yytext);
            MANGER_CC;
            balise_fermante(sortie_syntaxique, __FUNCTION__);
            return ops[i];
        }
    }
    erreur(__FUNCTION__, "unknown operator '%s' for relation, available "
                         "operators are '<', '<=', '=', '<>', '>' and '>='",
                         yytext);
    return 0;
}

// SimpleExpression -> Factor [ AddingOperator SimpleExpression ]
n_exp *SimpleExpression (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_exp *S0 = NULL;
    n_exp *S1 = NULL;
    operation S2;
    n_exp *S3 = NULL;

    if (!TEST_PREM_EXPR) {
        erreur(__FUNCTION__, "expected an expression's premier instead of '%s' "
                             "in a simple expression", yytext);
    }
    S1 = Factor();
    if (TEST_CC('+') || TEST_CC('-')) {
        S2 = AddingOperator();
        if (!TEST_PREM_EXPR) {
            erreur(__FUNCTION__, "expected an expression's premier instead of "
                                 "'%s' after adding operator", yytext);
        }
        S3 = SimpleExpression();
        S0 = cree_n_exp_op(S2, S1, S3);
    } else {
        S0 = S1;
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// AddingOperator -> '+' | '-' || OR
operation AddingOperator (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    operation op;

    if (TEST_CC('+')) {
        op = plus;
    } else if (TEST_CC('-')) {
        op = moins;
    } else {
        erreur(__FUNCTION__, "unknown adding operator '%s', "
                             "expected '+' or '-'", yytext);
        return -1;
    }

    balise_text(sortie_syntaxique, "OP", yytext);
    MANGER_CC;
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return op;
}

// Factor -> [ UnaryRelation ] Atom [ MultiplyingOperator Factor ]
n_exp *Factor (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_exp *S0 = NULL;
    operation S1 = 0;
    int hasUnaryOperation = 0;
    n_exp *S2 = NULL;
    operation S3;
    n_exp *S4 = NULL;

    if (TEST_CC('-') || TEST_CC(NOT)) {
        S1 = UnaryRelation();
        hasUnaryOperation = 1;
    }
    if (!TEST_PREM_ATOM) {
        erreur(__FUNCTION__, "expected an atom's premier instead of '%s' "
                             "in a factor", yytext);
    }
    S2 = Atom();
    if (TEST_CC('*') || TEST_CC(DIV) || TEST_CC(MOD)) {
        S3 = MultiplyingOperator();
        if (!TEST_PREM_EXPR) {
            erreur(__FUNCTION__, "expected an expression's premier instead "
                                 "of '%s' after multiplying operator", yytext);
        }
        S4 = Factor();
        S0 = cree_n_exp_op(S3, S2, S4);
        if (hasUnaryOperation) {
            S0 = cree_n_exp_op(S1, S0, NULL);
        }
    } else {
        if (hasUnaryOperation) {
            S0 = cree_n_exp_op(S1, S2, NULL);
        } else {
            S0 = S2;
        }
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// UnaryRelation -> '-' | NOT
operation UnaryRelation (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    operation op;

    if (TEST_CC('-')) {
        op = negatif;
    } else if (TEST_CC(NOT)) {
        op = non;
    } else {
        erreur(__FUNCTION__, "unknown unary relation '%s', available "
                             "operators are '-' and 'not'", yytext);
        return -1;
    }
    MANGER_CC;
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return op;
}

// MultiplyingOperator -> '*' | DIV | MOD
operation MultiplyingOperator (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    operation op;

    if (TEST_CC('*')) {
        op = fois;
    } else if (TEST_CC(DIV)) {
        op = divise;
    } else if (TEST_CC(MOD)) {
        op = modulo;
    } else {
        erreur(__FUNCTION__, "unknown multiplying operator '%s', available "
                             "operators are '*', 'div' and 'mod'", yytext);
        return -1;
    }
    MANGER_CC;
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return op;
}

// Atom -> AtomId | READ '(' ')' | NUMERAL | TRUE | FALSE | '(' Expression ')'
n_exp *Atom (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_exp *S0 = NULL;

    if (TEST_CC(ID)) {
        S0 = AtomId();
    }
    if (TEST_CC(READ)) {
        MANGER_CC;
        if (!TEST_CC('(')) {
            erreur(__FUNCTION__, "expected '(' instead of '%s' after 'read'",
                                 yytext);
        }
        MANGER_CC;
        if (!TEST_CC(')')) {
            erreur(__FUNCTION__, "expected ')' instead of '%s' after 'read('",
                                 yytext);
        }
        MANGER_CC;
        S0 = cree_n_exp_lire();
    }
    if (TEST_CC(NUMERAL)) {
        balise_text(sortie_syntaxique, "NUMERAL", yytext);
        S0 = cree_n_exp_entier(atoi(yytext));
        MANGER_CC;
    }
    if (TEST_CC(TRUE)) {
        balise_text(sortie_syntaxique, "BOOLEAN", yytext);
        S0 = cree_n_exp_true();
        MANGER_CC;
    }
    if (TEST_CC(FALSE)) {
        balise_text(sortie_syntaxique, "BOOLEAN", yytext);
        S0 = cree_n_exp_false();
        MANGER_CC;
    }
    if (TEST_CC('(')) {
        MANGER_CC;
        if (!TEST_PREM_EXPR) {
            erreur(__FUNCTION__, "expected an expression's premier instead "
                                 "of '%s' after '('", yytext);
        }
        S0 = Expression();
        if (!TEST_CC(')')) {
            erreur(__FUNCTION__, "expected ')' instead of '%s' to close "
                                 "parenthezed expression previously opened",
                                 yytext);
        }
        MANGER_CC;
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// AtomId -> ID FunctionCall | ID VariableAccess
n_exp *AtomId (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_exp   *S0 = NULL;
    n_appel *S6 = NULL;
    n_var   *S8 = NULL;

    if (!TEST_CC(ID)) {
        erreur(__FUNCTION__, "expected identifier instead of '%s' in an AtomId",
                             yytext);
    }
    balise_text(sortie_syntaxique, "id", yytext);
    char *id = malloc(strlen(yytext));
    strcpy(id, yytext);
    MANGER_CC;
    if (TEST_CC('(')) {
        S6 = FunctionCall();
        S6->fonction = id;
        S0 = cree_n_exp_appel(S6);
    } else {
        S8 = VariableAccess();
        S8->nom = id;
        S0 = cree_n_exp_var(S8);
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

// VariableAccess -> '[' Expression ']'
n_var *VariableAccess (void) {
    balise_ouvrante(sortie_syntaxique, __FUNCTION__);

    n_var *S0 = NULL;
    n_exp *S2 = NULL;

    if (TEST_CC('[')) {
        MANGER_CC;
        if (!TEST_PREM_EXPR) {
            erreur(__FUNCTION__, "expected an expression's premier instead of "
                                 "'%s' after '['", yytext);
        }
        S2 = Expression();
        if (!TEST_CC(']')) {
            erreur(__FUNCTION__, "expected ']' after index of array", yytext);
        }
        MANGER_CC;
        S0 = cree_n_var_indicee(NULL, S2);  // nom est fixe dans AtomId
    } else {
        S0 = cree_n_var_simple(NULL);       // nom est fixe dans AtomId
    }
    balise_fermante(sortie_syntaxique, __FUNCTION__);
    return S0;
}

void erreur (const char *func, const char *format, ...) {
    va_list ap;
    fprintf(stderr, "%s\terr: ", func);
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}
