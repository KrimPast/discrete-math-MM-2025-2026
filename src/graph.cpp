#include "graph.h"

#include <ranges>

void graph::insert(const int from, const int to) {
    if (type == Undefined) throw runtime_error("insert: graph type is undefined");

    if ((*this)[from].contains(to))
        return; // Удивительно, но такие графы есть

    ++vertex_counter[from];
    ++vertex_counter[to];
    vertexes.insert(from);
    vertexes.insert(to);

    (*this)[from].insert(to);
    if (type == Undirected && from != to) {
        (*this)[to].insert(from);
    }
    ++amount_edges;
}
void graph::remove(const int from, const int to) {
    if (type == Undefined) throw runtime_error("insert: graph type is undefined");
    if (!contains(from)) return;

    (*this)[from].erase(to);
    if ((*this)[from].empty()) erase(from);

    if (type == Undirected && from != to) {
        (*this)[to].erase(from);
        if ((*this)[to].empty()) erase(to);
    }

    --vertex_counter[from];
    --vertex_counter[to];
    if (vertex_counter[from] == 0) {
        vertex_counter.erase(from);
        vertexes.erase(from);
    }
    if (vertex_counter[to] == 0) {
        vertex_counter.erase(to);
        vertexes.erase(to);
    }
    --amount_edges;
}


graph graph::get_reversed() const {
    if (type == Undirected) {
        throw runtime_error("get_reversed_graph: You sure that you really need RG by undirected graph?");
    } else if (type == Undefined) {
        throw runtime_error("get_reversed_graph: Undefined type of graph");
    }
    graph rg;
    rg.type = Directed;
    rg.amount_edges = amount_edges;
    for (auto& p : *this) {
        int from = p.first;
        auto& tos = p.second;
        for (auto to : tos) {
            rg.insert(to, from);
        }
    }
    return rg;
}

size_t graph::amount_vertexes() const {
    if (type == Undirected) {
        return size();
    }
    else if (type == Directed) {
        return vertex_counter.size();
    }
    else {
        throw runtime_error("calculate_amount_of_vertexes: Invalid graph type");
    }
}
