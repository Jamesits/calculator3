//
//  shunt.c
//  calculator3
//
//  Created by James Swineson on 15/1/7.
//  Copyright (c) 2015年 James Swineson. All rights reserved.
//
/*
 * Reference:
 * http://en.literateprograms.org/Shunting_yard_algorithm_(C)?oldid=18970
 */

#include "debug.h"
#include "errorcode.h"
#include "math_func.c"
#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXOPSTACK 64
#define MAXNUMSTACK 64
#define MAXEXPRSTACK 128

extern int errorcode;
extern jmp_buf env;

enum {ASSOC_NONE = 0, ASSOC_LEFT, ASSOC_RIGHT};

struct op_s
{
    char op;
    int prec;
    int assoc;
    int unary;
    
    math_type (*eval)(math_type a1, math_type a2);
    
    char fullname[5];
} ops[] = {
    {'N', 11, ASSOC_RIGHT, 1, eval_empty}, // "empty"
    {'_', 11, ASSOC_RIGHT, 1, eval_uminus},
    {'^', 9, ASSOC_RIGHT, 0, eval_exp},
    {'*', 8, ASSOC_LEFT, 0, eval_mul},
    {'/', 8, ASSOC_LEFT, 0, eval_div},
    {'%', 8, ASSOC_LEFT, 0, eval_mod},
    {'+', 5, ASSOC_LEFT, 0, eval_add},
    {'-', 5, ASSOC_LEFT, 0, eval_sub},
    {'(', 0, ASSOC_NONE, 0, NULL},
    {')', 0, ASSOC_NONE, 0, NULL},
    {-1, 10, ASSOC_RIGHT, 1, eval_sin, "sin"},
    {-2, 10, ASSOC_RIGHT, 1, eval_cos, "cos"},
    {-3, 10, ASSOC_RIGHT, 1, eval_tan, "tan"},
    {-4, 10, ASSOC_RIGHT, 1, eval_ln, "ln"},
    {-5, 10, ASSOC_RIGHT, 1, eval_log, "log"},
    {'!', 11, ASSOC_LEFT, 1, eval_frac},
};

struct op_s * getop(char ch)
{
    for ( int i = 0; i < sizeof ops / sizeof ops[0]; ++i )
    {
        if ( ops[i].op == ch )
        {
            return ops + i;
        }
    }
    
    return NULL;
}

_Bool hasleftoperand(char ch)
{
    for ( int i = 0; i < sizeof ops / sizeof ops[0]; ++i )
    {
        if ( ops[i].op == ch )
        {
            return !(ops[i].assoc != ASSOC_LEFT && ops[i].unary);
        }
    }
    
    return 0;
}

_Bool isopleftbracklet(struct op_s *op)
{
    
    // return op->assoc == ASSOC_NONE && op->op != ')';
    return op->op == '(';
}

_Bool isoprightbracklet(struct op_s *op)
{
    return op->op == ')';
}

struct op_s *opstack[MAXOPSTACK];
int nopstack = 0;
math_type numstack[MAXNUMSTACK];
int nnumstack = 0;

void push_opstack(struct op_s *op)
{
    if ( nopstack > MAXOPSTACK - 1 )
    {
        raise_error(4, __func__);
    }
    if ( DEBUG ) printf("[Debug] push opstack: %c\n", op->op);
    opstack[nopstack++] = op;
}

struct op_s * pop_opstack()
{
    if ( !nopstack )
    {
        raise_error(2, __func__);
    }
    
    if ( DEBUG )
        printf("[Debug] pop opstack: %c\n", opstack[nopstack - 1]->op);
    
    
    return opstack[--nopstack];
}

void push_numstack(math_type num)
{
    if ( nnumstack > MAXNUMSTACK - 1 )
    {
        raise_error(2, __func__);
    }
    if ( DEBUG ) printf("[Debug] push numstack: %lf\n", num);
    numstack[nnumstack++] = num;
}

math_type pop_numstack()
{
    if ( !nnumstack )
    {
        raise_error(4, __func__);
    }
    
    if ( DEBUG )
        printf("[Debug] pop numstack: %lf\n", numstack[nnumstack - 1]);
    
    
    return numstack[--nnumstack];
}

void shunt_op(struct op_s *op)
{
    struct op_s *pop;
    math_type n1, n2;
    
    if ( isopleftbracklet(op) )
    {
        // if is a left bracklet just push it
        push_opstack(op);
        
        return;
    }
    else if ( isoprightbracklet(op) )
    {
        // if is a right bracklet
        while ( nopstack > 0 && !isopleftbracklet(opstack[nopstack - 1]) )
        {       // eval from right until find a left bracklet or opstack is
                // empty
            pop = pop_opstack();
            n1 = pop_numstack();
            if ( pop->unary ) push_numstack( pop->eval(n1, 0) );
            else {
                n2 = pop_numstack();
                push_numstack( pop->eval(n2, n1) );
            }
        }
        // found left bracklet
        if ( !isopleftbracklet(opstack[nopstack - 1]) )
        {
            pop = pop_opstack();
            n1 = pop_numstack();
            if ( pop->unary ) push_numstack( pop->eval(n1, 0) );
            else {
                n2 = pop_numstack();
                push_numstack( pop->eval(n2, n1) );
            }
        }
        // not correct symbol
        if ( !( pop = pop_opstack() ) || op != NULL ||
            !isopleftbracklet(opstack[nopstack - 1]))
        {
            raise_error(4, __func__);
        }
        
        return;
    }
    
    if ( op->assoc == ASSOC_RIGHT )
    {
        while ( nopstack && op->prec < opstack[nopstack - 1]->prec )   // if
                                                                       // lower
                                                                       // prec
        {
            pop = pop_opstack();
            n1 = pop_numstack();
            if ( pop->unary ) push_numstack( pop->eval(n1, 0) );
            else {
                n2 = pop_numstack();
                push_numstack( pop->eval(n2, n1) );
            }
        }
    }
    else {
        while ( nopstack && op->prec <= opstack[nopstack - 1]->prec )
        {
            pop = pop_opstack();
            n1 = pop_numstack();
            if ( pop->unary ) push_numstack( pop->eval(n1, 0) );
            else {
                n2 = pop_numstack();
                push_numstack( pop->eval(n2, n1) );
            }
        }
    }
    push_opstack(op);
}

void init_stack()
{
    nopstack = 0;
    nnumstack = 0;
}

math_type eval2(char *input_array, math_type *result)
{
    printf("Eval2 process str: %s\n", input_array);
    
    char *expr;                 // read buffer pointer
    char *tstart = NULL;        // the pointer current token starts
    struct op_s startop = {'X', 0, ASSOC_NONE, 0, NULL};      // Dummy operator
                                                              // to mark start
    struct op_s *op = NULL;
    math_type n1, n2;
    struct op_s *lastop = &startop;
    
    for ( expr = input_array; *expr; ++expr )
    {
        if ( !tstart )     // outside a token
        {
            if ( ( op = getop(*expr) ) )   // try if can get an operator
            {
                if ( lastop &&
                    ( lastop == &startop || !isoprightbracklet(lastop) ) )
                    // the start of a sub-expression
                {
                    if ( DEBUG ) printf("[Debug] Got operator %c\n", op->op);
                    // replace uniary operators
                    if ( op->op == '-' ) op = getop('_');
                    //else if ( op->op == '+' || op->op == '*' )
                    //    op = getop('N');
                    else if ( !(isopleftbracklet(op) || op->op < 0))
                    {
                        if ( DEBUG )
                            printf(
                                   "[Debug] Error processing symbol %c\n",
                                   op->op);
                        raise_error(4, __func__);
                    }
                }
                shunt_op(op);
                lastop = op;
            }
            else if ( iisdigit(*expr) )
                tstart = expr;
            else if ( !isspace(*expr) )
            {
                raise_error(4, __func__);
            }
        }
        else {  // inside a token
            if ( isspace(*expr) )   // found a space: token end
            {
                if ( DEBUG ) printf( "[Debug] Got number %lf\n",
                                    str_to_num(tstart) );
                push_numstack( str_to_num(tstart) );
                tstart = NULL;
                lastop = NULL;
            }
            else if ( ( op = getop(*expr) ) )   // found an operator: token end
            {
                if ( DEBUG ) printf( "[Debug] Got number %lf\n",
                                    str_to_num(tstart) );
                push_numstack( str_to_num(tstart) );
                tstart = NULL;
                shunt_op(op);
                lastop = op;
            }
            else if ( !iisdigit(*expr) )    // not space, operator, number
            {
                raise_error(4, __func__);
            }
        }
    }
    if ( tstart ) push_numstack( str_to_num(tstart) );
    
    while ( nopstack )
    {
        op = pop_opstack();
        n1 = pop_numstack();
        if ( op->unary )
        {
            push_numstack( op->eval(n1, 0) );
        }
        else {
            n2 = pop_numstack();
            push_numstack( op->eval(n2, n1) );
        }
    }
    if ( nnumstack != 1 )
    {
        raise_error(4, __func__);
    }
    *result = numstack[nnumstack - 1];
    
    return errorcode;
}

bool isNumber(char *str)
{
    for (int i = 0; i < strlen(str); i++)
        if (!iisdigit(str[i])) {
            return false;
        }
    return true;
}

math_type eval(char *input_array, math_type *result)
{
    char *expr;                 // read buffer pointer
    
    char *last_leftbracklet;
    
    math_type r = 0;            // temp result
    int errorcode = 0;
    
    void *gctemp = NULL;
    
    while (!isNumber(input_array)) {
        init_stack();
        last_leftbracklet = NULL;
        gctemp = (void *)input_array;
        
        for ( expr = input_array; *expr; ++expr )
        {
            if (*expr == '(') {
                last_leftbracklet = expr;
            }
            if (*expr == ')') {
                break;
            }
        }
        
        
        char *process_string = (char *)malloc(sizeof(char) * (expr - last_leftbracklet + 2));
        memset(process_string, 0, expr - last_leftbracklet + 2);
        memcpy(process_string, last_leftbracklet + 1, (expr - last_leftbracklet - 1));
        printf("Center: %s\n", process_string);
        if ((errorcode = eval2(process_string, &r)) != 0) break;
        free(process_string);
        char *new_input_array = (char *)malloc(sizeof(char) * MAXEXPRSTACK);
        memcpy(new_input_array, input_array, (last_leftbracklet - input_array));
        sprintf(new_input_array + (last_leftbracklet - input_array), math_type_format, r);
        memcpy(new_input_array + strlen(new_input_array), expr + 1, strlen(expr + 1) + 1);
        printf("New array: %s\n", new_input_array);
        free(input_array); gctemp = NULL;
        input_array = new_input_array;
        //printf("result: %f\n", *result);
        
    }
    init_stack();
    printf("Final str: %s\n", input_array);
    /*
    char *new_input_array = (char *)malloc(sizeof(char) * MAXEXPRSTACK);
    memcpy(new_input_array, input_array + 1, strlen(input_array) - 2);
    free(input_array);
    eval2(new_input_array, result);
    free(new_input_array);
    result = &r;
     */
    if (gctemp != NULL) {
        free(gctemp);
    }
    sscanf(input_array, math_type_format, result);
    
    return errorcode;
}
