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
    email, git, orkut, youtube, vk
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

    measure(undefined_field, "parsing",                                            [&] { g = uni_parser::parse(get_dataset_path(dgraph)); return "----- start tests -----";});

    // Base graph data
    measure(graph_name, "graph name",                                              [&]{ return filesystem::path(get_dataset_path(dgraph)).filename(); });
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

int main() {
    // Tests work only in DEBUG build
    analyzer_tests::tests();
    graph_tests::tests();

    for (auto p : paths) {
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

    return 0;
}
