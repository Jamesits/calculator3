#include "angle.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

typedef double math_type;

#define math_type_format "%lf"
#define math_type_output_format "%.15lf"

extern math_type last_result;
extern int angle_mode;

math_type convert_angles_to_radian(math_type a1)
{
    // printf("[Debug] angle mode: %d, rad %lf, deg %lf\n", angle_mode, a1, a1 *
    // M_PI / 180.0);
    if (angle_mode == MODE_RAD) return a1;
    if (angle_mode == MODE_DEG) return a1 * M_PI / 180.0;
    if (angle_mode == MODE_GRA) return a1 * M_PI / 200.0;

    return 0;
}

math_type eval_empty(math_type a1, math_type a2)
{
    return a1;
}

math_type eval_uminus(math_type a1, math_type a2)
{
    return -a1;
}

math_type eval_exp(math_type a1, math_type a2)
{
    return a2 < 0 ? 0 : ( a2 == 0 ? 1 : a1 *eval_exp(a1, a2 - 1) );
}

math_type eval_mul(math_type a1, math_type a2)
{
    return a1 * a2;
}

math_type eval_div(math_type a1, math_type a2)
{
    if (!a2)
    {
        fprintf(stderr, "ERROR: Division by zero\n");
        exit(EXIT_FAILURE);
    }

    return a1 / a2;
}

math_type eval_mod(math_type a1, math_type a2)
{
    if (!a2)
    {
        fprintf(stderr, "ERROR: Division by zero\n");
        exit(EXIT_FAILURE);
    }

    // return a1 %((int)a2);
    return a1 - a1 / a2;
}

math_type eval_add(math_type a1, math_type a2)
{
    return a1 + a2;
}

math_type eval_sub(math_type a1, math_type a2)
{
    return a1 - a2;
}

math_type eval_sin(math_type a1, math_type a2)
{
    return sin( convert_angles_to_radian(a1) );
}

math_type eval_cos(math_type a1, math_type a2)
{
    return cos( convert_angles_to_radian(a1) );
}

math_type eval_tan(math_type a1, math_type a2)
{
    return tan( convert_angles_to_radian(a1) );
}

math_type eval_ln(math_type a1, math_type a2)
{
    return log(a1);
}

math_type eval_log(math_type a1, math_type a2)
{
    return log10(a1);
}

math_type eval_frac(math_type a1, math_type a2)
{
    int result = a1;

    for (; --a1 > 1; )
    {
        result *= a1;
    }

    return result;
}

math_type str_to_num(char *str)
{
    if (*str == '$') return last_result;

    math_type result;

    sscanf(str, math_type_format, &result);

    return result;
}

int iisdigit(char ch)
{
    return ( (ch >= '0' && ch <= '9') || ch == '.' || ch == '$' );
}
