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
    auto real_graph_type = uni_parser::get_graph_type(output_file);

    measure(graph_name, "graph name",                                              [&]{ return filesystem::path(get_dataset_path(dgraph)).filename(); });
    measure(undefined_field, "parsing",                                            [&] { g = uni_parser::parse(get_dataset_path(dgraph), Undirected); return "----- start tests -----";});

    // Base graph data
    measure(graph_type, "graph type",                                                   [&]{ return real_graph_type == Directed ? "Directed" : "Undirected"; });
    measure(amount_of_vertexes, "amount vertexes",                                 [&] { return g.amount_vertexes(); });
    measure(amount_of_edges, "amount edges",                                       [&] { return g.amount_edges; });

    measure(min_degree, "min degree",                                          [&] { return analyzer.get_min_degree(); });
    measure(max_degree, "max degree",                                          [&] { return analyzer.get_max_degree(); });
    measure(average_degree, "average degree",                                  [&]{ return analyzer.get_average_degree(); });

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

    measure(double_sweep_diameter, "double sweep diameter",                        [&] { return analyzer.estimate_diameter_of_max_CC_from_double_sweep(); });
    measure(sample_diameter, "sample diameter",                                    [&] { return analyzer.estimate_diameter_of_max_CC_from_sample(); });
    measure(snowball_diameter, "snowball diameter",                                [&] { return analyzer.estimate_diameter_of_max_CC_from_snowball(); });
    measure(sample_90_percentile, "sample 90 percentile",                          [&] { return analyzer.estimate_90th_percentile_of_max_CC_from_sample(); });
    measure(snowball_90_percentile, "snowball 90 percentile",                      [&] { return analyzer.estimate_90th_percentile_of_max_CC_from_snowball(); });


    // SCC
    if (real_graph_type == Directed) {
        graph og = uni_parser::parse(get_dataset_path(dgraph), Directed);
        graph_analyzer oanalyzer(og);
        measure(amount_of_SCCs, "amount of SCC",                                   [&] { return oanalyzer.get_amount_of_SCC(); });
        measure(fraction_of_vertexes_in_max_SCC, "fraction of ver in max SCC",     [&]{ return oanalyzer.get_fraction_of_vertexes_in_max_SCC(); });
        og.clear();
    }

    // Warning! These functions break the graph
    measure(probability_that_random_vertex_has_degree_less_than_some_degree, "probab. that v. has degree less",
            [&] { return analyzer.get_probabilities_that_random_vertex_has_less_than_some_degree(); });

    measure(probability_that_random_vertex_has_degree_less_than_some_degree_in_log2_scale, "probab. that v. has log_degree <= smth",
            [&] { return analyzer.get_probabilities_that_random_vertex_has_less_than_some_degree_log_log(); });

    graph g_copy = g;
    measure(sizes_of_max_CC_after_delete_x_percent_random_vertexes, "delete 0% - 100% random vertexes",
            [&] { return analyzer.get_sizes_of_max_CC_after_delete_x_percentage_vertexes(); });

    measure(sizes_of_max_CC_after_delete_x_percent_max_degreed_vertexes, "delete 0% - 100% max degreed vertexes",
            [&] { return graph_analyzer(g_copy).get_sizes_of_max_CC_after_delete_x_percentage_vertexes_of_max_degrees(); });

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

void landmarks_research() {
    const auto path = get_dataset_path(dataset_choose());
    graph g = uni_parser::parse(path);
    graph_analyzer analyzer(g);

    vector<function<void(int)>> precompute_methods;
    vector<string> precompute_methods_names;

    vector<function<void(void)>> precompute_wrappers;
    vector<function<size_t(int, int)>> landmarks_type;

    precompute_methods.emplace_back([&](int k){ analyzer.landmarks_precompute_random(k); });
    precompute_methods.emplace_back([&](int k){ analyzer.landmarks_precompute_highest_degrees(k); });
    precompute_methods.emplace_back([&](int k){ analyzer.landmarks_precompute_best_coverage(k); });
    precompute_methods_names.emplace_back("choosing (random)");
    precompute_methods_names.emplace_back("choosing (highest degrees)");
    precompute_methods_names.emplace_back("choosing (best coverage)");

    precompute_wrappers.emplace_back([&](){ analyzer.landmarks_basic_precompute(); });
    precompute_wrappers.emplace_back([&](){ analyzer.landmarks_bfs_precompute(); });
    landmarks_type.emplace_back([&](int s, int t) { return analyzer.landmarks_basic(s, t); });
    landmarks_type.emplace_back([&](int s, int t) { return analyzer.landmarks_bfs(s, t); });

    // const int amount_vertexes = min(500, static_cast<int>(g.amount_vertexes()));

    int amount_vertexes, k;
    cout << "Enter amount of vertexes: ";
    cin >> amount_vertexes;

    size_t measurings = precompute_methods.size(); // =3
    auto real_results = vector<size_t>();
    vector random_vertexes(g.vertexes.begin(), g.vertexes.end());
    other::shuffle_vector(random_vertexes);

    // Calculating real distances
    cout << "Calculating real distances started!" << endl;
    for (int j = 0; j < amount_vertexes; j++) {
        int p1 = random_vertexes[2 * j];
        int p2 = random_vertexes[2 * j + 1];
        size_t real_dist = analyzer.landmarks_get_shortest_path(p1, p2).size() - 1;  // -1 because we measure length of path, not vertexes in path
        real_results.push_back(real_dist);
    }
    cout << "Calculating real distances ended!" << endl << endl;
    cout << "Enter amount of landmarks: ";
    cin >> k;
    cout << endl;

    // 0 is Landmarks-basic, 1 is Landmarks-BFS
    for (int l = 0; l <= 1; l++) {
        auto& landmarks = landmarks_type[l]; // function of measuring
        auto results = vector(measurings, vector<size_t>());

        cout << ">> Algorithm " << (l == 0 ? "Landmarks-Basic" : "Landmarks-BFS") << endl;

        for (int i = 0; i < measurings; i++) {
            precompute_methods[i](k); // it may be random, max-degrees or best coverage
            precompute_wrappers[l](); // it may be landmarks-basic's or landmarks-BFS's precompute

            for (int j = 0; j < amount_vertexes; j++) {
                int p1 = random_vertexes[2 * j];
                int p2 = random_vertexes[2 * j + 1];
                size_t dist = landmarks(p1, p2);
                results[i].push_back(dist);
            }
            cout << "Method " << precompute_methods_names[i]  << " calculated!" << endl;
        }
        cout << endl;
        auto minimals = vector(measurings, 0);
        for (int j = 0; j < amount_vertexes; j++) {
            size_t mn_dist = UINT_MAX;
            for (int i = 0; i < measurings; i++) {
                mn_dist = min(mn_dist, results[i][j]);
            }
            for (int i = 0; i < measurings; i++) {
                if (mn_dist != UINT_MAX && mn_dist == results[i][j]) ++minimals[i];
            }
        }
        for (int i = 0; i < measurings; i++) {
            cout << "Method " << precompute_methods_names[i] << " has " << minimals[i] << "/" << amount_vertexes << " of minimum distances" << endl;
        }
        cout << endl;
        for (int i = 0; i < measurings; i++) {
            size_t measured_size = 0;
            size_t real_size = 0;
            for (int j = 0; j < amount_vertexes; j++) {
                if (results[i][j] == UINT_MAX) continue;
                measured_size += results[i][j];
                real_size += real_results[j];
            }
            cout << "Method " << precompute_methods_names[i] << " has approximation error: " << static_cast<double>(measured_size - real_size) / static_cast<double>(measured_size) << endl;
        }
        cout << endl;
    }


}
int main() {
    // Tests work only in DEBUG build
    analyzer_tests::tests();
    graph_tests::tests();
    parse_examples();

    // landmarks_research();

    return 0;
}
