#ifndef GRAPH_H
#define GRAPH_H

#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "general.h"

using namespace std;

class graph : public unordered_map<int, vector<int>>{
public:
    g_type type = Undefined;
    size_t amount_vertexes = 0;
    size_t amount_edges = 0;

    void insert(int from, int to);
    void remove(int from, int to);

    set<int> get_vertexes() const;
    graph get_reversed_graph() const;
    void calculate_vertexes();
};

#endif //GRAPH_H
