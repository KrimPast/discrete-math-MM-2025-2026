#ifndef SUMMARIZER_H
#define SUMMARIZER_H

#include "../parsers/parser.h"
#include "../../plugins/json.hpp"

using json = nlohmann::ordered_json;

namespace summarizer {
    enum result {
        // Task A1
        graph_name,
        graph_type,
        amount_vertexes,
        amount_edges,
        density,

        amount_of_CC,
        amount_of_SCC,

        fraction_of_vertexes_in_max_CC,
        fraction_of_vertexes_in_max_SCC,

        // Task A3
        amount_triangles,
        global_clustering_coefficient,
        average_clustering_coefficient,

        min_degree,
        max_degree,
        average_degree,

        // Make graphic?
        // double get_probability_that_random_vertex_has_some_degree(size_t degree);
        // double get_probability_that_random_vertex_has_some_degree_log_log(size_t log2_degree);

        // Many measures?
        // size_t get_size_of_max_CC_after_delete_x_percentage_vertexes(double x);
        // size_t get_size_of_max_CC_after_delete_x_percentage_vertexes_of_max_degrees(double x);
    };

    void sum_up(const string &graph_path, const string &log_path);
    string result_to_string(result res);
    json json_open(string& file_path);
    void json_write(json &j, string& file_path, bool force = false);
}

#endif //SUMMARIZER_H
