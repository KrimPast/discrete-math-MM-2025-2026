#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "general.h"

using namespace std;

class graph : public unordered_map<int, vector<int>>{
public:
    g_type type = Undefined;
    size_t vertexes = 0;
    size_t edges = 0;

    void insert(int v, int w);
    void remove(int v, int w);

    void calculate_vertexes();
    void calculate_edges();
};

#endif //GRAPH_H
