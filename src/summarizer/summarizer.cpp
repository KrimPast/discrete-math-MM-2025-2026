#include "../general.h"
#include "../analyzer.h"
#include "summarizer.h"

#include <fstream>
#include <iostream>

const string basic_information = "Basic Information";
const string additional_information = "Additional Information";
const string connected_components = "Connected Components";


namespace summarizer {
    void destructive_summarizes(json& j, graph& g);
    // void set_result(json& j, result res, const any& value) {
    //     j[result_to_string(res)] = value;
    // }
    void set_graph_name(json& j, const string& name) {
        j[basic_information]["Name"] = filesystem::path(name).filename();
    }
    void set_graph_type(json &j, g_type type) {
        j[basic_information]["Type"] = type == Directed ? "Directed" : "Undirected";
    }
    void set_amount_of_vertexes(json &j, size_t amount) {
        j[basic_information]["Amount of vertexes"] = amount;
    }
    void set_amount_of_edges(json &j, size_t amount) {
        j[basic_information]["Amount of edges"] = amount;
    }
    void set_density(json &j, double density) {
        j[basic_information]["Density"] = density;
    }
    void set_amount_of_CCs(json &j, size_t amount) {
        j[connected_components]["Amount of CCs"] = amount;
    }
    void set_amount_of_SCCs(json &j, size_t amount) {
        j[connected_components]["Amount of SCCs"] = amount;
    }
    void set_fraction_of_vertexes_in_max_CC(json &j, double fraction) {
        j[connected_components]["Fraction of vertexes in max CC"] = fraction;
    }
    void set_fraction_of_vertexes_in_max_SCC(json &j, double fraction) {
        j[connected_components]["Fraction of vertexes in max SCC"] = fraction;
    }
    void set_min_degree(json &j, size_t degree) {
        j[additional_information]["Minimum degree"] = degree;
    }
    void set_max_degree(json &j, size_t degree) {
        j[additional_information]["Maximum degree"] = degree;
    }
    void set_average_degree(json &j, double degree) {
        j[additional_information]["Average degree"] = degree;
    }
    void set_amount_of_triangles(json &j, size_t amount) {
        j[additional_information]["Amount of triangles"] = amount;
    }
    void set_global_clustering_coefficient(json &j, double coef) {
        j[additional_information]["Global Clustering coefficient"] = coef;
    }
    void set_average_clustering_coefficient(json &j, double coef) {
        j[additional_information]["Average Clustering coefficient"] = coef;
    }
    void sum_up(const string &graph_path, const string &log_path){
        ofstream out(log_path);
        if (!out.is_open()) throw runtime_error("sum_up: Cannot open output file!");

        graph g = uni_parser::parse(graph_path);
        auto analyzer = graph_analyzer(g);
        json j;

        set_graph_name(j, filesystem::path(graph_path).filename().string());
        set_graph_type(j, g.type);
        set_amount_of_vertexes(j, g.amount_vertexes);
        set_amount_of_edges(j, g.amount_edges);
        set_density(j, analyzer.get_density());

        set_amount_of_CCs(j, analyzer.get_amount_of_CC());
        set_fraction_of_vertexes_in_max_CC(j, analyzer.get_fraction_of_vertexes_in_max_CC());
        if (g.type == Directed) {
            set_amount_of_SCCs(j, analyzer.get_amount_of_SCC());
            set_fraction_of_vertexes_in_max_SCC(j, analyzer.get_fraction_of_vertexes_in_max_SCC());
        }

        if (g.type == Undirected) {
            set_min_degree(j, analyzer.get_min_degree());
            set_max_degree(j, analyzer.get_max_degree());
            set_average_degree(j, analyzer.get_average_degree());
        }

        set_amount_of_triangles(j, analyzer.get_amount_of_triangles()); // wastes a very long time
        set_global_clustering_coefficient(j, analyzer.get_global_clustering_coefficient());
        set_average_clustering_coefficient(j, analyzer.get_average_clustering_coefficient());

        destructive_summarizes(j, g);
        out << j.dump(4);
        out.close();
    }
    json json_open(string& file_path) {
        ifstream in(file_path);
        if (!in.is_open()) throw runtime_error("json_open: Cannot open a json file!");
        json j = json::parse(in);
        in.close();
        return j;
    }
    void json_write(json &j, string& file_path, bool force) {
        if (!force && filesystem::exists(file_path)) throw runtime_error("json_write: Writeable file already exists! If you sure what you do, use `force` flag");
        ofstream out(file_path);
        if (!out.is_open()) throw runtime_error("json_write: Cannot open a json file!");
        out << j.dump(4);
        out.close();
    }
    void destructive_summarizes(json& j, graph &g_copy) {
        graph g = g_copy;
        auto analyzer = graph_analyzer(g);

        int steps = 10;
        float p = 1.0f / (float) steps;
        j[additional_information]["Sizes of max CC after delete x% vertexes"]["0"]
            = analyzer.get_size_of_max_CC_after_delete_x_percentage_vertexes(0);
        for (int i = 0; i < steps; i++) {
            double curr_p = p / (1 - ((float)i / (float)steps));
            size_t size = analyzer.get_size_of_max_CC_after_delete_x_percentage_vertexes(curr_p);
            j[additional_information]["Sizes of max CC after delete x% vertexes"]
                [to_string((i + 1) * 100 / steps)] = size;
        }

        if (g.type == Undirected) {
            g = g_copy;
            analyzer.g = g;

            p = 1.0f / (float) steps;
            j[additional_information]["Sizes of max CC after delete x% vertexes of max degrees"]["0"]
                = analyzer.get_size_of_max_CC_after_delete_x_percentage_vertexes_of_max_degrees(0);
            for (int i = 0; i < steps; i++) {
                double curr_p = p / (1 - ((float)i / (float)steps));
                size_t size = analyzer.get_size_of_max_CC_after_delete_x_percentage_vertexes_of_max_degrees(curr_p);
                j[additional_information]["Sizes of max CC after delete x% vertexes of max degrees"]
                    [to_string((i + 1) * 100 / steps)] = size;
            }
        }
    }
    string result_to_string(const result res) {
        switch (res) {
            case graph_name: return "Name";
            case graph_type: return "Type";
            case amount_vertexes: return "Amount vertexes";
            case amount_edges: return "Amount edges";
            case density: return "Density";
            case amount_of_CC: return "Amount of CCs";
            case amount_of_SCC: return "Amount of SCCs";
            case fraction_of_vertexes_in_max_CC: return "Fraction of vertexes in max CC";
            case fraction_of_vertexes_in_max_SCC: return "Fraction of vertexes in max SCC";
            case amount_triangles: return "Amount of triangles";
            case global_clustering_coefficient: return "Global Clustering coefficient";
            case average_clustering_coefficient: return "Average Clustering coefficient";
            case min_degree: return "Minimum degree";
            case max_degree: return "Maximum degree";
            case average_degree: return "Average degree";
        }
        throw runtime_error("Undefined type of result!");
    }
}