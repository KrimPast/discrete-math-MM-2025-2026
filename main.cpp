#include <iostream>
#include <filesystem>

#include "src/analyzer.h"
#include "src/parsers/parser.h"

int main() {
    graph g;
    string project_path = filesystem::current_path().parent_path();
    g = uni_parser::parse(project_path + "/data/undirected/Email-EuAll.txt");
    // g = uni_parser::parse(project_path + "/data/undirected/ca-coauthors-dblp.txt");
    // g = uni_parser::parse(project_path + "/data/undirected/musae_git_edges.csv");
    // g = uni_parser::parse(project_path + "/data/directed/soc-wiki-Vote.mtx");
    graph_analyzer analyzer(g);
    printf("%.8lf\n", analyzer.get_density());
    return 0;
}
