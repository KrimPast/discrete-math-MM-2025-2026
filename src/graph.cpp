#include "graph.h"

void graph::insert(const int from, const int to) {
    if (type == Undefined) throw runtime_error("insert: graph type is undefined");

    vector<int> &neighbors = (*this)[from];
    if (ranges::find(neighbors, to) != neighbors.end())
        return; // Удивительно, но такие графы есть
    
    (*this)[from].push_back(to);
    if (type == Undirected && from != to) {
        (*this)[to].push_back(from);
    }
    ++amount_edges;
}
void graph::remove(const int from, const int to) {
    if (type == Undefined) throw runtime_error("insert: graph type is undefined");
    if (!contains(from)) return;

    std::erase((*this)[from], to);
    if ((*this)[from].empty()) erase(from);

    if (type == Undirected && from != to) {
        std::erase((*this)[to], from);
        if ((*this)[to].empty()) erase(to);
    }
    --amount_edges;
}

void graph::remove_vertex(int v) {
    if (type == Undefined) throw runtime_error("remove_vertex: graph type is undefined");
    if (type == Undirected) {
        auto &neighbors = (*this)[v];
        for (int i = neighbors.size() - 1; i >= 0; --i) {
            remove(v, neighbors[i]);
        }
        neighbors.clear();

    }
    else { // type == Directed
        is_changed = true;
        auto vertexes = get_vertexes();
        for (auto other : vertexes) {
            amount_edges -= std::erase((*this)[other], v);
        }
        amount_edges -= (*this)[v].size();
        (*this)[v].clear();
    }
    erase(v);
    if (amount_vertexes > 0) --amount_vertexes; // if set
}

set<int> graph::get_vertexes() const {
    static size_t last_amount_vertexes = 0;
    static set<int> vertexes = set<int>();
    if (last_amount_vertexes == amount_vertexes) {
        return vertexes;
    }
    last_amount_vertexes = amount_vertexes;
    vertexes.clear();

    for (auto& pair : *this) {
        if (pair.second.empty()) continue;
        vertexes.insert(pair.first);
        auto& others = pair.second;
        for (auto other : others) {
            vertexes.insert(other);
        }
    }
    return vertexes;
}

graph graph::get_reversed() const {
    if (type == Undirected) {
        throw runtime_error("get_reversed_graph: You sure that you really need RG by undirected graph?");
    } else if (type == Undefined) {
        throw runtime_error("get_reversed_graph: Undefined type of graph");
    }
    graph rg;
    rg.type = Directed;
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

void graph::calculate_amount_of_vertexes() {
    if (type == Undirected) {
        amount_vertexes = size();
    }
    else if (type == Directed) {
        amount_vertexes = get_vertexes().size();
    }
    else {
        throw runtime_error("calculate_amount_of_vertexes: Invalid graph type");
    }
}
