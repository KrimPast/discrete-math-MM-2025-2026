#ifndef ANALYZERS_H
#define ANALYZERS_H

#include <vector>

#include "general.h"
#include "graph.h"

using namespace std;

class graph_analyzer{
public:
    graph& g;
    graph_analyzer(graph& graph) : g(graph) {}

    double get_density() const;

    // CC means Connected Components
    // CSC means Strongly Connected Components
    set<set<int>> get_CCs();
    set<set<int>> get_SCCs();

    size_t get_amount_of_CC();
    size_t get_amount_of_SCC();

    double get_fraction_of_vertexes_in_max_CC();
    double get_fraction_of_vertexes_in_max_SCC();

    double get_local_clustering_coefficient(int v);
    double get_global_clustering_coefficient();
    double get_average_clustering_coefficient();

    size_t get_amount_of_triangles();

    size_t get_amount_of_opened_triplets();
    size_t get_amount_of_opened_triplets(int v);

    size_t get_amount_of_closed_triplets();
    size_t get_amount_of_closed_triplets(int v);
private:
    // For searching connected components
    unordered_map<int, int> CC_comp_id;
    graph rg; // reversed graph

    void CC_undirected_dfs(int v);
    void CC_directed_dfs(int v);

    // For searching strongly connected components
    unordered_map<int, bool> SCC_visited;
    vector<int> SCC_order;
    set<int> SCC_component;
    void SCC_dfs1(int v);
    void SCC_dfs2(int v);

    size_t get_amount_of_triplets(size_t (*amount_func)(int));

    // For measuring global clustering coefficient
    size_t get_amount_of_closed_triplets(int v, vector<int>& neighbourhood);
};

#endif // ANALYZERS_H
