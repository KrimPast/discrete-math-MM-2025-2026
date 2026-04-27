#ifndef SUMMARIZER_H
#define SUMMARIZER_H

#include "../parsers/parser.h"
#include "../../plugins/json.hpp"

using json = nlohmann::ordered_json;

namespace summarizer {
    void sum_up(const string &graph_path, const string &log_path);
    json json_open(string& file_path);
    void json_write(json &j, string& file_path, bool force = false);
}

#endif //SUMMARIZER_H
