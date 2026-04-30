#include <omp.h>
#include <atomic>
#include <queue>

#include "analyzer.h"

#include <chrono>

double graph_analyzer::get_density() const {
    const size_t max_edges = g.amount_vertexes() * (g.amount_vertexes() - 1) / 2;
    return static_cast<double>(g.amount_edges) / static_cast<double>(max_edges);
}

size_t graph_analyzer::get_amount_of_CC() {
    return get_CCs().size();
}
size_t graph_analyzer::get_amount_of_SCC() {
    return get_SCCs().size();
}

double graph_analyzer::get_fraction_of_vertexes_in_max_CC() {
    const auto components = get_CCs();
    size_t mx = 0;
    for (auto &component : components) {
        mx = max(mx, component.size());
    }
    return static_cast<double>(mx) / static_cast<double>(g.size());
}

double graph_analyzer::get_fraction_of_vertexes_in_max_SCC() {
    const auto components = get_SCCs();
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
    static unordered_map<int, double> cache;
    static size_t last_amount_edges = 0;
    if (last_amount_edges != g.amount_edges) {
        cache.clear();
    } else if (cache.contains(v)) {
        return cache[v];
    }

    // Объяснение, что здесь происходит:
    // https://en.wikipedia.org/wiki/Clustering_coefficient#Local_clustering_coefficient

    unordered_set<int> neighborhood = g[v];
    if (g.type == Directed) {
        if (rg.empty()) rg = g.get_reversed();
        neighborhood.insert_range(rg[v]);
    }

    size_t count = get_amount_of_closed_triplets(neighborhood);
    const size_t neighbours = neighborhood.size();
    size_t max_count = neighbours * (neighbours - 1);
    if (g.type == Undirected) max_count /= 2;

    if (max_count == 0) return 0; // means vertex doesn't have third neighbor
    cache[v] = static_cast<double>(count) / static_cast<double>(max_count);
    last_amount_edges = g.amount_edges;

    return cache[v];
}

double graph_analyzer::get_global_clustering_coefficient() const {
    const size_t opened_triplets = get_amount_of_opened_triplets();
    const size_t closed_triplets = get_amount_of_closed_triplets();
    const size_t triplets = opened_triplets + closed_triplets;
    return static_cast<double>(closed_triplets) / static_cast<double>(triplets);
}

double graph_analyzer::get_average_clustering_coefficient() {
    double amount = 0;
    for (const auto v : g.vertexes) {
        amount += get_local_clustering_coefficient(v);
    }
    return amount / static_cast<double>(g.amount_vertexes());
}

size_t graph_analyzer::get_amount_of_triangles() const {
    return get_amount_of_closed_triplets() * 3;
}

// CC means Connected Components
void graph_analyzer::CC_undirected_bfs(const int v) {
    queue<int> q;
    q.push(v);
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        for (int i : g[cur]) {
            if (CC_comp_id[i] == -1) {
                CC_comp_id[i] = CC_comp_id[cur];
                q.push(i);
            }
        }
    }
}
void graph_analyzer::CC_directed_bfs(const int v) {
    queue<int> q;
    q.push(v);
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        for (int i : g[cur]) {
            if (CC_comp_id[i] == -1) {
                CC_comp_id[i] = CC_comp_id[cur];
                q.push(i);
            }
        }
        for (int i : rg[cur]) {
            if (CC_comp_id[i] == -1) {
                CC_comp_id[i] = CC_comp_id[cur];
                q.push(i);
            }
        }
    }
}

vector<set<int>> graph_analyzer::get_CCs() {
    CC_comp_id.reserve(g.amount_vertexes());
    for (auto v : g.vertexes) CC_comp_id[v] = -1;

    int c = 0;
    if (g.type == Undirected) {
        for (auto v : g.vertexes) {
            if (CC_comp_id[v] != -1) continue;
            CC_comp_id[v] = c++;
            CC_undirected_bfs(v);
        }
    }
    else if (g.type == Directed) {
        if (rg.empty()) rg = g.get_reversed();

        for (auto v : g.vertexes) {
            if (CC_comp_id[v] != -1) continue;
            CC_comp_id[v] = c++;
            CC_directed_bfs(v);
        }
    }

    auto components_list = vector<set<int>>(c);
    for (auto v : g.vertexes) {
        components_list[CC_comp_id[v]].insert(v);
    }
    CC_comp_id.clear();
    return components_list;
}

vector<set<int>> graph_analyzer::get_SCCs() {
    if (g.type != Directed) throw runtime_error("get_SCCs: SCCs exists only in directed graph!");
    if (rg.empty()) rg = g.get_reversed();
    for (auto v : g.vertexes) {
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
    static size_t last_vertexes = 0;
    static atomic_size_t last_amount = 0;

    if (last_vertexes == g.amount_vertexes()) {
        return last_amount;
    }
    last_amount = 0;
    last_vertexes = g.amount_vertexes();

    const vector vertexes_list(g.vertexes.begin(), g.vertexes.end());
#pragma omp parallel for default(none) shared(vertexes_list, last_amount, g)
    for (const auto v : vertexes_list) {
        last_amount += get_amount_of_opened_triplets(v);
    }
    return last_amount;
}

size_t graph_analyzer::get_amount_of_opened_triplets(const int v) const {
    const auto& neighbourhood = g[v];
    atomic_size_t count = 0;
// #pragma omp parallel for default(none) shared(neighbourhood, g, v, count)
    for (auto second : neighbourhood) {
        for (auto third : g[second]) {
            if (v == third) continue;
            if (!neighbourhood.contains(third)) {
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
    for (const auto v : g.vertexes) {
        amount += get_amount_of_closed_triplets(v);
    }
    last_amount_vertexes = g.amount_vertexes();
    last_closed_triplets = amount;
    return amount;
}

size_t graph_analyzer::get_amount_of_closed_triplets(const int v) const {
    return get_amount_of_closed_triplets(g[v]);
}
size_t graph_analyzer::get_amount_of_closed_triplets(const unordered_set<int>& neighbourhood) const {
    size_t count = 0;
    for (auto second : neighbourhood) {
        // atomic_size_t k_start = g.type == Undirected ? j + 1 : 0;
        // #pragma omp parallel for default(none) shared(neighbourhood, g, count, j, k_start)
        for (auto third : neighbourhood) {
            if (g[second].contains(third)) {
                ++count;
            }
        }
    }
    return g.type == Undirected ? count / 2 : count;
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



size_t graph_analyzer::get_degree(const int v) const {
    if (g.type == Undirected) {
        if (!g.contains(v)) throw runtime_error("get_degree: No such vertex in graph");
        return g[v].size() + (g[v].contains(v) ? 1 : 0); // reflexive adds two to degree
    }
    throw runtime_error("get_degree: Not implemented for graphs this type");
}

size_t graph_analyzer::get_min_degree() const {
    size_t mn = g.amount_edges;
    for (const auto v : g.vertexes) {
        mn = min(mn, get_degree(v));
    }
    return mn;
}

size_t graph_analyzer::get_max_degree() const {
    size_t mx = 0;
    for (const auto v : g.vertexes) {
        mx = max(mx, get_degree(v));
    }
    return mx;
}

double graph_analyzer::get_average_degree() const {
    size_t sm = 0;
    for (const auto v : g.vertexes) {
        sm += get_degree(v);
    }
    return static_cast<double>(sm) / static_cast<double>(g.amount_vertexes());
}

// Function return probability, which enters in [0, 1], what means random vertex has degree, which equals input degree
double graph_analyzer::get_probability_that_random_vertex_has_some_degree(const size_t degree) {
    if (degrees_counter.empty()) init_degree_counters_cache();

    return static_cast<double>(degrees_counter[degree]) / static_cast<double>(g.amount_vertexes());
}
double graph_analyzer::get_probability_that_random_vertex_has_less_than_some_degree(const size_t degree) {
    double s = 0;
    for (size_t i = 1; i <= degree; i++){
        s += get_probability_that_random_vertex_has_some_degree(i);
    }
    return s;
}

json graph_analyzer::get_probabilities_that_random_vertex_has_less_than_some_degree() {
    if (degrees_vector.empty()) init_degree_counters_cache();

    ranges::sort(degrees_vector, other::degree_greater);
    json probabilities;
    if (degrees_vector.empty()) return probabilities;

    const int max_degree = get_max_degree();
    vector<size_t> degrees_per_step = {1};
    size_t i = 1;
    while (i <= max_degree) {
        i *= 2;
        degrees_per_step.push_back(i);
    } ;
    for (const auto degree : degrees_per_step) {
        probabilities[to_string(degree)] = get_probability_that_random_vertex_has_less_than_some_degree(degree);
    }
    return probabilities;
}

// Function returns log2(probability), which enters in (-infinity, 0], what means random vertex has degree, which enters in...
// ... [2 ^ log2_degree,  2 ^ (log2_degree + 1) )
double graph_analyzer::get_probability_that_random_vertex_has_some_degree_log_log(const size_t log2_degree) {
    if (degrees_counter.empty()) init_degree_counters_cache();

    const size_t min_degree = 1 << log2_degree;
    const size_t max_degree = min_degree * 2;
    size_t amount = 0;
    for (size_t degree = min_degree; degree < max_degree; degree++) {
        amount += degrees_counter[degree];
    }
    return log2(static_cast<double>(amount) / static_cast<double>(g.amount_vertexes()));
}


void graph_analyzer::init_degree_counters_cache() {
    degrees_counter.clear();
    degrees_vector.clear();
    for (auto v : g.vertexes) {
        size_t degree = get_degree(v);
        ++degrees_counter[degree];
        degrees_vector.emplace_back(degree, v);
    }
}

set<int> graph_analyzer::get_max_CC() {
    auto CCs = get_CCs();
    if (CCs.empty()) return {};

    auto &max_CC = CCs[0];
    for (auto &CC: CCs) {
        if (CC.size() > max_CC.size()) {
            max_CC = CC;
        }
    }
    return max_CC;
}

json graph_analyzer::get_sizes_of_max_CC_after_delete_x_percentage_vertexes() {
    json sizes = {{"0%", get_max_CC().size()}};
    constexpr int steps = 10;
    const long to_delete = g.amount_vertexes() / 10;
    vector deletable_list(g.vertexes.begin(), g.vertexes.end());

    other::shuffle_vector(deletable_list);
    int ind = 0;
    for (int i = 1; i <= steps; ++i) {
        long deleted_on_step = (g.amount_vertexes() < to_delete) ? g.amount_vertexes() : to_delete;
        while (deleted_on_step > 0) {
            int el = deletable_list[ind++];

            long amount_before = g.amount_vertexes();
            remove_vertex(el); // This thing also can delete of neighbors of "el" in special case
            long amount_after = g.amount_vertexes();

            deleted_on_step -= (amount_before - amount_after);
        }
        sizes[to_string(i * steps) + "%"] = get_max_CC().size();
    }
    return sizes;
}
json graph_analyzer::get_sizes_of_max_CC_after_delete_x_percentage_vertexes_of_max_degrees() {
    if (g.type != Undirected) throw runtime_error("get_sizes_of_max_CC_after_delete_x_percentage_vertexes_of_max_degrees: You can use this function only on undirected graph!");
    json sizes = {{"0%", get_max_CC().size()}};
    size_t initial_size = g.amount_vertexes();
    vector<double> percentage_to_delete_on_each_step = {0.005, 0.005, 0.01, 0.01, 0.01, 0.01, 0.025, 0.025, 0.05, 0.1, 0.1, 0.15};
    vector<double> percentage_to_delete_sum = vector(percentage_to_delete_on_each_step.size(), -1.0);
    percentage_to_delete_sum[0] = percentage_to_delete_on_each_step[0];
    for (int i = 1; i < percentage_to_delete_on_each_step.size(); ++i) {
        percentage_to_delete_sum[i] = percentage_to_delete_sum[i - 1] + percentage_to_delete_on_each_step[i];
    }

    init_degree_counters_cache();
    ranges::sort(degrees_vector, other::degree_greater);
    int ind = 0;
    for (int i = 0; i < percentage_to_delete_on_each_step.size(); ++i) {
        long to_delete_on_step = min(static_cast<size_t>(initial_size * percentage_to_delete_on_each_step[i]), g.amount_vertexes()); // if deleted_on_step = average_delete_step
        long deleted = 0;
        while (deleted < to_delete_on_step) {
            remove_vertex(degrees_vector[ind++].second);
            ++deleted;
        }
        string string_percent = std::format("{:.1f}", percentage_to_delete_sum[i] * 100) + '%';
        sizes[string_percent] = get_max_CC().size();
    }
    return sizes;
}

double graph_analyzer::get_average_clustering_coefficient_max_CC() {
    const auto max_cc = get_max_CC();
    if (max_cc.empty()) return 0.0;

    double sum = 0.0;
    for (const int v : max_cc) {
        sum += get_local_clustering_coefficient(v);
    }
    return sum / static_cast<double>(max_cc.size());
}

size_t graph_analyzer::estimate_diameter_of_max_CC_from_double_sweep() {
    const auto max_CC = get_max_CC();
    if (max_CC.empty()) return 0;

    const int v = other::random_element(max_CC);

    auto [v1, d1] = find_farthest_vertex_by_bfs(v);
    auto [v2, d2] = find_farthest_vertex_by_bfs(v1);

    return static_cast<size_t>(d2);
}

void graph_analyzer::ensure_landmarks_built() {
    if (!landmarks_built) {
        build_landmarks();
        landmarks_built = true;
    }
}

void graph_analyzer::build_landmarks() {
    landmark_ids.clear();
    landmark_dist.clear();

    const auto max_cc = get_max_CC();
    if (max_cc.empty()) return;

    vector<pair<size_t, int>> deg_vertex;
    for (int v : max_cc) {
        deg_vertex.emplace_back(get_degree(v), v);
    }
    ranges::sort(deg_vertex, greater());

    landmark_ids.reserve(num_landmarks);
    for (auto &v: deg_vertex | views::values) {
        if (landmark_ids.size() >= num_landmarks) break;
        bool too_close = false;
        for (int chosen : landmark_ids) {
            if (g[v].contains(chosen) || g[chosen].contains(v)) {
                too_close = true;
                break;
            }
        }
        if (!too_close)
            landmark_ids.push_back(v);
    }

    if (landmark_ids.size() < num_landmarks) {
        for (auto &v: deg_vertex | views::values) {
            if (ranges::find(landmark_ids, v) == landmark_ids.end()) {
                landmark_ids.push_back(v);
                if (landmark_ids.size() >= num_landmarks) break;
            }
        }
    }

    for (int v : g.vertexes) {
        landmark_dist[v].assign(num_landmarks, -1);
    }

    for (int i = 0; i < landmark_ids.size(); ++i) {
        const int landmark = landmark_ids[i];
        for (auto dists = get_distances_from(landmark); auto &[v, d] : dists) {
            if (landmark_dist.contains(v)) {
                landmark_dist[v][i] = d;
            }
        }
    }
}

int graph_analyzer::estimate_distance(const int s, const int t) const {
    int best = INT_MAX;
    for (int i = 0; i < landmark_ids.size(); ++i) {
        const int ds = landmark_dist.at(s)[i];
        if (const int dt = landmark_dist.at(t)[i]; ds != -1 && dt != -1) {
            if (const int via = ds + dt; via < best) best = via;
        }
    }
    return (best == INT_MAX) ? -1 : best;
}

size_t graph_analyzer::estimate_diameter_of_max_CC_from_sample(const int sample_size) {
    ensure_landmarks_built();
    const auto max_cc = get_max_CC();
    if (max_cc.empty()) return 0;

    const int k = min(sample_size, static_cast<int>(max_cc.size()));
    const auto sample = other::get_random_n_elements_from_set(max_cc, k);

    size_t diameter = 0;
    for (size_t i = 0; i < sample.size(); ++i) {
        for (size_t j = i + 1; j < sample.size(); ++j) {
            if (const int est = estimate_distance(sample[i], sample[j]); est > static_cast<int>(diameter)) diameter = est;
        }
    }
    return diameter;
}

size_t graph_analyzer::estimate_90th_percentile_of_max_CC_from_sample(const int sample_size) {
    ensure_landmarks_built();
    const auto max_cc = get_max_CC();
    if (max_cc.empty()) return 0;

    const int k = min(sample_size, static_cast<int>(max_cc.size()));
    const auto sample = other::get_random_n_elements_from_set(max_cc, k);

    vector<int> dists;
    dists.reserve(k * (k - 1) / 2);
    for (size_t i = 0; i < sample.size(); ++i) {
        for (size_t j = i + 1; j < sample.size(); ++j) {
            if (int est = estimate_distance(sample[i], sample[j]); est >= 0) dists.push_back(est);
        }
    }
    if (dists.empty()) return 0;

    ranges::sort(dists);
    const size_t idx = (dists.size() - 1) * 9 / 10;
    return dists[idx];
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

    queue<int> q;
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

void graph_analyzer::remove_vertex(const int v) {
    if (!g.contains(v)) return;
    if (g.type == Undefined) throw runtime_error("remove_vertex: graph type is undefined");
    if (g.type == Undirected) {
        const auto& neighbors = g[v];
        while (!neighbors.empty()) {
            g.remove(v, *neighbors.begin());
        }
    }
    else { // type == Directed
        if (rg.empty()) rg = g.get_reversed();
        auto other_neigbours = rg[v];
        for (auto other : other_neigbours) {
            g.remove(other, v);
            rg.remove(v, other);
        }
        auto neighbors = g[v];
        for (auto other : neighbors) {
            g.remove(v, other);
            rg.remove(other, v);
        }
    }
    g.erase(v);
}

size_t graph_analyzer::estimate_diameter_of_max_CC_from_snowball(const int target_size) {
    ensure_landmarks_built();
    auto snowball = build_snowball_sample(target_size);
    if (snowball.size() < 2) return 0;

    const vector vertexes(snowball.begin(), snowball.end());
    size_t diameter = 0;
    for (size_t i = 0; i < vertexes.size(); ++i) {
        for (size_t j = i + 1; j < vertexes.size(); ++j) {
            if (const int est = estimate_distance(vertexes[i], vertexes[j]); est > static_cast<int>(diameter)) diameter = est;
        }
    }
    return diameter;
}

size_t graph_analyzer::estimate_90th_percentile_of_max_CC_from_snowball(const int target_size) {
    ensure_landmarks_built();
    auto snowball = build_snowball_sample(target_size);
    if (snowball.size() < 2) return 0;

    const vector vertexes(snowball.begin(), snowball.end());
    vector<int> dists;
    dists.reserve(vertexes.size() * (vertexes.size() - 1) / 2);
    for (size_t i = 0; i < vertexes.size(); ++i) {
        for (size_t j = i + 1; j < vertexes.size(); ++j) {
            if (int est = estimate_distance(vertexes[i], vertexes[j]); est >= 0) dists.push_back(est);
        }
    }
    if (dists.empty()) return 0;

    ranges::sort(dists);
    const size_t idx = (dists.size() - 1) * 9 / 10;
    return dists[idx];
}
