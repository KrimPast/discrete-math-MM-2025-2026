#ifndef SUMMARIZER_H
#define SUMMARIZER_H

#include "../parsers/parser.h"
#include "../../plugins/json.hpp"

using json = nlohmann::ordered_json;

namespace summarizer {
    enum measure_type {
        undefined_field,
        graph_name, graph_type, amount_of_vertexes, amount_of_edges,
        density, amount_of_CCs, amount_of_SCCs,
        fraction_of_vertexes_in_max_CC, fraction_of_vertexes_in_max_SCC,
        min_degree, max_degree, average_degree,
        amount_of_triangles, global_clustering_coefficient,
        amount_of_opened_triplets, amount_of_closed_triplets,
        average_clustering_coefficient, average_clustering_coefficient_in_max_CC,
        double_sweep_diameter, sample_diameter, snowball_diameter,
        sample_90_percentile, snowball_90_percentile,
        probability_that_random_vertex_has_degree_less_than_some_degree,
        probability_that_random_vertex_has_degree_less_than_some_degree_in_log2_scale,
        sizes_of_max_CC_after_delete_x_percent_random_vertexes,
        sizes_of_max_CC_after_delete_x_percent_max_degreed_vertexes
    };

    void sum_up(const string &graph_path, const string &log_path);
    json json_open(const string& file_path);
    void json_write(json &j, const string& file_path, bool force = false);

    nlohmann::basic_json<nlohmann::ordered_map>& get_json_placing(json &j, measure_type type);
}

#endif //SUMMARIZER_H
