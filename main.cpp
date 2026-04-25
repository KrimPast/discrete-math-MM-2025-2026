#include <iostream>
#include <filesystem>

#include "src/analyzer.h"
#include "src/parsers/parser.h"
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
     cout << g.amount_edges << endl;
     cout << graph_analyzer(g).get_size_of_max_CC_after_delete_x_percentage_vertexes(0) << endl;
     cout << graph_analyzer(g).get_size_of_max_CC_after_delete_x_percentage_vertexes(0.9) << endl;
}
int main() {
     // Tests works only in DEBUG build
     analyzer_tests::tests();
     graph_tests::tests();
     parse_example();
     return 0;
}