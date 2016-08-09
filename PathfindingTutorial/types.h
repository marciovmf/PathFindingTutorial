#ifndef TYPES_H

#define TYPES_H
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(x) ((x)<0 ? -(x) : (x))

#include <stdint.h>
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef int8_t int8;
typedef uint8_t bool8;

#endif