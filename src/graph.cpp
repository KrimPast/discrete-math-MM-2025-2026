#include "graph.h"

void graph::insert(const int from, const int to) {
    if (type == Undefined) throw runtime_error("insert: graph type is undefined");

    vector<int> &neighbors = (*this)[from];
    if (ranges::find(neighbors, to) != neighbors.end())
        return; // Удивительно, но такие графы есть
    ++vertex_counter[from];
    ++vertex_counter[to];

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

    --vertex_counter[from];
    --vertex_counter[to];
    if (vertex_counter[from] == 0) vertex_counter.erase(from);
    if (vertex_counter[to] == 0) vertex_counter.erase(to);

    --amount_edges;
}

void graph::remove_vertex(int v) {
    if (!contains(v)) return;
    if (type == Undefined) throw runtime_error("remove_vertex: graph type is undefined");
    if (type == Undirected) {
        auto &neighbors = (*this)[v];
        for (int i = neighbors.size() - 1; i >= 0; --i) {
            remove(v, neighbors[i]);
        }
        neighbors.clear();
    }
    else { // type == Directed
        auto vertexes = get_vertexes();
        for (auto other : vertexes) {
            if (!contains(other)) continue;
            auto& other_neighbors = (*this)[other];
            if (ranges::find(other_neighbors, v) != other_neighbors.end())
                remove(other, v);
        }
        auto neighbours = (*this)[v];
        for (auto other : neighbours) {
            remove(v, other);
        }
    }
    erase(v);
}

set<int> graph::get_vertexes() const{
    auto vertexes = set<int>();
    for (auto v : vertex_counter)
        vertexes.insert(v.first);
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

size_t graph::amount_vertexes() {
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
