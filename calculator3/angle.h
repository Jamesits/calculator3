#ifndef _ANGLE_H_
#define _ANGLE_H_

enum {MODE_DEG = 0, MODE_RAD = 1, MODE_GRA = 2};

const char angle_mode_name[][8] =
{
    "Degree",
    "Radian",
    "Gradian"
};

#endif

#ifndef M_PI
#define M_PI (3.1415926535)
#endif
