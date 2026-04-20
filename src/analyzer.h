#ifndef ANALYZERS_H
#define ANALYZERS_H

#include <vector>

#include "general.h"
#include "graph.h"

using namespace std;

class graph_analyzer{
public:
    graph& g;
    graph_analyzer(graph& graph) : g(graph) {}

    double get_density();

    size_t get_amount_of_connected_components();
    double get_fraction_of_vertexes_in_max_connected_component();

    size_t get_amount_of_strongly_connected_components();
    double get_fraction_of_vertexes_in_max_strongly_connected_component();
private:
    vector<vector<int>> get_connected_components();
    vector<vector<int>> get_strongly_connected_components();
};

#endif // ANALYZERS_H
