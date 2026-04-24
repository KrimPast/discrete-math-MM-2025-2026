#ifndef GENERAL_H
#define GENERAL_H

#include <cstdio>
#include <boost/algorithm/string.hpp>

#define ERROR_PRINT(...) fprintf(stderr, __VA_ARGS__)
enum g_type {
    Undefined, Directed, Undirected
};
enum process_action {
    TryParseEdge,
    ParseMetadata,
    DoNothing
};
typedef std::pair<int, int> edge;
constexpr edge null_edge = edge{INT_MIN, INT_MIN};

#endif // GENERAL_H
