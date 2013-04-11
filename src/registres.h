/*
 * @File registres.h
 *
 * @Date 11-04-2013
 *
 */
#ifndef __REGISTRES_H__
#define __REGISTRES_H__

#define NB_REGISTRES            32

int reg[NB_REGISTRES];

/* cree le tableau des derniers appels :
 * dernier[ligne] = derniere ligne ou ligne est referencee
 */
int *creer_derniers_appels();
void initialiser_regs();

int cherche_registre_libre(int *dernier, int ligne);
int trouve_registre_associe(int ligne);

#endif /* __REGISTRES_H__ */
