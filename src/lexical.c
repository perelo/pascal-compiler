/*
 * @File yylex.c
 *
 * @Date 31-01-2013
 *
 * @Brief Analyseur lexical
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexical.h"
#include "lexemes.h"

int codeMotClefDefaut = ID;
char * libelleMotsClefs [] = { "program", "var",   "function", "begin", "end",
                               "if",      "then",  "else",     "while", "do",
                               "integer", "array", "read",     "write", "mod",
                               "div",     "and",   "or",       "true",  "false",
                               "boolean", "of",    "procedure" };
int codeMotsClefs [] = { PROGRAM, VAR,   FUNCTION, BEGIN, END,
                         IF,      THEN,  ELSE,     WHILE, DO,
                         INTEGER, ARRAY, READ,     WRITE, MOD,
                         DIV,     AND,   OR,       TRUE,  FALSE,
                         BOOLEAN, OF,    PROCEDURE };

int comparerTextAvecMotsClefs() {
    int nbMotsClefs = sizeof(codeMotsClefs) / sizeof(int);
    int i;
    for (i = 0; i < nbMotsClefs; ++i) {
        if (strcmp(yytext, libelleMotsClefs[i]) == 0) {
            return codeMotsClefs[i];
        }
    }
    return codeMotClefDefaut;

}

int yylex() {

    int code;
    int i = 0;
    char c = fgetc(yyin);

    while (!feof(yyin) && ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t'))) {
        c = fgetc(yyin);
    }

    if (feof(yyin)) {
        return 0;
    }

    if (isalpha(c)) {   // mot du langage ou var
        while (! feof(yyin) && (isalpha(c) || isdigit(c) || c == '_')) {
            yytext[i++] = c;
            c = fgetc(yyin);
        }
        yytext[i] = '\0';
        if (!feof(yyin))
            ungetc(c, yyin);

        code = comparerTextAvecMotsClefs();

    }
    else if (isdigit(c)) { // NUMBER
        while (! feof(yyin) && isdigit(c)) {
            yytext[i++] = c;
            c = fgetc(yyin);
        }
        yytext[i] = '\0';
        if (!feof(yyin))
            ungetc(c, yyin);

        code = NUMERAL;
    }
    else if (c == '<') {
        yytext[0] = c;
        code = c;
        c = fgetc(yyin);

        i = 1;
        if (c == '=') {
            yytext[i++] = c;
            code = INFEG;
        }
        else if (c == '>') {
            yytext[i++] = c;
            code = DIFF;
        }
        else {
            if (!feof(yyin))
                ungetc(c, yyin);
        }
        yytext[i] = '\0';
    }
    else if (c == '>') {
        yytext[0] = c;
        code = c;
        c = fgetc(yyin);

        i = 1;
        if (c == '=') {
            yytext[i++] = c;
            code = SUPEG;
        }
        else {
            if (!feof(yyin))
                ungetc(c, yyin);
        }
        yytext[i] = '\0';
    }
    else if (c == ':') {
        yytext[0] = c;
        code = c;
        c = fgetc(yyin);
        int i = 1;
        if (c == '=') {
            yytext[i++] = c;
            code = AFFECT;
        }
        else {
            if (!feof(yyin))
                ungetc(c, yyin);
        }
        yytext[i] = '\0';
    } else if (c == '.') {
        yytext[0] = c;
        code = c;
        c = fgetc(yyin);
        int i = 1;
        if (c == '.') {
            yytext[i++] = c;
            code = DOTDOT;
        }
        else {
            if (!feof(yyin))
                ungetc(c, yyin);
        }
        yytext[i] = '\0';
    }
    else {
        yytext[0] = c;
        yytext[1] = '\0';
        code = c;
    }

//    printf("yylex() returning %d as %s\n", code, yytext);
    return code;

} // yylex()

//int main(int argc, char **argv) {
//    int uniteCourante;
//    yyin = fopen(argv[1], "r");
//    if(yyin == NULL){
//      fprintf(stderr, "impossible d'ouvrir le fichier %s\n", argv[1]);
//      exit(1);
//    }
//
//    uniteCourante = yylex();
//    while (uniteCourante != 0) {
//      printf("%s, %d\n", yytext, uniteCourante);
//      uniteCourante = yylex();
//    }
//    return 0;
//}

