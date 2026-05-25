#include "../general.h"
#include "../analyzer.h"
#include "summarizer.h"

#include <fstream>
#include <iostream>

const string basic_information = "Basic Information";
const string connected_components = "Connected Components";
const string additional_information = "Additional Information";

namespace summarizer {

    void destructive_summarizes(json& j, graph& g);

    nlohmann::basic_json<nlohmann::ordered_map>& get_json_placing(json &j, const measure_type type) {
        switch (type) {
            case graph_name: return j[basic_information]["Name"];
            case graph_type: return j[basic_information]["Type"];
            case amount_of_vertexes: return j[basic_information]["Amount of vertexes"];
            case amount_of_edges: return j[basic_information]["Amount of edges"];
            case density: return j[basic_information]["Density"];
            case amount_of_CCs: return j[connected_components]["Amount of CCs"];
            case amount_of_SCCs: return j[connected_components]["Amount of SCCs"];
            case fraction_of_vertexes_in_max_CC: return j[connected_components]["Fraction of vertexes in max CC"];
            case fraction_of_vertexes_in_max_SCC: return j[connected_components]["Fraction of vertexes in max SCC"];
            case min_degree: return j[additional_information]["Minimum degree"];
            case max_degree: return j[additional_information]["Maximum degree"];
            case average_degree: return j[additional_information]["Average degree"];
            case amount_of_triangles: return j[additional_information]["Amount of triangles"];
            case global_clustering_coefficient: return j[additional_information]["Global Clustering coefficient"];
            case amount_of_opened_triplets: return j[additional_information]["Amount of opened triplets"];
            case amount_of_closed_triplets: return j[additional_information]["Amount of closed triplets"];
            case average_clustering_coefficient: return j[additional_information]["Average Clustering coefficient"];
            case average_clustering_coefficient_in_max_CC: return j[additional_information]["Average Clustering coefficient in max CC"];
            case double_sweep_diameter: return j[additional_information]["Double sweep diameter"];
            case sample_diameter: return j[additional_information]["Sample diameter"];
            case snowball_diameter: return j[additional_information]["Snowball diameter"];
            case sample_90_percentile: return j[additional_information]["Sample 90 procentile"];
            case snowball_90_percentile: return j[additional_information]["Snowball 90 procentile"];
            case probability_that_random_vertex_has_degree_less_than_some_degree: return j[additional_information]["Probability that random vertex has degree <= than some degree"];
            case probability_that_random_vertex_has_degree_less_than_some_degree_in_log2_scale: return j[additional_information]["Probability in log2 scale that random vertex has log2_degree >= than some log2_degree"];
            case sizes_of_max_CC_after_delete_x_percent_random_vertexes: return j[additional_information]["Sizes of max CC after delete x% random vertexes"];
            case sizes_of_max_CC_after_delete_x_percent_max_degreed_vertexes: return j[additional_information]["Sizes of max CC after delete x% max degreed vertexes"];

            case undefined_field: return j["undefined"];
        }
        throw runtime_error("get_json_placing: Undefined measure type!");
    }

    json json_open(const string& file_path) {
        ifstream in(file_path);
        if (!in.is_open()) throw runtime_error("json_open: Cannot open a json file!");
        json j = json::parse(in);
        in.close();
        return j;
    }
    void json_write(json &j, const string& file_path, const bool force) {
        if (!force && filesystem::exists(file_path)) throw runtime_error("json_write: Writeable file already exists! If you sure what you do, use `force` flag");

        filesystem::path dirPath = filesystem::path(file_path).parent_path();
        if (!filesystem::exists(dirPath)) {
            filesystem::create_directory(dirPath);
        }
        ofstream out(file_path);
        if (!out.is_open())
            throw runtime_error("json_write: Cannot open a json file!");
        out << j.dump(4);
        out.close();
    }
}