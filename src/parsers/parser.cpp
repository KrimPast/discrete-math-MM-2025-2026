#include <filesystem>
#include <fstream>
#include <iostream>

#include "parser.h"

using namespace std;

graph parser::parse(const string &file_path) {
    line_index = 0;
    is_has_metadata = false;
    edge metadata = null_edge;

    ifstream in(file_path);
    if (in.is_open()) {
        string line;
        while (getline(in, line)) {
            auto action = whatWeDoWithIt(line);
            switch (action) {
                case TryParseEdge:
                {
                    edge new_edge = try_parse_edge(line);
                    if (new_edge != null_edge) // If one of many strokes in invalid, we only send warning
                        g.insert(new_edge.first, new_edge.second);
                    break;
                }
                case TryParseMetadata:
                {
                    if (metadata != null_edge) {
                        cerr << "(warning) parse: Double metadata in one file!" << endl;
                    }
                    metadata = try_parse_metadata(line);
                    break;
                }
                case DoNothing: break;
            }
            ++line_index;
        }
    }
    if (metadata != null_edge) {
        if (metadata.first != g.amount_vertexes())
            cerr << "(warning) parse: Real amount of vertexes is not equal expected (" << metadata.first << " != " <<
                g.amount_vertexes() << ")" << endl;
        if (metadata.second != g.amount_edges)
            cerr << "(warning) parse: Real amount of edges is not equal expected (" << metadata.second << " != " <<
                g.amount_edges << ")" << endl;
    }
    in.close();
    return g;
}

edge parser::try_parse_edge(string &line) const {
    vector<string> strs;
    boost::split(strs, line, separator);
    if (strs.size() >= 2) {
        return edge{stoi(strs[0]), stoi(strs[1])};
    }
    cerr << "(warning) try_parse_edge: Not enough arguments in #" << line_index + 1 << " line!" << endl;
    return null_edge;
}

// (Start) Section "What we do with it"
process_action txt_parser::whatWeDoWithIt(string &line) const {
    if ((line[0] == '#' && line_index == 2) || (line[0] != '#' && line_index == 0)) {
        return TryParseMetadata;
    }
    if (line[0] == '#') return DoNothing;
    return TryParseEdge;
}
process_action csv_parser::whatWeDoWithIt(string &line) const {
    if (line_index == 0)
        return DoNothing;
    return TryParseEdge;
}

process_action mtx_parser::whatWeDoWithIt(string &line) const {
    if (line_index == 0) return DoNothing;
    else if (line_index == 1) return TryParseMetadata;
    return TryParseEdge;
}
// (End) Section "What we do with it"

// (Start) Section "Try parse metadata"
edge txt_parser::try_parse_metadata(string &line) const {
    vector<string> strs;
    boost::split(strs, line, separator);
    if (line_index == 0) return edge{stoi(strs[0]), stoi(strs[1])};
    return edge{stoi(strs[2]), stoi(strs[4])};
}
edge csv_parser::try_parse_metadata(string &line) const {
     return null_edge;
}
edge mtx_parser::try_parse_metadata(string &line) const {
    vector<string> strs;
    boost::split(strs, line, separator);

    if (strs.size() == 3) {
        return edge{stoi(strs[0]), stoi(strs[2])};
    }
    return null_edge;
}
// (End) Section "Try parse metadata"

// Пройтись по файлу, считать, сколько у каждой вершины ребёр и проставить capacity
graph uni_parser::parse(const string &file_path) {
    filesystem::path f = file_path;
    if (!filesystem::exists(file_path)) {
        throw runtime_error("File is not exist");
    }
    string ext = f.extension();
    graph g;

    if (boost::contains(file_path, "undirected") || boost::contains(file_path, "very_large_graphs")) {
        g.type = g_type::Undirected;
    }
    else if (boost::contains(file_path, "directed")) {
        g.type = g_type::Directed;
    }
    else {
        throw runtime_error("Cannot identify type of graph. You must install it manually!\n");
    }
    auto a = mtx_parser(g);
    if (ext == ".mtx")
        mtx_parser(g).parse(file_path);
    else if (ext == ".txt")
        txt_parser(g).parse(file_path);
    else if (ext == ".csv")
        csv_parser(g).parse(file_path);
    else
        throw runtime_error("uni_parser: Undefined format!");

    return g;
}