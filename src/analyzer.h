#ifndef ANALYZERS_H
#define ANALYZERS_H

#include <cmath>

#include "general.h"

class graph_analyzer{
public:
    graph& g;
    explicit graph_analyzer(graph& graph) : g(graph) {}

    double get_density() const;

    // CC means Connected Components
    // CSC means Strongly Connected Components
    vector<set<int>> get_CCs();
    vector<set<int>> get_SCCs();

    size_t get_amount_of_CC();
    size_t get_amount_of_SCC();

    double get_fraction_of_vertexes_in_max_CC();
    double get_fraction_of_vertexes_in_max_SCC();

    double get_local_clustering_coefficient(int v);
    double get_global_clustering_coefficient() const;
    double get_average_clustering_coefficient();

    size_t get_amount_of_triangles() const;

    size_t get_amount_of_opened_triplets() const;
    size_t get_amount_of_opened_triplets(int v) const;

    size_t get_amount_of_closed_triplets() const;
    size_t get_amount_of_closed_triplets(int v) const;

    size_t get_degree(int v) const;
    size_t get_min_degree() const;
    size_t get_max_degree() const;
    double get_average_degree() const;

    double get_probability_that_random_vertex_has_some_degree(size_t degree);
    double get_probability_that_random_vertex_has_some_degree_log_log(size_t log2_degree);

    size_t get_size_of_max_CC_after_delete_x_percentage_vertexes(double x);
    size_t get_size_of_max_CC_after_delete_x_percentage_vertexes_of_max_degrees(double x);

    size_t estimate_diameter_of_max_CC_from_double_sweep(); // 2a
    size_t estimate_diameter_of_max_CC_from_sample(int sample_size = 1000); // 2b
    double estimate_90th_percentile_of_max_CC_from_sample(int sample_size = 1000);
    size_t estimate_diameter_of_max_CC_from_snowball(int target_size = 1000); // 2c
    double estimate_90th_percentile_of_max_CC_from_snowball(int target_size = 1000);

    double get_average_clustering_coefficient_max_CC(); // 4

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

    // For double swap
    pair<int, int> find_farthest_vertex_by_bfs(int v);
    unordered_map<int, int> get_distances_from(int v);

    // For measuring global clustering coefficient
    size_t get_amount_of_closed_triplets(const vector<int>& neighbourhood) const;

    // For measuring probabilities function of degrees
    unordered_map<size_t, size_t> degrees_counter;
    vector<pair<size_t, int>> degrees_vector; // first - degree, second - vertex
    void init_degree_counters_cache();

    // For measuring sizes after deletes
    set<int> get_max_CC();

    // For snowball
    set<int> build_snowball_sample(int target_size);
    unordered_map<int, int> get_distances_in_subset(int v, const set<int>& allowed);

    vector<int> pairwise_distances_in_component(const vector<int>& sample);
    vector<int> pairwise_distances_in_subset(const set<int>& subset);
};

#endif // ANALYZERS_H
