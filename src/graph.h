#ifndef GRAPH_H
#define GRAPH_H

#include <optional>
#include <set>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <unordered_set>

using namespace std;

enum g_type {
    Undefined, Directed, Undirected
};

class graph : public unordered_map<int, unordered_set<int>>{
public:
    g_type type = Undefined;

    size_t amount_vertexes() const;
    size_t amount_edges = 0;

    void insert(int from, int to);
    void remove(int from, int to);

    unordered_set<int> vertexes;
    graph get_reversed() const;
private:
    unordered_map<int, int> vertex_counter;
};

#endif //GRAPH_H
