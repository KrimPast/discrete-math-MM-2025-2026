#include <iostream>
#include <filesystem>

#include "src/analyzer.h"
#include "src/parsers/parser.h"
#include "src/summarizer/summarizer.h"
#include "tests/tests.h"

const string project_path = filesystem::current_path().parent_path();
const string datasets_path = project_path + "/datasets/";
map<string, string> datasets = {
    {"socwiki",   datasets_path + "directed/soc-wiki-Vote.mtx"},
    {"google",    datasets_path + "directed/google.txt"},
    {"notredame", datasets_path + "directed/web-NotreDame.txt"},
    {"stanford",  datasets_path + "directed/web-Stanford.txt"},
    {"wiki",      datasets_path + "directed/Wiki-Vote.txt"},

    {"astro",     datasets_path + "undirected/CA-AstroPh.txt"},
    {"coauthors", datasets_path + "undirected/ca-coauthors-dblp.txt"},
    {"grqc",      datasets_path + "undirected/CA-GrQc.txt"},
    {"email",     datasets_path + "undirected/Email-EuAll.txt"},
    {"git",       datasets_path + "undirected/musae_git_edges.csv"},

    {"orkut",     datasets_path + "very_large_graphs/com-orkut.ungraph.txt"},
    {"youtube",   datasets_path + "very_large_graphs/com-youtube.ungraph.txt"},
    {"vk",        datasets_path + "very_large_graphs/vk.csv"}
};

template <typename Func>
auto measure(const string& name, Func&& func) {
    const auto start = std::chrono::steady_clock::now();
    auto result = std::forward<Func>(func)();
    const auto end = std::chrono::steady_clock::now();
    const auto ms = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::ostringstream oss;
    oss << fixed << result;
    cout << left << std::setw(40) << name
         << setw(30) << oss.str()
         << setw(10) << (std::to_string(ms) + " ms") << endl;
    return result;
}

void parse_example() {
    graph g;
    graph_analyzer analyzer(g);

    const string graph_name = "vk"; // Chose one

    measure("parsing", [&] { g = uni_parser::parse(datasets[graph_name]); return "----- start tests -----";});

    // Base graph data
    measure("graph type",                [&]{ return g.type; });
    measure("amount vertexes",           [&] { return g.amount_vertexes(); });
    measure("amount edges",              [&] { return g.amount_edges; });
    measure("min degree",                [&] { return analyzer.get_min_degree(); });
    measure("max degree",                [&] { return analyzer.get_max_degree(); });
    measure("average degree",            [&]{ return analyzer.get_average_degree(); });
    measure("density",                   [&]{ return analyzer.get_density(); });

    measure("amount of triangles",       [&] { return analyzer.get_amount_of_triangles(); });
    measure("amount of closed triplets", [&] { return analyzer.get_amount_of_closed_triplets(); });
    measure("amount of opened triplets", [&] { return analyzer.get_amount_of_opened_triplets(); });
    measure("global cluster coef",       [&]{ return analyzer.get_global_clustering_coefficient(); });
    // CC
    measure("amount of CC",              [&] { return analyzer.get_amount_of_CC(); });
    measure("average cluster coef",      [&]{ return analyzer.get_average_clustering_coefficient(); });
    measure("avr cluster coef in max CC",[&]{ return analyzer.get_average_clustering_coefficient_max_CC(); });
    measure("double sweep diameter",     [&] { return analyzer.estimate_diameter_of_max_CC_from_double_sweep(); });
    measure("sample diameter",           [&] { return analyzer.estimate_diameter_of_max_CC_from_sample(); });
    measure("snowball diameter",         [&] { return analyzer.estimate_diameter_of_max_CC_from_snowball(); });
    measure("sample 90 percentile",      [&] { return analyzer.estimate_90th_percentile_of_max_CC_from_sample(); });
    measure("snowball 90 percentile",    [&] { return analyzer.estimate_90th_percentile_of_max_CC_from_snowball(); });
    measure("fraction of ver in max CC", [&]{ return analyzer.get_fraction_of_vertexes_in_max_CC(); });
    // SCC
    if (datasets[graph_name].contains("/directed/")) {
        measure("amount of SCC",             [&] { return analyzer.get_amount_of_SCC(); });
        measure("fraction of ver in max SCC",[&]{ return analyzer.get_fraction_of_vertexes_in_max_SCC(); });
    }
    // Все функции вероятности пропущены потому что там числа надо вставлять, сам добавишь


    // Warring! Those functions break the graph when argument != 0
    measure("delete 0 percentage",       [&] { return analyzer.get_size_of_max_CC_after_delete_x_percentage_vertexes(0); });
    measure("delete 0.9 percentage",     [&] { return analyzer.get_size_of_max_CC_after_delete_x_percentage_vertexes(0.9); });
}

void json_example() { // Если что сломается - сорян
    const string graph_name = "email"; // Chose one
    string summarized_path = project_path + "/summarized/" + graph_name + ".json";
    string summarized_out_path = project_path + "/summarized/" + graph_name + "_out.json";
    json j = summarizer::json_open(summarized_path);
    j["test"] = 123;
    summarizer::json_write(j, summarized_out_path, true);
    cout << j.dump(4) << endl;
}

int main() {
    // Tests works only in DEBUG build
    analyzer_tests::tests();
    graph_tests::tests();

    // string graph_path = project_path + "/datasets/directed/soc-wiki-Vote.mtx";
    // string graph_path = project_path + "/datasets/directed/web-Stanford.txt";
    // string graph_path = project_path + "/datasets/undirected/musae_git_edges.csv";

    string graph_path = project_path + "/datasets/undirected/Email-EuAll.txt";
    string summarized_path = project_path + "/summarized/" + filesystem::path(graph_path).filename().string() + ".json";

    // summarizer::sum_up(graph_path, summarized_path);
    // json_example();

    parse_example();

    return 0;
}
