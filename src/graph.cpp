#include "graph.h"

#include <set>

void graph::insert(const int from, const int to) {
    (*this)[from].push_back(to);
    if (type == Undirected) {
        (*this)[to].push_back(from);
    }
    ++amount_edges;
}
void graph::remove(const int from, const int to) {
    if (!contains(from)) return;

    std::erase((*this)[from], to);
    if (type == Undirected) {
        std::erase((*this)[to], from);
    }
    --amount_edges;
}

set<int> graph::get_vertexes() const {
    set<int> vertexes;
    for (auto& pair : *this) {
        vertexes.insert(pair.first);
        auto& others = pair.second;
        for (auto other : others) {
            vertexes.insert(other);
        }
    }
    return vertexes;
}

graph graph::get_reversed_graph() const {
    if (type == Undirected) {
        throw runtime_error("get_reversed_graph: You sure that you really need RG by undirected graph?");
    } else if (type == Undefined) {
        throw runtime_error("get_reversed_graph: Undefined type of graph");
    }
    graph rg;
    rg.type = type;
    rg.amount_vertexes = amount_vertexes;
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

void graph::calculate_vertexes() {
    if (type == Undirected) {
        amount_vertexes = size();
    }
    else if (type == Directed) {
        amount_vertexes = get_vertexes().size();
    }
    else {
        throw runtime_error("calculate_vertexes: Invalid graph type");
    }
}

// void graph::calculate_edges() {
//     if (type == Undefined) {
//         throw runtime_error("calculate_edges: Cannot count amount of edges, because graph type is Undefined");
//     }
//     amount_edges = 0;
//     for (auto &edges_list : *this) {
//         amount_edges += edges_list.second.size();
//     }
//     if (type == Undirected) {
//         amount_edges /= 2;
//     }
// }
