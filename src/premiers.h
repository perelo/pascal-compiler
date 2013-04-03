/*
 * @File premiers.h
 *
 * @Date 19-03-2013
 *
 * @Brief Macros pour symplifier l'analyseur syntaxique
 *
 */
#ifndef __PREMIERS_H__
#define __PREMIERS_H__


#define TEST_PREM_STMT ( \
    TEST_CC(READ) || TEST_CC(WRITE) || TEST_CC(ID) || \
    TEST_CC(IF)   || TEST_CC(WHILE) || TEST_CC(BEGIN) \
)

#define TEST_PREM_ATOM ( \
    TEST_CC(READ) || TEST_CC(WRITE) || TEST_CC(ID)      || \
    TEST_CC(TRUE) || TEST_CC(FALSE) || TEST_CC(NUMERAL) || \
    TEST_CC('(') \
)

#define TEST_PREM_EXPR ( \
    TEST_CC('-')  || TEST_CC(NOT)   || TEST_PREM_ATOM \
)


#endif /* __PREMIERS_H__ */
