/*
 * @File syntabs.h
 *
 * @Date 07-03-2013
 *
 * @Brief Declaration des fonctions de remplissage de l'arbre abstrait
 *
 */
#ifndef __SYNTABS__
#define __SYNTABS__

typedef struct n_l_instr_ n_l_instr;
typedef struct n_instr_ n_instr;
typedef struct n_exp_ n_exp;
typedef struct n_l_exp_ n_l_exp;
typedef struct n_var_ n_var;
typedef struct n_l_dec_ n_l_dec;
typedef struct n_dec_ n_dec;
typedef struct n_type_ n_type;
typedef struct n_l_fun_dec_ n_l_fun_dec;
typedef struct n_fun_dec_ n_fun_dec;
typedef struct n_prog_ n_prog;
typedef struct n_appel_ n_appel;

struct n_prog_ {
    n_l_dec *variables;
    n_l_fun_dec *fonctions;
    n_instr *corps;
};

n_prog *cree_n_prog(n_l_dec *variables, n_l_fun_dec *fonctions, n_instr *corps);

/*------ TYPES ------*/

struct n_type_ {
    enum {t_int, t_bool, t_array} type;
    struct n_type_ *arrayof;
    int debut;
    int fin;
};

n_type *cree_n_type_int(void);
n_type *cree_n_type_bool(void);
n_type *cree_n_type_array(n_type *t, int debut, int fin);

/*------ DECLARATIONS ------*/

struct n_dec_ {
    char *nom;
    n_type *type;
};

n_dec *cree_n_dec_var(char *nom, n_type *t);

struct n_fun_dec_{
    char *nom;
    n_type *type;
    n_l_dec *variables;
    n_l_dec *param;
    n_instr *corps;
};

n_fun_dec *cree_n_dec_fonc(char *nom, n_type *t,
                           n_l_dec *param, n_l_dec *variables,
                           n_instr *corps);

/*------ EXPRESSIONS ------*/

typedef enum {plus, moins, fois, divise, modulo, egal, diff, inf, sup, infeg, supeg, ou, et, non, negatif} operation;
struct n_exp_ {
    enum{varExp, intExp, opExp, trueExp, falseExp, appelExp, lireExp} type;
    union{
        struct{operation op; struct n_exp_ *op1; struct n_exp_ *op2;} opExp_;
        n_var *var;
        int entier;
        n_appel *appel;
    }u;
};

n_exp *cree_n_exp_op(operation type, n_exp *op1, n_exp *op2);
n_exp *cree_n_exp_entier(int entier);
n_exp *cree_n_exp_var(n_var *var);
n_exp *cree_n_exp_appel(n_appel *app);
n_exp *cree_n_exp_lire(void);
n_exp *cree_n_exp_true(void);
n_exp *cree_n_exp_false(void);

struct n_appel_{
    char *fonction;
    n_l_exp *args;
};

n_appel *cree_n_appel(char *fonction, n_l_exp *args);

struct n_var_ {
    enum {simple, indicee} type;
    char *nom;
    n_exp *indice;
};

n_var *cree_n_var_simple(char *nom);
n_var *cree_n_var_indicee(char *nom, n_exp *indice);

/*------ INSTRUCTIONS ------*/

struct n_instr_ {
    enum {affecteInst, siInst, tantqueInst, appelInst, ecrireInst, videInst, blocInst} type;
    union{
        struct{n_exp *test; struct n_instr_ *alors; struct n_instr_ *sinon;} si_;
        struct{n_exp *test; struct n_instr_ *faire;} tantque_;
        n_appel *appel;
        struct{n_var *var; n_exp *exp;} affecte_;
        struct{n_exp *expression;} retour_;
        struct{n_exp *expression;} ecrire_;
        n_l_instr *liste;
    }u;
};

n_instr *cree_n_instr_si(n_exp *test, n_instr *alors, n_instr *sinon);
n_instr *cree_n_instr_bloc(n_l_instr *liste);
n_instr *cree_n_instr_tantque(n_exp *test, n_instr *faire);
n_instr *cree_n_instr_affect(n_var *var, n_exp *exp);
n_instr *cree_n_instr_appel(n_appel *appel);
n_instr *cree_n_instr_ecrire(n_exp *expression);
n_instr *cree_n_instr_vide(void);

/*------ LISTES D'OBJETS------*/
struct n_l_exp_ {
    n_exp *tete;
    struct n_l_exp_ *queue;
};

n_l_exp *cree_n_l_exp(n_exp *tete, n_l_exp *queue);

struct n_l_instr_ {
    n_instr *tete;
    struct n_l_instr_ *queue;
};

n_l_instr *cree_n_l_instr(n_instr *tete, n_l_instr *queue);

struct n_l_dec_{
    n_dec *tete;
    struct n_l_dec_ *queue;
};

n_l_dec *cree_n_l_dec(n_dec *tete, n_l_dec *queue);
n_l_dec *fusionne_n_l_dec(n_l_dec *l1, n_l_dec *l2);

struct n_l_fun_dec_{
    n_fun_dec *tete;
    struct n_l_fun_dec_ *queue;
};

n_l_fun_dec *cree_n_l_fun_dec(n_fun_dec *tete, n_l_fun_dec *queue);

#endif /* __SYNTABS__ */
