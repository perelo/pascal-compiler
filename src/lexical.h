/*
 * @File yylex.h
 *
 * @Date 31-01-2013
 *
 * @Brief Definitions pour l'analyseur lexical
 *
 */
#ifndef __YYLEX_H__
#define __YYLEX_H__

#include <stdio.h>

#include "lexemes.h"

char yytext[100];
FILE *yyin;

int yylex();
int comparerTextAvecMotsClefs();

#endif /* __YYLEX_H__ */
