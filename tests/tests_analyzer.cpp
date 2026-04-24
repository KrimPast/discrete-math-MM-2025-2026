#include "tests_analyzer.h"
#include "../src/analyzer.h"
#include "../src/general.h"
#include "../src/graph.h"

namespace analyzer_tests {
    void tests() {
        test_unidirected_amount_vertexes();
        test_undirected_local_clustering_coefficient();
        test_directed_local_clustering_coefficient();
        test_amount_opened_triplets();
        test_amount_closed_triplets();
        test_vertex_degrees();
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
            g.calculate_amount_of_vertexes();
            assert(g.amount_vertexes == 2);
            assert(g.amount_edges == 1);

            g.insert(2, 3);
            g.calculate_amount_of_vertexes();
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

    void test_directed_local_clustering_coefficient() {
        graph g;
        g.type = Directed;
        g.insert(1, 2);
        g.insert(1, 3);
        g.insert(4, 1);

        assert(graph_analyzer(g).get_local_clustering_coefficient(1) == 0);
        g.insert(3, 2);
        assert(graph_analyzer(g).get_local_clustering_coefficient(1) == 1.0 / 6);
        g.insert(2, 3);
        assert(graph_analyzer(g).get_local_clustering_coefficient(1) == 2.0 / 6);
        g.insert(4, 3);
        assert(graph_analyzer(g).get_local_clustering_coefficient(1) == 3.0 / 6);
        g.insert(3, 4);
        assert(graph_analyzer(g).get_local_clustering_coefficient(1) == 4.0 / 6);
        g.insert(2, 4);
        assert(graph_analyzer(g).get_local_clustering_coefficient(1) == 5.0 / 6);
        g.insert(4, 2);
        assert(graph_analyzer(g).get_local_clustering_coefficient(1) == 6.0 / 6);
    }
    void test_amount_opened_triplets() {
        graph g;
        g.type = Undirected;
        vector<pair<int, int>> edges = {
            {1, 2}, {1, 3}, {2, 3},
            {2, 4}, {4, 5}, {5, 6},
            {3, 6}
        };
        for (auto p : edges)
            g.insert(p.first, p.second);
        vector<size_t> results;
        for (int i = 1; i <= 6; i++) {
            results.push_back(graph_analyzer(g).get_amount_of_opened_triplets(i));
        }
        vector<size_t> solution = {2, 2, 2, 3, 2, 3};
        assert(results == solution);
    }
    void test_amount_closed_triplets() {
        graph g;
        g.type = Undirected;
        g.insert(1, 2);
        g.insert(1, 5);
        g.insert(2, 5);

        g.insert(2, 3);
        g.insert(3, 4);
        g.insert(2, 4);

        g.insert(2, 6);
        g.insert(6, 7);
        vector<size_t> results;
        for (int i = 1; i <= 7; i++)
            results.push_back(graph_analyzer(g).get_amount_of_closed_triplets(i));

        vector<size_t> solution = {1, 2, 1, 1, 1, 0, 0};
        assert(results == solution);
    }

    void test_vertex_degrees() {
        graph g;
        g.type = Undirected;
        vector<pair<int, int>> edges = {
            {1, 2}, {2, 3}, {3, 4},
            {4, 5}, {5, 6}, {2, 6},
            {3, 5}, {5, 5}
        };
        for (auto edge : edges)
            g.insert(edge.first, edge.second);

        vector<size_t> results;
        graph_analyzer analyzer(g);
        for (int i = 1; i <= 6; i++)
            results.push_back(analyzer.get_degree(i));
        vector<size_t> solution = {1, 3, 3, 2, 5, 2};
        assert(solution == results);
        assert(analyzer.get_min_degree() == 1);
        assert(analyzer.get_max_degree() == 5);
        assert(analyzer.get_average_degree() == 16.0 / 6);

        assert(analyzer.get_probability_that_random_vertex_has_some_degree(1) == 1.0 / 6);
        assert(analyzer.get_probability_that_random_vertex_has_some_degree(2) == 2.0 / 6);
        assert(analyzer.get_probability_that_random_vertex_has_some_degree(3) == 2.0 / 6);
        assert(analyzer.get_probability_that_random_vertex_has_some_degree(4) == 0);
        assert(analyzer.get_probability_that_random_vertex_has_some_degree(5) == 1.0 / 6);
        assert(analyzer.get_probability_that_random_vertex_has_some_degree(6) == 0);

        assert(analyzer.get_probability_that_random_vertex_has_some_degree_log_log(0) == log2(1.0/6)); // [1, 2)
        assert(analyzer.get_probability_that_random_vertex_has_some_degree_log_log(1) == log2(4.0/6)); // [2, 4)
        assert(analyzer.get_probability_that_random_vertex_has_some_degree_log_log(2) == log2(1.0/6)); // [4, 8)
        assert(analyzer.get_probability_that_random_vertex_has_some_degree_log_log(3) == -INFINITY); // [8, 16)
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
        sort(output.begin(), output.end(), other::set_greater); // sort by set.size() and first element
        vector<set<int>> solution = {{1, 2, 3}, {5, 6}, {9, 10}, {4}, {7}, {8}};
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
            sort(output.begin(), output.end(), other::set_greater); // sort by set.size() and first element
            vector<set<int>> solution = {{1, 2, 3, 4}, {5, 6}, {7, 8}};
            assert(output == solution);
            assert(fraction == 0.5);
        }

    }

}