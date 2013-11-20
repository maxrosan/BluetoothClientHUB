
#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#ifndef TRUE
#define TRUE 1
#endif 

#ifndef FALSE
#define FALSE 0
#endif

#define DEBUG_ENABLED 1
#define DEBUG(MSG, ...) if (DEBUG_ENABLED) fprintf(stderr, MSG "\n", ## __VA_ARGS__)

#endif