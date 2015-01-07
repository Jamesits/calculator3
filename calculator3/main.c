//
//  main.c
//  calculator3
//
//  Created by James Swineson on 15/1/7.
//  Copyright (c) 2015年 James Swineson. All rights reserved.
//

#include "angle.h"
#include "command.c"
#include "errorcode.h"
#include "shunt.c"
#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

int iisspace(int c)
{
    return (c == 0x00 || c == 0x20 || c == 0x09 || c == 0x0a || c == 0x0b ||
            c == 0x0c || c == 0x0d);
}

void str_replace(char *orig, char *rep, char with)
{
    char *position = NULL;
    
    while (position = strstr(orig, rep), position != NULL)
    {
        unsigned long length = strlen(rep);
        
        *position = with;
        memmove(position + 1, position + length, strlen(position + length) + 1);
    }
}

char *input, *p;
int length;
jmp_buf env;
int errorcode = 0;
math_type last_result = 0;
int angle_mode = MODE_RAD;

void addchar(char c)
{
    long n;
    
    // continue to input
    n = p - input; // current string length
    if (n + 2 == length) input = (char *) realloc(input, length *= 2); // n + 2
                                                                       // for we
                                                                       // need
                                                                       // to set
                                                                       // *p++ =
                                                                       // ' '
                                                                       // later.
    p = n + input;
    *p++ = c;         // store the input, then set the pointer + 1
}

void init()
{
    fseek(stdin, 0, SEEK_END);  // flush stdin buffer
    errorcode = 0;
    input = (char *) malloc(length = 4);
    *input = 0;
    p = input;
    init_stack();
    printf("calculator[%s] > ", angle_mode_name[angle_mode]);
    addchar('(');
}

int main()
{
    clear_screen("");
    puts(
         "Input any expression and press Enter. Type \"help\" for more information.");
    
    char c;
    char lastchar = 0;
    
    init();
    while ( ( c = getchar() ) != EOF )  // input
    {
        if (c != '\n')  // in one line
        {
            if ( lastchar != 0 && !iisspace(lastchar) &&
                (iisdigit(lastchar) ||
                 lastchar == ')') && !iisdigit(c) && !iisspace(c) &&
                !hasleftoperand(c) )
                // auto add missing *
                addchar('*');
            //if ( !iisspace(c) )
            {
                lastchar = c;
            }
            addchar(c);     // push to input queue
            
        } else{    // enter pressed, start string process
            addchar(')');
            if ( DEBUG ) printf("[Debug] got string '%s'\n", input);
            if ( strcmp(input, "()") != 0 && !iscommand(input) ) // if not a command
                if ( !setjmp(env) )
                {
                    // assume input data is an expression
                    *p = 0;
                    
                    // check brecklet mismatch
                    int bracklet_count = 0;
                    for (int i = 0; i < strlen(input); i++)
                    {
                        if (input[i] == '(') bracklet_count++;
                        if (input[i] == ')') bracklet_count--;
                        if (bracklet_count < 0) raise_error(4, __func__);
                    }
                    if (bracklet_count > 0) // auto match bracklets
                    {
                        fprintf(stderr,
                                "[Warning] An attempt was made to fix mismatched parentheses, brackets, or braces.\n");
                        while (bracklet_count-- > 0)
                            addchar(')');
                    }
                    for (int i = 0; i < strlen(input); i++)
                        if (input[i] == '!')
                            fprintf(stderr,
                                    "[Warning] Deprecated symbol \'!\': this sometimes lead to wrong result. Use function frac(x) instead.\n");
                    
                    if ( DEBUG )  printf("[Debug] Real input string: %s\n", input);
                    // char *processed_input_string;
                    for (int i = 0; i < sizeof ops / sizeof ops[0]; ++i)
                    {
                        if ( strlen(ops[i].fullname) )
                        {
                            str_replace(input, ops[i].fullname, ops[i].op);
                        }
                    }
                    if ( DEBUG )  printf("[Debug] Real processed string: %s\n", input);
                    // calculate
                    math_type *pc = malloc( sizeof(math_type) );
                    int return_code = eval(input, pc);
                    
                    // prepare for print result
                    if (!return_code)
                    {
                        // truncate zeros in the end
                        last_result = *pc;
                        char temp[80];
                        sprintf(temp, math_type_output_format, *pc);
                        while (temp[strlen(temp) - 1] == '0')
                            temp[strlen(temp) - 1] = 0;
                        if (temp[strlen(temp) - 1] ==
                            '.') temp[strlen(temp) - 1] = 0;
                        // print result
                        printf("= %s\n", temp);
                    }
                }
            
            // start a new line
            //if (input != NULL) free(input);
            init();
            continue;
        }
    
    }
    return EXIT_SUCCESS;
}
