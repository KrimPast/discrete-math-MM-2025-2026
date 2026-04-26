#include "analyzer.h"

double graph_analyzer::get_density() const {
    size_t max_edges = (g.amount_vertexes / 2) * (g.amount_vertexes - 1);
    return (double)g.amount_edges / (double)max_edges;
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
    return (double)mx / (double)g.size();
}

double graph_analyzer::get_fraction_of_vertexes_in_max_SCC() {
    auto components = get_SCCs();
    size_t mx = 0;
    for (auto &component : components) {
        mx = max(mx, component.size());
    }
    return (double)mx / (double)g.size();
}

double graph_analyzer::get_local_clustering_coefficient(int v) {
    if (g.type == Undefined) {
        throw runtime_error("get_local_clustering_coefficient: Cannot on undefined graph!\n");
    }
    set<int> neighbourhood_set(g[v].begin(), g[v].end());

    if (g.type == Directed) {
        if (rg.empty()) rg = g.get_reversed();
        neighbourhood_set.insert(rg[v].begin(), rg[v].end());
    }
    vector<int> neighbourhood_list(neighbourhood_set.begin(), neighbourhood_set.end());
    neighbourhood_set.clear();

    size_t count = get_amount_of_closed_triplets(v, neighbourhood_list);
    if (g.type == Undirected) count *= 2;

    size_t neighbours = neighbourhood_list.size();
    size_t max_count = neighbours * (neighbours - 1);
    if (max_count == 0) return 0; // means vertex doesn't have third neighbor
    return (double)count / (double) max_count;
}

double graph_analyzer::get_global_clustering_coefficient() const {
    size_t opened_triplets = get_amount_of_opened_triplets();
    size_t closed_triplets = get_amount_of_closed_triplets();
    size_t triplets = opened_triplets + closed_triplets;
    return (double)closed_triplets / (double)triplets;
}

double graph_analyzer::get_average_clustering_coefficient() {
    double amount = 0;
    auto vertexes = g.get_vertexes();
    for (auto v : vertexes) {
        amount += get_local_clustering_coefficient(v);
    }
    return amount / (double) vertexes.size();
}

size_t graph_analyzer::get_amount_of_triangles() const {
    static size_t triangles;
    if (!g.is_changed)
        return triangles;
    g.is_changed = false;
    triangles = get_amount_of_closed_triplets() * 3;
    return triangles;
}

// CC means Connected Components
void graph_analyzer::CC_undirected_dfs(int v) {
    for (auto o : g[v]) {
        if (CC_comp_id[o] != -1) continue;
        CC_comp_id[o] = CC_comp_id[v];
        CC_undirected_dfs(o);
    }
}
void graph_analyzer::CC_directed_dfs(int v) {
    CC_undirected_dfs(v);
    for (auto o : rg[v]) {
        if (CC_comp_id[o] != -1) continue;
        CC_comp_id[o] = CC_comp_id[v];
        CC_directed_dfs(o);
    }
}
vector<set<int>> graph_analyzer::get_CCs() {
    auto v_list = g.get_vertexes();

    g.calculate_amount_of_vertexes();
    CC_comp_id.reserve(g.amount_vertexes);
    for (auto v : v_list) CC_comp_id[v] = -1;

    int c = 0;
    if (g.type == Undirected) {
        // For search CC in undirected graph just use DFS
        for (auto v : v_list) {
            if (CC_comp_id[v] != -1) continue;
            CC_comp_id[v] = c++;
            CC_undirected_dfs(v);
        }
    }
    else if (g.type == Directed) {
        // For search CC in dir-graph, firstly, init reversed graph and, secondly, use DFS on both graphs
        if (rg.empty()) rg = g.get_reversed();

        for (auto v : v_list) {
            if (CC_comp_id[v] != -1) continue;
            CC_comp_id[v] = c++;
            CC_directed_dfs(v);
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
    size_t n = SCC_order.size();
    for (int i = 0; i < n; i++) {
        int v = SCC_order[n - i - 1];
        if (!SCC_visited[v]) {
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
void graph_analyzer::SCC_dfs1(int v) {
    SCC_visited[v] = true;
    for (auto o : g[v])
        if (!SCC_visited[o])
            SCC_dfs1(o);
    SCC_order.push_back(v);
}
void graph_analyzer::SCC_dfs2(int v) {
    SCC_visited[v] = true;
    SCC_component.insert(v);
    for (auto o : rg[v])
        if (!SCC_visited[o])
            SCC_dfs2(o);
}

size_t graph_analyzer::get_amount_of_opened_triplets() const {
    size_t amount = 0;
    auto vertexes = g.get_vertexes();
    for (auto v : vertexes) {
        amount += get_amount_of_opened_triplets(v);
    }
    return amount;
}

size_t graph_analyzer::get_amount_of_opened_triplets(int v) const {
    vector<int>& neighbourhood = g[v];
    size_t count = 0;
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
    if (last_amount_vertexes == g.amount_vertexes) {
        return last_closed_triplets;
    }

    size_t amount = 0;
    auto vertexes = g.get_vertexes();
    for (auto v : vertexes) {
        amount += get_amount_of_closed_triplets(v);
    }
    last_amount_vertexes = g.amount_vertexes;
    last_closed_triplets = amount;
    return amount;
}

size_t graph_analyzer::get_amount_of_closed_triplets(int v) const {
    vector<int>& neighbourhood = g[v];
    return get_amount_of_closed_triplets(v, neighbourhood);
}
size_t graph_analyzer::get_amount_of_closed_triplets(int v, const vector<int>& neighbourhood) const {
    size_t count = 0;
    for (int j = 0; j < neighbourhood.size(); j++) {
        size_t k_start = g.type == Undirected ? j + 1 : 0;
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

size_t graph_analyzer::get_degree(int v) const {
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
    auto vertexes = g.get_vertexes();
    size_t mx = 0;
    for (auto v : vertexes) {
        mx = max(mx, get_degree(v));
    }
    return mx;
}

double graph_analyzer::get_average_degree() const {
    auto vertexes = g.get_vertexes();
    size_t sm = 0;
    for (auto v : vertexes) {
        sm += get_degree(v);
    }
    g.calculate_amount_of_vertexes();
    return (double)sm / (double)g.amount_vertexes;
}

// Function return probability, which enters in [0, 1], what means random vertex has degree, which equals input degree
double graph_analyzer::get_probability_that_random_vertex_has_some_degree(size_t degree) {
    if (degrees_counter.empty()) init_degree_counters_cache();
    if (g.amount_vertexes == 0) g.calculate_amount_of_vertexes();

    return (double)degrees_counter[degree] / (double)g.amount_vertexes;
}

// Function returns log2(probability), which enters in (-infinity, 0], what means random vertex has degree, which enters in...
// ... [2 ^ log2_degree,  2 ^ (log2_degree + 1) )
double graph_analyzer::get_probability_that_random_vertex_has_some_degree_log_log(size_t log2_degree) {
    if (degrees_counter.empty()) init_degree_counters_cache();
    if (g.amount_vertexes == 0) g.calculate_amount_of_vertexes();

    size_t min_degree = 1 << log2_degree;
    size_t max_degree = min_degree * 2;
    size_t amount = 0;
    for (size_t degree = min_degree; degree < max_degree; degree++) {
        amount += degrees_counter[degree];
    }
    return std::log2((double) amount / (double)g.amount_vertexes);
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
    if (vertexes.empty()) return vertexes;

    set<int> &max_CC = CCs[0];
    for (auto &CC: CCs) {
        if (CC.size() > max_CC.size()) {
            max_CC = CC;
        }
    }
    return max_CC;
}

size_t graph_analyzer::get_size_of_max_CC_after_delete_x_percentage_vertexes(double x){
    if (x < 0 || x > 1) throw runtime_error("X must be between 0 and 1");
    if (x == 1) return 0;

    g.calculate_amount_of_vertexes();
    auto deleting_amount = (size_t)(x * (float)g.amount_vertexes);
    auto deleting = other::get_random_n_elements_from_set(g.get_vertexes(), deleting_amount);
    for (auto v : deleting) {
        g.remove_vertex(v);
    }
    return get_max_CC().size();
}

size_t graph_analyzer::get_size_of_max_CC_after_delete_x_percentage_vertexes_of_max_degrees(double x) {
    if (x < 0 || x > 1) throw runtime_error("X must be between 0 and 1");
    if (x == 1) return 0;

    g.calculate_amount_of_vertexes();
    auto deleting_amount = (size_t)(x * (float)g.amount_vertexes);
    init_degree_counters_cache();

    sort(degrees_vector.begin(), degrees_vector.end(), other::degree_greater);
    size_t deleted = 0;
    while (deleted < deleting_amount) {
        g.remove_vertex(degrees_vector[deleted].second);
        ++deleted;
    }

    return get_max_CC().size();
}
