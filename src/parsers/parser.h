#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <filesystem>

#include "../graph.h"
using namespace std;

class parser {
public:
    virtual ~parser() = default;
    virtual graph parse(const string &file_path);
    bool is_has_metadata = false;
protected:
    bool is_first = true;
private:
    virtual void process(graph& graph, string& line) {}
};

class txt_parser : public parser {
    void process(graph& graph, string& line) override;
};

class csv_parser : public parser {
    void process(graph& graph, string& line) override;
};

class mtx_parser : public parser {
    void process(graph& graph, string& line) override;
};

class uni_parser {
public:
    uni_parser() = delete;
    static graph parse(const string &file_path);
};
#endif // PARSER_H
