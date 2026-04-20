#ifndef GENERAL_H
#define GENERAL_H

#include <stdio.h>

#define ERROR_PRINT(...) fprintf(stderr, __VA_ARGS__)
enum g_type {
    Undefined, Directed, Undirected
};

#endif // GENERAL_H
