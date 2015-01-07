#ifndef _ERRORCODE_H_
#define _ERRORCODE_H_

#include <setjmp.h>
#include <stdio.h>

const char error_msg_en[][40] = {
    "",
    "Unknown error",
    "Stack overflow",
    "Division by zero",
    "Syntax error",
    "Base conversion error"
};

#define error_msg error_msg_en

extern int errorcode;
extern jmp_buf env;

void raise_error(int ierrorcode, const char *function_name)
{
    fprintf(stderr,
            "[Error %d] In function %s: %s\n",
            ierrorcode,
            function_name,
            error_msg[ierrorcode]);
    errorcode = ierrorcode;
    longjmp(env, 0);
}

#endif
