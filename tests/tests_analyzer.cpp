#include "tests_analyzer.h"
#include "../src/analyzer.h"
#include "../src/general.h"
#include "../src/graph.h"

namespace analyzer_tests {
    void tests() {
        test_unidirected_amount_vertexes();
        test_undirected_local_clustering_coefficient();
        test_CSC_and_fraction_equality();
        test_CC_and_fraction_equality();
    }
    void test_unidirected_amount_vertexes() {
        auto types = { Directed, Undirected };
        for (auto type : types) {
            graph g;
            g.type = type;

            assert(g.amount_vertexes == 0);
            assert(g.amount_edges == 0);

            g.insert(1, 2);
            g.calculate_vertexes();
            assert(g.amount_vertexes == 2);
            assert(g.amount_edges == 1);

            g.insert(2, 3);
            g.calculate_vertexes();
            assert(g.amount_vertexes == 3);
            assert(g.amount_edges == 2);
        }
    }

    void test_undirected_local_clustering_coefficient() {
        graph g;
        g.type = Undirected;
        g.insert(1, 2);
        g.insert(1, 3);
        g.insert(1, 4);
        assert(graph_analyzer(g).get_local_clustering_coefficient(1) == 0);
        g.insert(2, 3);
        assert(graph_analyzer(g).get_local_clustering_coefficient(1) == 1.0 / 3);
        g.insert(2, 4);
        assert(graph_analyzer(g).get_local_clustering_coefficient(1) == 2.0 / 3);
        g.insert(3, 4);
        assert(graph_analyzer(g).get_local_clustering_coefficient(1) == 1);
    }

    void test_CSC_and_fraction_equality(){
        graph g;
        g.type = Directed;
        vector<pair<int, int>> edges = {
            {1, 2}, {2, 3}, {3, 1},
            {3, 4}, {5, 4}, {5, 6},
            {6, 5}, {7, 4}, {7, 8},
            {8, 9}, {9, 10}, {10, 9}
        };
        for (auto p : edges)
            g.insert(p.first, p.second);

        graph_analyzer analyzer(g);
        auto output = analyzer.get_SCCs();
        auto fraction = analyzer.get_fraction_of_vertexes_in_max_SCC();
        set<set<int>> solution = {{1, 2, 3}, {5, 6}, {9, 10}, {4}, {7}, {8}};
        assert(output == solution);
        assert(fraction == 0.3);
    }
    void test_CC_and_fraction_equality() {
        vector<pair<int, int>> edges = {
            {1, 2}, {2, 3},{3, 4},{1, 3},
            {2, 4}, {5, 6},{8, 7}
        };
        auto types = { Directed, Undirected };
        for (auto type : types) {
            graph g;
            g.type = type;
            for (auto p : edges)
                g.insert(p.first, p.second);

            graph_analyzer analyzer(g);
            auto output = analyzer.get_CCs();
            auto fraction = analyzer.get_fraction_of_vertexes_in_max_CC();
            set<set<int>> solution = {{1, 2, 3, 4}, {5, 6}, {7, 8}};
            assert(output == solution);
            assert(fraction == 0.5);
        }

    }

}