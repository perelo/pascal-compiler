/*
 * @File mips.h
 *
 * @Date 23-04-2013
 *
 */
#ifndef __MIPS_H__
#define __MIPS_H__

#include <stdio.h>

void genere_mips(FILE *sortie);

/* pile apres etre entree dans une fonction

  -----------
  |$fp      |
  |$t32     |
  | ...     |
  |$t0      |
  -----------
  |valeur   |   <- $fp
  |de retour|
  -----------
  |param 1  |
  | ...     |
  |param n  |   <- $sp avant jump
  -----------
  |variables|
  |locales  |
  -----------
  |adresse  |   <- $sp dans la fonction
  |de retour|
  -----------

*/

#endif /* __MIPS_H__ */
