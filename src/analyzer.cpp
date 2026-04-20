#include "analyzer.h"

double graph_analyzer::get_density() {
    double max_edges = (g.vertexes / 2) * (g.vertexes - 1);
    return g.edges / max_edges;
}