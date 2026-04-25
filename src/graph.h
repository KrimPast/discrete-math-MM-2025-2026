#ifndef GRAPH_H
#define GRAPH_H

#include <set>
#include <unordered_map>
#include <vector>
#include <stdexcept>

using namespace std;

enum g_type {
    Undefined, Directed, Undirected
};

class graph : public unordered_map<int, vector<int>>{
public:
    g_type type = Undefined;
    size_t amount_vertexes = 0;
    size_t amount_edges = 0;

    void insert(int from, int to);
    void remove(int from, int to);
    void remove_vertex(int v);

    set<int> get_vertexes() const;
    graph get_reversed() const;
    void calculate_amount_of_vertexes();
};

#endif //GRAPH_H
