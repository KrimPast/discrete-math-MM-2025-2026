#ifndef ANALYZERS_H
#define ANALYZERS_H

#include <cmath>
#include <unordered_map>
#include <vector>
#include <set>
#include <queue>

#include "general.h"

class graph_analyzer {
public:
    graph& g;
    explicit graph_analyzer(graph& graph) : g(graph) {}

    double get_density() const;

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

    size_t estimate_diameter_of_max_CC_from_double_sweep();
    size_t estimate_diameter_of_max_CC_from_sample(int sample_size = 1000);
    size_t estimate_90th_percentile_of_max_CC_from_sample(int sample_size = 1000);
    size_t estimate_diameter_of_max_CC_from_snowball(int target_size = 1000);
    size_t estimate_90th_percentile_of_max_CC_from_snowball(int target_size = 1000);

    double get_average_clustering_coefficient_max_CC();

private:
    // Connected components
    unordered_map<int, int> CC_comp_id;
    graph rg; // reversed graph
    void CC_undirected_bfs(int v);
    void CC_directed_bfs(int v);

    // Strongly connected components
    unordered_map<int, bool> SCC_visited;
    vector<int> SCC_order;
    set<int> SCC_component;
    void SCC_dfs1(int v);
    void SCC_dfs2(int v);

    // Double sweep / BFS
    pair<int, int> find_farthest_vertex_by_bfs(int v);
    unordered_map<int, int> get_distances_from(int v);

    // Landmark-based fast distance estimation
    static constexpr int DEFAULT_NUM_LANDMARKS = 30;
    int num_landmarks = DEFAULT_NUM_LANDMARKS;
    bool landmarks_built = false;
    vector<int> landmark_ids;
    unordered_map<int, vector<int>> landmark_dist;

    void ensure_landmarks_built();
    void build_landmarks();
    int estimate_distance(int s, int t) const;

    // Global clustering
    size_t get_amount_of_closed_triplets(const vector<int>& neighbourhood) const;

    // Degree distribution
    unordered_map<size_t, size_t> degrees_counter;
    vector<pair<size_t, int>> degrees_vector;
    void init_degree_counters_cache();

    // Max CC handling
    set<int> get_max_CC();

    // Snowball
    set<int> build_snowball_sample(int target_size);
};

#endif // ANALYZERS_H
