#include <omp.h>
#include "analyzer.h"

#include <queue>

double graph_analyzer::get_density() const {
    size_t max_edges = g.amount_vertexes * (g.amount_vertexes - 1) / 2;
    return static_cast<double>(g.amount_edges) / static_cast<double>(max_edges);
}

size_t graph_analyzer::get_amount_of_CC() {
    return get_CCs().size();
}
size_t graph_analyzer::get_amount_of_SCC() {
    return get_SCCs().size();
}

double graph_analyzer::get_fraction_of_vertexes_in_max_CC() {
    auto components = get_CCs();
    size_t mx = 0;
    for (auto &component : components) {
        mx = max(mx, component.size());
    }
    return static_cast<double>(mx) / static_cast<double>(g.size());
}

double graph_analyzer::get_fraction_of_vertexes_in_max_SCC() {
    auto components = get_SCCs();
    size_t mx = 0;
    for (auto &component : components) {
        mx = max(mx, component.size());
    }
    return static_cast<double>(mx) / static_cast<double>(g.size());
}

double graph_analyzer::get_local_clustering_coefficient(const int v) {
    if (g.type == Undefined) {
        throw runtime_error("get_local_clustering_coefficient: Cannot on undefined graph!\n");
    }
    set neighbourhood_set(g[v].begin(), g[v].end());

    if (g.type == Directed) {
        if (rg.empty()) rg = g.get_reversed();
        neighbourhood_set.insert(rg[v].begin(), rg[v].end());
    }
    vector<int> neighbourhood_list(neighbourhood_set.begin(), neighbourhood_set.end());
    neighbourhood_set.clear();

    size_t count = get_amount_of_closed_triplets(neighbourhood_list);
    if (g.type == Undirected) count *= 2;

    size_t neighbours = neighbourhood_list.size();
    size_t max_count = neighbours * (neighbours - 1);
    if (max_count == 0) return 0; // means vertex doesn't have third neighbor
    return static_cast<double>(count) / static_cast<double>(max_count);
}

double graph_analyzer::get_global_clustering_coefficient() const {
    size_t opened_triplets = get_amount_of_opened_triplets();
    size_t closed_triplets = get_amount_of_closed_triplets();
    size_t triplets = opened_triplets + closed_triplets;
    return static_cast<double>(closed_triplets) / static_cast<double>(triplets);
}

double graph_analyzer::get_average_clustering_coefficient() {
    double amount = 0;
    auto vertexes = g.get_vertexes();
    for (auto v : vertexes) {
        amount += get_local_clustering_coefficient(v);
    }
    return amount / static_cast<double>(vertexes.size());
}

size_t graph_analyzer::get_amount_of_triangles() const {
    static size_t last_amount_vertexes = 0;
    static size_t last_triangles = 0;
    if (last_amount_vertexes == g.amount_vertexes()) {
        return last_triangles;
    }
    last_amount_vertexes = g.amount_vertexes();
    last_triangles = get_amount_of_closed_triplets() * 3;
    return last_triangles;
}

// CC means Connected Components
void graph_analyzer::CC_undirected_bfs(const int v) {
    queue<int> q;
    q.push(v);
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        for (int o : g[cur]) {
            if (CC_comp_id[o] == -1) {
                CC_comp_id[o] = CC_comp_id[cur];
                q.push(o);
            }
        }
    }
}
void graph_analyzer::CC_directed_bfs(const int v) {
    queue<int> q;
    q.push(v);
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        for (int o : g[cur]) {
            if (CC_comp_id[o] == -1) {
                CC_comp_id[o] = CC_comp_id[cur];
                q.push(o);
            }
        }
        for (int o : rg[cur]) {
            if (CC_comp_id[o] == -1) {
                CC_comp_id[o] = CC_comp_id[cur];
                q.push(o);
            }
        }
    }
}

vector<set<int>> graph_analyzer::get_CCs() {
    const auto v_list = g.get_vertexes();

    CC_comp_id.reserve(g.amount_vertexes());
    for (auto v : v_list) CC_comp_id[v] = -1;

    int c = 0;
    if (g.type == Undirected) {
        for (auto v : v_list) {
            if (CC_comp_id[v] != -1) continue;
            CC_comp_id[v] = c++;
            CC_undirected_bfs(v);
        }
    }
    else if (g.type == Directed) {
        if (rg.empty()) rg = g.get_reversed();

        for (auto v : v_list) {
            if (CC_comp_id[v] != -1) continue;
            CC_comp_id[v] = c++;
            CC_directed_bfs(v);
        }
    }

    auto components_list = vector<set<int>>(c);
    for (auto v : v_list) {
        components_list[CC_comp_id[v]].insert(v);
    }
    CC_comp_id.clear();
    return components_list;
}

vector<set<int>> graph_analyzer::get_SCCs() {
    if (g.type != Directed) throw runtime_error("get_SCCs: SCCs exists only in directed graph!");
    auto v_list = g.get_vertexes();
    if (rg.empty()) rg = g.get_reversed();
    for (auto v : v_list) {
        if (!SCC_visited[v])
            SCC_dfs1(v);
    }
    SCC_visited.clear();
    auto components = vector<set<int>>();
    const size_t n = SCC_order.size();
    for (int i = 0; i < n; i++) {
        if (int v = SCC_order[n - i - 1]; !SCC_visited[v]) {
            SCC_dfs2(v);
            components.push_back(SCC_component);
            SCC_component.clear();
        }
    }
    SCC_visited.clear();
    SCC_order.clear();
    return components;
}

// This is implementation of Kosaraju's algorithm
void graph_analyzer::SCC_dfs1(const int v) {
    SCC_visited[v] = true;
    for (auto o : g[v])
        if (!SCC_visited[o])
            SCC_dfs1(o);
    SCC_order.push_back(v);
}
void graph_analyzer::SCC_dfs2(const int v) {
    SCC_visited[v] = true;
    SCC_component.insert(v);
    for (auto o : rg[v])
        if (!SCC_visited[o])
            SCC_dfs2(o);
}

size_t graph_analyzer::get_amount_of_opened_triplets() const {
    atomic_size_t amount = 0;
    auto vertexes = g.get_vertexes();
    vector<int> vertexes_list(vertexes.begin(), vertexes.end());
#pragma omp parallel for
    for (auto v : vertexes_list) {
        amount += get_amount_of_opened_triplets(v);
    }
    return amount;
}

size_t graph_analyzer::get_amount_of_opened_triplets(const int v) const {
    vector<int>& neighbourhood = g[v];
    atomic_size_t count = 0;
#pragma omp parallel for
    for (auto second : neighbourhood) {
        for (auto third : g[second]) {
            if (v == third) continue;
            if (ranges::find(g[v], third) == g[v].end()) {
                ++count;
            }
        }
    }
    return count;
}

size_t graph_analyzer::get_amount_of_closed_triplets() const {
    static size_t last_amount_vertexes = 0;
    static size_t last_closed_triplets = 0;
    if (last_amount_vertexes == g.amount_vertexes()) {
        return last_closed_triplets;
    }

    size_t amount = 0;
    auto vertexes = g.get_vertexes();
    for (auto v : vertexes) {
        amount += get_amount_of_closed_triplets(v);
    }
    last_amount_vertexes = g.amount_vertexes();
    last_closed_triplets = amount;
    return amount;
}

size_t graph_analyzer::get_amount_of_closed_triplets(const int v) const {
    vector<int>& neighbourhood = g[v];
    return get_amount_of_closed_triplets(neighbourhood);
}

pair<int, int> graph_analyzer::find_farthest_vertex_by_bfs(const int v) {
    unordered_map<int, int> distances = get_distances_from(v);
    int far_vertex = v;
    int max_dist = 0;
    for (const auto& [vertex, dist] : distances) {
        if (dist > max_dist) {
            max_dist = dist;
            far_vertex = vertex;
        }
    }
    return {far_vertex, max_dist};
}

unordered_map<int, int> graph_analyzer::get_distances_from(const int v) {
    unordered_map<int, int> dist;
    queue<int> q;
    dist[v] = 0;
    q.push(v);

    if (g.type == Directed && rg.empty())
        rg = g.get_reversed();

    while (!q.empty()) {
        int cur = q.front(); q.pop();

        for (int to : g[cur]) {
            if (!dist.contains(to)) {
                dist[to] = dist[cur] + 1;
                q.push(to);
            }
        }

        if (g.type == Directed) {
            for (int to : rg[cur]) {
                if (!dist.contains(to)) {
                    dist[to] = dist[cur] + 1;
                    q.push(to);
                }
            }
        }
    }
    return dist;
}
size_t graph_analyzer::get_amount_of_closed_triplets(const vector<int>& neighbourhood) const {
    atomic_size_t count = 0;
    for (int j = 0; j < neighbourhood.size(); j++) {
        atomic_size_t k_start = g.type == Undirected ? j + 1 : 0;
#pragma omp parallel for
        for (size_t k = k_start; k < neighbourhood.size(); k++) {
            int second = neighbourhood[j];
            int third = neighbourhood[k];
            if (ranges::find(g[second], third) != g[second].end()) {
                ++count;
            }
        }
    }
    return count;
}

size_t graph_analyzer::get_degree(const int v) const {
    if (g.type == Undirected) {
        if (!g.contains(v)) throw runtime_error("get_degree: No such vertex in graph");
        return g[v].size() + (ranges::find(g[v], v) != g[v].end() ? 1 : 0); // reflexive adds two to degree
    }
    throw runtime_error("get_degree: Not implemented for graphs this type");
}

size_t graph_analyzer::get_min_degree() const {
    auto vertexes = g.get_vertexes();
    size_t mn = g.amount_edges;
    for (auto v : vertexes) {
        mn = min(mn, get_degree(v));
    }
    return mn;
}

size_t graph_analyzer::get_max_degree() const {
    const auto vertexes = g.get_vertexes();
    size_t mx = 0;
    for (const auto v : vertexes) {
        mx = max(mx, get_degree(v));
    }
    return mx;
}

double graph_analyzer::get_average_degree() const {
    const auto vertexes = g.get_vertexes();
    size_t sm = 0;
    for (const auto v : vertexes) {
        sm += get_degree(v);
    }
    g.calculate_amount_of_vertexes();
    return static_cast<double>(sm) / static_cast<double>(g.amount_vertexes);
}

// Function return probability, which enters in [0, 1], what means random vertex has degree, which equals input degree
double graph_analyzer::get_probability_that_random_vertex_has_some_degree(size_t degree) {
    if (degrees_counter.empty()) init_degree_counters_cache();

    return static_cast<double>(degrees_counter[degree]) / static_cast<double>(g.amount_vertexes);
}

// Function returns log2(probability), which enters in (-infinity, 0], what means random vertex has degree, which enters in...
// ... [2 ^ log2_degree,  2 ^ (log2_degree + 1) )
double graph_analyzer::get_probability_that_random_vertex_has_some_degree_log_log(size_t log2_degree) {
    if (degrees_counter.empty()) init_degree_counters_cache();

    const size_t min_degree = 1 << log2_degree;
    const size_t max_degree = min_degree * 2;
    size_t amount = 0;
    for (size_t degree = min_degree; degree < max_degree; degree++) {
        amount += degrees_counter[degree];
    }
    return std::log2(static_cast<double>(amount) / static_cast<double>(g.amount_vertexes));
}


void graph_analyzer::init_degree_counters_cache() {
    degrees_counter.clear();
    degrees_vector.clear();
    auto vertexes = g.get_vertexes();
    for (auto v : vertexes) {
        size_t degree = get_degree(v);
        ++degrees_counter[degree];
        degrees_vector.emplace_back(degree, v);
    }
}

set<int> graph_analyzer::get_max_CC() {
    auto CCs = get_CCs();
    auto vertexes = g.get_vertexes();
    if (vertexes.empty()) return {};

    set<int> &max_CC = CCs[0];
    for (auto &CC: CCs) {
        if (CC.size() > max_CC.size()) {
            max_CC = CC;
        }
    }
    return max_CC;
}

size_t graph_analyzer::get_size_of_max_CC_after_delete_x_percentage_vertexes(const double x){
    if (x < 0 || x > 1) throw runtime_error("X must be between 0 and 1");
    if (x == 1) return 0;

    const auto deleting_amount = static_cast<size_t>(x * static_cast<double>(g.amount_vertexes));
    for (const auto deleting = other::get_random_n_elements_from_set(g.get_vertexes(), deleting_amount); const auto v : deleting) {
        g.remove_vertex(v);
    }
    return get_max_CC().size();
}

size_t graph_analyzer::get_size_of_max_CC_after_delete_x_percentage_vertexes_of_max_degrees(const double x) {
    if (x < 0 || x > 1) throw runtime_error("X must be between 0 and 1");
    if (x == 1) return 0;

    auto deleting_amount = static_cast<size_t>(x * static_cast<double>(g.amount_vertexes));
    init_degree_counters_cache();

    ranges::sort(degrees_vector, other::degree_greater);
    size_t deleted = 0;
    while (deleted < deleting_amount) {
        g.remove_vertex(degrees_vector[deleted].second);
        ++deleted;
    }

    return get_max_CC().size();
}

size_t graph_analyzer::estimate_diameter_of_max_CC_from_double_sweep() {
    const auto max_CC = get_max_CC();
    if (max_CC.empty()) return 0;

    const int start = other::random_element(max_CC);

    auto [v1, d1] = find_farthest_vertex_by_bfs(start);

    auto [v2, d2] = find_farthest_vertex_by_bfs(v1);

    return static_cast<size_t>(d2);
}

size_t graph_analyzer::estimate_diameter_of_max_CC_from_sample(const int sample_size) {
    const auto max_cc = get_max_CC();
    if (max_cc.empty()) return 0;

    const int k = min(sample_size, static_cast<int>(max_cc.size()));
    const auto sample = other::get_random_n_elements_from_set(max_cc, k);
    auto dists = pairwise_distances_in_component(sample);
    if (dists.empty()) return 0;
    return *ranges::max_element(dists);
}

double graph_analyzer::estimate_90th_percentile_of_max_CC_from_sample(const int sample_size) {
    const auto max_cc = get_max_CC();
    if (max_cc.empty()) return 0.0;

    const int k = min(sample_size, static_cast<int>(max_cc.size()));
    const auto sample = other::get_random_n_elements_from_set(max_cc, k);
    auto dists = pairwise_distances_in_component(sample);
    if (dists.empty()) return 0.0;

    ranges::sort(dists);
    const size_t idx = (dists.size() - 1) * 9 / 10;
    return dists[idx];
}

double graph_analyzer::get_average_clustering_coefficient_max_CC() {
    const set<int> max_cc = get_max_CC();
    if (max_cc.empty()) return 0.0;

    double sum = 0.0;
    for (const int v : max_cc) {
        sum += get_local_clustering_coefficient(v);
    }
    return sum / static_cast<double>(max_cc.size());
}

set<int> graph_analyzer::build_snowball_sample(const int target_size) {
    auto max_cc = get_max_CC();
    if (max_cc.empty() || max_cc.size() == 1) return max_cc;

    if (g.type == Directed && rg.empty())
        rg = g.get_reversed();

    const int seed1 = other::random_element(max_cc);

    set<int> neighbors;
    for (int to : g[seed1]) neighbors.insert(to);
    if (g.type == Directed)
        for (int to : rg[seed1]) neighbors.insert(to);

    const int seed2 = other::random_element(neighbors);

    set<int> sample;
    sample.insert(seed1);
    sample.insert(seed2);

    std::queue<int> q;
    q.push(seed1);
    q.push(seed2);

    while (!q.empty() && sample.size() < target_size) {
        int cur = q.front(); q.pop();

        for (int to : g[cur]) {
            if (sample.insert(to).second) {
                if (sample.size() >= target_size) break;
                q.push(to);
            }
        }
        if (sample.size() >= target_size) break;

        if (g.type == Directed) {
            for (int to : rg[cur]) {
                if (sample.insert(to).second) {
                    if (sample.size() >= target_size) break;
                    q.push(to);
                }
            }
        }
    }

    return sample;
}

unordered_map<int, int> graph_analyzer::get_distances_in_subset(const int v, const set<int>& allowed)
{
    unordered_map<int, int> dist;
    queue<int> q;
    if (!allowed.contains(v)) return dist;

    dist[v] = 0;
    q.push(v);

    if (g.type == Directed && rg.empty())
        rg = g.get_reversed();

    while (!q.empty()) {
        int cur = q.front(); q.pop();

        for (int to : g[cur]) {
            if (allowed.contains(to) && !dist.contains(to)) {
                dist[to] = dist[cur] + 1;
                q.push(to);
            }
        }

        if (g.type == Directed) {
            for (int to : rg[cur]) {
                if (allowed.contains(to) && !dist.contains(to)) {
                    dist[to] = dist[cur] + 1;
                    q.push(to);
                }
            }
        }
    }
    return dist;
}

size_t graph_analyzer::estimate_diameter_of_max_CC_from_snowball(const int target_size) {
    const auto snowball = build_snowball_sample(target_size);
    if (snowball.size() < 2) return 0;

    auto dists = pairwise_distances_in_subset(snowball);
    if (dists.empty()) return 0;
    return *ranges::max_element(dists);
}

double graph_analyzer::estimate_90th_percentile_of_max_CC_from_snowball(const int target_size) {
    const auto snowball = build_snowball_sample(target_size);
    if (snowball.size() < 2) return 0.0;

    auto dists = pairwise_distances_in_subset(snowball);
    if (dists.empty()) return 0.0;

    ranges::sort(dists);
    const size_t idx = (dists.size() - 1) * 9 / 10;
    return dists[idx];
}

vector<int> graph_analyzer::pairwise_distances_in_component(const vector<int>& sample) {
    vector<int> result;
    const size_t k = sample.size();
    result.reserve(k * (k - 1) / 2);
    for (int i = 0; i < k; ++i) {
        auto dist_map = get_distances_from(sample[i]);
        for (int j = i + 1; j < k; ++j) {
            if (auto it = dist_map.find(sample[j]); it != dist_map.end()) {
                result.push_back(it->second);
            }
        }
    }
    return result;
}

vector<int> graph_analyzer::pairwise_distances_in_subset(const set<int>& subset) {
    const vector vertices(subset.begin(), subset.end());
    vector<int> result;
    const size_t n = vertices.size();
    result.reserve(n * (n - 1) / 2);
    for (int i = 0; i < n; ++i) {
        auto dmap = get_distances_in_subset(vertices[i], subset);
        for (int j = i + 1; j < n; ++j) {
            if (auto it = dmap.find(vertices[j]); it != dmap.end()) {
                result.push_back(it->second);
            }
        }
    }
    return result;
}


