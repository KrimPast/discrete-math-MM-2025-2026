#include "graph.h"

void graph::insert(const int v, const int w) {
    (*this)[v].push_back(w);
    if (type == Directed) {
        (*this)[w].push_back(v);
    }
}
void graph::remove(const int v, const int w) {
    if (!contains(v)) return;
    std::erase((*this)[v], w); // std::erase = vector::erase, erase() - erase() of unordered map
    if ((*this)[v].empty())
        erase(v);
}

void graph::calculate_vertexes() {
    vertexes = size();
}

void graph::calculate_edges() {
    if (type == Undefined) {
        ERROR_PRINT("calculate_edges: Cannot count amount of edges, because graph type is Undefined");
        return;
    }
    edges = 0;
    for (auto edges_list : *this) {
        edges += edges_list.second.size();
    }
    if (type == Undirected) {
        edges /= 2;
    }
}