#include <fstream>
#include <iostream>
#include <stdexcept>

#include "parser.h"
#include "../general.h"

#include <filesystem>
using namespace std;

graph parser::parse(const string &file_path) {
    is_first = true;
    is_has_metadata = false;

    auto g = graph();
    ifstream in(file_path);
    if (in.is_open()) {
        string line;
        while (getline(in, line)) {
            process(g, line);
        }
    }
    in.close();
    return g;
}

void txt_parser::process(graph& graph, string& line){
    vector<string> strs;
    boost::split(strs, line, separator);
    if (line[0] == '#') {
        is_first = false;
        if (boost::contains(line, "Nodes")) {
            graph.amount_vertexes = stoi(strs[2]);
            graph.amount_edges = stoi(strs[4]);
        }
    }
    else if (strs.size() == 2) {
        int first = stoi(strs[0]), second = stoi(strs[1]);
        if (is_first) {
            graph.amount_vertexes = first;
            graph.amount_edges = second;
            is_first = false;
        }
        else {
            graph.insert(stoi(strs[0]), stoi(strs[1]));
        }
    }
    else {
        throw runtime_error("txt_parser: Caused error!");
    }
}
void csv_parser::process(graph& graph, string& line) {
    if (is_first) {
        is_first = false;
        return;
    }
    vector<string> strs;
    boost::split(strs, line, separator);
    if (strs.size() == 2) {
        graph.insert(stoi(strs[0]), stoi(strs[1]));
    }
    else {
        throw runtime_error("csv_parser: Caused error!");
    }
}
void mtx_parser::process(graph& graph, string& line) {
    if (line[0] == '%') return;
    vector<string> strs;
    boost::split(strs, line, separator);

    if (is_first && strs.size() == 3) {
        graph.amount_vertexes = stoi(strs[0]);
        graph.amount_edges = stoi(strs[2]);
        is_first = false;
        return;
    }

    if (strs.size() == 2) {
        graph.insert(stoi(strs[0]), stoi(strs[1]));
    }
    else {
        throw runtime_error("csv_parser: Caused error!");
    }
}

graph uni_parser::parse(const string &file_path) {
    filesystem::path f = file_path;
    if (!filesystem::exists(file_path)) {
        throw runtime_error("File is not exist");
    }
    string ext = f.extension();
    graph g;
    if (ext == ".mtx")
        g = mtx_parser().parse(file_path);
    else if (ext == ".txt")
        g = txt_parser().parse(file_path);
    else if (ext == ".csv")
        g = csv_parser().parse(file_path);
    else
        throw runtime_error("uni_parser: Undefined format!");

    if (boost::contains(file_path, "undirected")) {
        g.type = g_type::Undirected;
    }
    else if (boost::contains(file_path, "directed")) {
        g.type = g_type::Directed;
    }
    else {
        throw runtime_error("Cannot identify type of graph. You must install it manually!\n");
    }

    if (g.amount_vertexes == 0) g.calculate_amount_of_vertexes();

    return g;
}