#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <filesystem>

#include "../graph.h"
using namespace std;

typedef boost::algorithm::detail::is_any_ofF<char> sep_type;

class parser {
public:
    explicit parser(graph &g, const sep_type& separator) : g(g), separator(separator) {}
    virtual ~parser() = default;

    graph parse(const string &file_path);
    bool is_has_metadata = false;
protected:
    graph& g;
    size_t line_index = 0;
    sep_type separator;
private:
    unordered_map<int, int> edgesOfVertex;

    virtual process_action whatWeDoWithIt(string& line) const = 0;
    virtual edge try_parse_metadata(string& line) const = 0;
    edge try_parse_edge(string& line) const;
};

class txt_parser : public parser {
public:
    explicit txt_parser(graph &g) : parser(g, sep_type(" \t")) {}
private:
    process_action whatWeDoWithIt(string& line) const override;
    edge try_parse_metadata(string& line) const override;
};

class csv_parser : public parser {
public:
    explicit csv_parser(graph &g) : parser(g, sep_type(",")) {}
private:
    process_action whatWeDoWithIt(string& line) const override;
    edge try_parse_metadata(string& line) const override;
};

class mtx_parser : public parser {
public:
    explicit mtx_parser(graph &g) : parser(g, sep_type(" ")) {}
private:
    process_action whatWeDoWithIt(string& line) const override;
    edge try_parse_metadata(string& line) const override;
};

class uni_parser {
public:
    uni_parser() = delete;
    static graph parse(const string &file_path);
};
#endif // PARSER_H
