#include <iostream>
#include <filesystem>
#include <thread>

#include "src/analyzer.h"
#include "src/parsers/parser.h"
#include "src/summarizer/summarizer.h"
#include "tests/tests.h"

using namespace summarizer;

const string project_path = filesystem::current_path().parent_path();
const string datasets_path = project_path + "/datasets/";
const string summarized_path = project_path + "/summarized/";

static json output_json;
static string output_file;

enum dataset {
    socwiki, google, notredame, stanford,
    wiki, astro, coauthors, grqc,
    email, git, youtube, vk, orkut,
};
map<dataset, string> paths = {
    {socwiki,   "directed/soc-wiki-Vote.mtx"},
    {google,    "directed/web-Google.txt"},
    {notredame, "directed/web-NotreDame.txt"},
    {stanford,  "directed/web-Stanford.txt"},
    {wiki,      "directed/Wiki-Vote.txt"},

    {astro,     "undirected/CA-AstroPh.txt"},
    {coauthors, "undirected/ca-coauthors-dblp.txt"},
    {grqc,      "undirected/CA-GrQc.txt"},
    {email,     "undirected/Email-EuAll.txt"},
    {git,       "undirected/musae_git_edges.csv"},

    {orkut,     "very_large_graphs/com-orkut.ungraph.txt"},
    {youtube,   "very_large_graphs/com-youtube.ungraph.txt"},
    {vk,        "very_large_graphs/vk.csv"}
};
string get_dataset_path(const dataset ds) {
    return datasets_path + paths[ds];
}
string get_summarized_path(const dataset ds) {
    return summarized_path + filesystem::path(paths[ds]).replace_extension(".json").string();
}
string get_dataset_name(const dataset ds) {
    return filesystem::path(paths[ds]).replace_extension("");
}
static bool measure_is_finished;

void print_measure_time(const string& name, const long ms, const auto& result = "") {
    std::ostringstream oss;
    oss << fixed << result;
    cout << '\r' << left << std::setw(40) << name
         << setw(30) << oss.str()
         << setw(10) << (std::to_string(ms) + " ms");
    flush(cout);
}
void loop_print_measure_time(const string& name) {
    const auto start = std::chrono::steady_clock::now();
    while (true) {
        const auto end = std::chrono::steady_clock::now();
        const auto ms = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        print_measure_time<string>(name, ms);
        if (measure_is_finished)
            break;
        this_thread::sleep_for(chrono::milliseconds(20));
    }
}

template <typename Func>
auto measure(const measure_type type, const string& name, Func&& func) {
    measure_is_finished = false;
    thread t_print(loop_print_measure_time, name); // Запускаем отдельный поток с циклом

    const auto start = std::chrono::steady_clock::now();
    auto result = std::forward<Func>(func)();
    const auto end = std::chrono::steady_clock::now();
    const auto ms = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    measure_is_finished = true;
    t_print.join();
    print_measure_time(name, ms, result);

    if (type != undefined_field) {
        get_json_placing(output_json, type) = result;
        json_write(output_json, output_file, true);
    }
    cout << endl;
    return result;
}

void parse_example(dataset dgraph) {
    graph g;
    graph_analyzer analyzer(g);

    output_file = get_summarized_path(dgraph);
    const auto start = std::chrono::steady_clock::now();

    measure(graph_name, "graph name",                                              [&]{ return filesystem::path(get_dataset_path(dgraph)).filename(); });
    measure(undefined_field, "parsing",                                            [&] { g = uni_parser::parse(get_dataset_path(dgraph)); return "----- start tests -----";});

    // Base graph data
    measure(graph_type, "graph type",                                                   [&]{ return g.type == Directed ? "Directed" : "Undirected"; });
    measure(amount_of_vertexes, "amount vertexes",                                 [&] { return g.amount_vertexes(); });
    measure(amount_of_edges, "amount edges",                                       [&] { return g.amount_edges; });
    if (g.type == Undirected) {
        measure(min_degree, "min degree",                                          [&] { return analyzer.get_min_degree(); });
        measure(max_degree, "max degree",                                          [&] { return analyzer.get_max_degree(); });
        measure(average_degree, "average degree",                                  [&]{ return analyzer.get_average_degree(); });
    }

    measure(density, "density",                                                    [&]{ return analyzer.get_density(); });

    measure(amount_of_triangles, "amount of triangles",                            [&] { return analyzer.get_amount_of_triangles(); });
    measure(amount_of_closed_triplets, "amount of closed triplets",                [&] { return analyzer.get_amount_of_closed_triplets(); });
    measure(amount_of_opened_triplets, "amount of opened triplets",                [&] { return analyzer.get_amount_of_opened_triplets(); });
    measure(global_clustering_coefficient, "global cluster coef",                  [&]{ return analyzer.get_global_clustering_coefficient(); });

    // CC
    measure(amount_of_CCs, "amount of CC",                                         [&] { return analyzer.get_amount_of_CC(); });
    measure(fraction_of_vertexes_in_max_CC, "fraction of ver in max CC",           [&]{ return analyzer.get_fraction_of_vertexes_in_max_CC(); });

    measure(average_clustering_coefficient, "average cluster coef",                [&]{ return analyzer.get_average_clustering_coefficient(); });
    measure(average_clustering_coefficient_in_max_CC, "avr cluster coef in max CC",[&]{ return analyzer.get_average_clustering_coefficient_max_CC(); });
    if (g.type == Undirected) {
        measure(double_sweep_diameter, "double sweep diameter",                        [&] { return analyzer.estimate_diameter_of_max_CC_from_double_sweep(); });
        measure(sample_diameter, "sample diameter",                                    [&] { return analyzer.estimate_diameter_of_max_CC_from_sample(); });
        measure(snowball_diameter, "snowball diameter",                                [&] { return analyzer.estimate_diameter_of_max_CC_from_snowball(); });
        measure(sample_90_percentile, "sample 90 percentile",                          [&] { return analyzer.estimate_90th_percentile_of_max_CC_from_sample(); });
        measure(snowball_90_percentile, "snowball 90 percentile",                      [&] { return analyzer.estimate_90th_percentile_of_max_CC_from_snowball(); });

    }

    // SCC
    if (g.type == Directed) {
        measure(amount_of_SCCs, "amount of SCC",                                   [&] { return analyzer.get_amount_of_SCC(); });
        measure(fraction_of_vertexes_in_max_SCC, "fraction of ver in max SCC",     [&]{ return analyzer.get_fraction_of_vertexes_in_max_SCC(); });
    }

    // Warning! These functions break the graph
    if (g.type == Undirected) {
        measure(probability_that_random_vertex_has_degree_less_than_some_degree, "probab. that v. has degree less",
                [&] { return analyzer.get_probabilities_that_random_vertex_has_less_than_some_degree(); });
    }

    graph g_copy = g;
    measure(sizes_of_max_CC_after_delete_x_percent_random_vertexes, "delete 0% - 100% random vertexes",
            [&] { return analyzer.get_sizes_of_max_CC_after_delete_x_percentage_vertexes(); });

    if (g.type == Undirected) {
        measure(sizes_of_max_CC_after_delete_x_percent_max_degreed_vertexes, "delete 0% - 100% max degreed vertexes",
                [&] { return graph_analyzer(g_copy).get_sizes_of_max_CC_after_delete_x_percentage_vertexes_of_max_degrees(); });
    }

    const auto end = std::chrono::steady_clock::now();
    const auto ms = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << endl; // для привлечения внимания, что следующая строка - не результат теста
    measure(undefined_field, "Total execution time",   [&]{ return to_string(ms) + " ms"; });
}

void parse_examples() {
    for (const auto& p : paths) {
        bool is_skip = false;
        if (filesystem::exists(get_summarized_path(p.first))) {
            char c;
            while (true) {
                cout << "Graph " << p.second << " is already computed!" << endl;
                cout << "Do you want to skip it?(y/n): ";
                cin >> c;
                if (c == 'y' || c == 'Y') {
                    is_skip = true;
                    break;
                }
                else if (c == 'n' || c == 'N') break;
            }
        }
        if (is_skip) continue;

        output_json = json();
        output_file = "";
        parse_example(p.first);
    }
}
void print_available_datasets() {
    int i = 0;
    for (auto& p : paths) {
        cout << i++ << ") "<< get_dataset_name(p.first) << endl;
    }
}
dataset dataset_choose() {
    string num_str;
    int num;
    while (true) {
        print_available_datasets();
        cout << "Choose a dataset (0 - " << paths.size() - 1 << "): ";
        cin >> num_str;
        try {
            num = std::stoi(num_str);
            if (num < 0 || num >= paths.size()) throw runtime_error("...");
            break;
        } catch(...) {
            cout << "Incorrect number!" << endl;
        }
    }
    int i = 0;
    for (auto& el : paths) {
        if (i == num) {
            cout << "Successfully graph `" << get_dataset_name(el.first) << "` was chosen!" << endl;
            return el.first;
        }
        i++;
    }
    cout << 2 << endl;
    throw runtime_error("dataset_choose: Undefined error!");
}

void landmarks_basic_research() {
    const auto path = get_dataset_path(dataset_choose());
    graph g = uni_parser::parse(path);
    graph_analyzer analyzer(g);

    vector<function<void(int)>> methods;
    vector<string> methods_names;

    methods.emplace_back([&](int x){ analyzer.landmarks_basic_precompute_random(x); });
    methods.emplace_back([&](int x){ analyzer.landmarks_basic_precompute_highest_degrees(x); });
    methods.emplace_back([&](int x){ analyzer.landmarks_basic_precompute_best_coverage(x); });
    methods_names.emplace_back("choosing (random)");
    methods_names.emplace_back("choosing (highest degrees)");
    methods_names.emplace_back("choosing (best coverage)");

    auto results = vector(methods.size(), vector<size_t>());
    auto real_results = vector<size_t>();
    vector shuffled_vertexes(g.vertexes.begin(), g.vertexes.end());
    other::shuffle_vector(shuffled_vertexes);

    const int amount_vertexes = min(500, static_cast<int>(g.amount_vertexes()));
    int k;
    cout << "Enter amount of landmarks: ";
    cin >> k;

    for (int j = 0; j < amount_vertexes; j++) {
        int p1 = shuffled_vertexes[2 * j];
        int p2 = shuffled_vertexes[2 * j + 1];
        size_t real_dist = analyzer.landmarks_get_shortest_path(p1, p2).size() - 2;
        real_results.push_back(real_dist);
    }
    cout << "Real distances precalculated!" << endl;
    for (int i = 0; i < methods.size(); i++) {
        methods[i](k);
        for (int j = 0; j < amount_vertexes; j++) {
            int p1 = shuffled_vertexes[2 * j];
            int p2 = shuffled_vertexes[2 * j + 1];
            size_t dist = analyzer.landmarks_basic(p1, p2);
            results[i].push_back(dist);
        }
        cout << "Method " << methods_names[i] << " calculated!" << endl;
    }
    cout << endl;
    for (int i = 0; i < methods.size(); i++) {
        int count = 0;
        for (int j = 0; j < amount_vertexes; j++) {
            if (results[i][j] != UINT_MAX) ++count;
        }
        cout << "Method " << methods_names[i]  << " calculated " << count << "/" << amount_vertexes << " distances" << endl;
    }
    cout << endl;
    auto minimals = vector(methods.size(), 0);
    for (int j = 0; j < amount_vertexes; j++) {
        size_t mn_dist = UINT_MAX;
        for (int i = 0; i < methods.size(); i++) {
            mn_dist = min(mn_dist, results[i][j]);
        }
        for (int i = 0; i < methods.size(); i++) {
            if (mn_dist != UINT_MAX && mn_dist == results[i][j]) ++minimals[i];
        }
    }
    for (int i = 0; i < methods.size(); i++) {
        cout << "Method " << methods_names[i] << " has " << minimals[i] << "/" << amount_vertexes << " of minimum distances" << endl;
    }
    cout << endl;
    for (int i = 0; i < methods.size(); i++) {
        size_t size = 0;
        size_t real_size = 0;
        for (int j = 0; j < amount_vertexes; j++) {
            if (results[i][j] == UINT_MAX) continue;
            size += results[i][j];
            real_size += real_results[j];
        }
        cout << "Method " << methods_names[i] << " has approximation error: " << static_cast<double>(size - real_size) / static_cast<double>(size) << endl;
    }
}
int main() {

    // Tests work only in DEBUG build
    analyzer_tests::tests();
    graph_tests::tests();

    // parse_examples();

    landmarks_basic_research();

    return 0;
}
