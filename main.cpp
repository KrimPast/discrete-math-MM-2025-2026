#include <iostream>
#include <filesystem>

#include "src/analyzer.h"
#include "src/parsers/parser.h"
#include "src/summarizer/summarizer.h"
#include "tests/tests.h"

void parse_example() {
     graph g;
     string project_path = filesystem::current_path().parent_path();
     g = uni_parser::parse(project_path + "/datasets/undirected/Email-EuAll.txt");
     // g = uni_parser::parse(project_path + "/datasets/undirected/ca-coauthors-dblp.txt");
     // g = uni_parser::parse(project_path + "/datasets/undirected/musae_git_edges.csv");
     // g = uni_parser::parse(project_path + "/datasets/directed/soc-wiki-Vote.mtx");
     // g = uni_parser::parse(project_path + "/datasets/directed/web-Stanford.txt");
     // g = uni_parser::parse(project_path + "/datasets/very_large_graphs/com-orkut.ungraph.txt");
     // g = uni_parser::parse(project_path + "/datasets/very_large_graphs/com-youtube.ungraph.txt");
     // g = uni_parser::parse(project_path + "/datasets/very_large_graphs/vk.csv");
     // cout << g.amount_edges << endl;
     // cout << graph_analyzer(g).get_size_of_max_CC_after_delete_x_percentage_vertexes(0) << endl;
     // cout << graph_analyzer(g).get_size_of_max_CC_after_delete_x_percentage_vertexes(0.9) << endl;
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

     summarizer::sum_up(graph_path, summarized_path);
     json_example();

     // parse_example();


     return 0;
}