#include <iostream>
#include <filesystem>

#include "src/analyzer.h"
#include "src/parsers/parser.h"
#include "src/summarizer/summarizer.h"
#include "tests/tests.h"

template <typename Func>
auto measure(const std::string& name, Func&& func) {
    const auto start = std::chrono::steady_clock::now();
    auto result = std::forward<Func>(func)();
    const auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << name << ": " << result << " [" << ms << " ms]\n";
    return result;
}

void parse_example() {
    const string project_path = filesystem::current_path().parent_path();
    graph g = uni_parser::parse(project_path + "/datasets/undirected/Email-EuAll.txt");
    // graph g = uni_parser::parse(project_path + "/datasets/undirected/ca-coauthors-dblp.txt");
    // graph g = uni_parser::parse(project_path + "/datasets/undirected/musae_git_edges.csv");
    // graph g = uni_parser::parse(project_path + "/datasets/directed/soc-wiki-Vote.mtx");
    // graph g = uni_parser::parse(project_path + "/datasets/directed/web-Stanford.txt");
    // graph g = uni_parser::parse(project_path + "/datasets/very_large_graphs/com-orkut.ungraph.txt");
    // graph g = uni_parser::parse(project_path + "/datasets/very_large_graphs/com-youtube.ungraph.txt");
    // graph g = uni_parser::parse(project_path + "/datasets/very_large_graphs/vk.csv");
    graph_analyzer analyzer(g);   // создаём один раз, чтобы не копировать граф
    cout << "----- start tests -----" << endl;
    cout << g.amount_edges << endl;
    measure("delete 0 percentage",    [&] { return analyzer.get_size_of_max_CC_after_delete_x_percentage_vertexes(0); });

    measure("delete 0 percentage",    [&] { return analyzer.get_size_of_max_CC_after_delete_x_percentage_vertexes(0.9); });

    measure("double sweep diameter",  [&] { return analyzer.estimate_diameter_of_max_CC_from_double_sweep(); });
    measure("sample diameter",        [&] { return analyzer.estimate_diameter_of_max_CC_from_sample(); });
    measure("snowball diameter",      [&] { return analyzer.estimate_diameter_of_max_CC_from_snowball(); });
    measure("sample 90 percentile",   [&] { return analyzer.estimate_90th_percentile_of_max_CC_from_sample(); });
    measure("snowball 90 percentile", [&] { return analyzer.estimate_90th_percentile_of_max_CC_from_snowball(); });
}
void json_example() {
     string project_path = filesystem::current_path().parent_path();
     string graph_path = project_path + "/datasets/undirected/Email-EuAll.txt";
     string summarized_path = project_path + "/summarized/" + filesystem::path(graph_path).filename().string() + ".json";
     string summarized_out_path = project_path + "/summarized/" + filesystem::path(graph_path).filename().string() + "_out.json";
     json j = summarizer::json_open(summarized_path);
     j["test"] = 123;
     summarizer::json_write(j, summarized_out_path, true);
     cout << j.dump(4) << endl;
}

int main() {
     // Tests works only in DEBUG build
     analyzer_tests::tests();
     graph_tests::tests();

     string project_path = filesystem::current_path().parent_path();

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