#include "tests.h"

#include <cassert>
#include "../src/analyzer.h"

namespace graph_tests {
    void tests() {
        test_directed_add_edge();
        test_undirected_add_edge();

        test_directed_remove_edge();
        test_undirected_remove_edge();

        test_directed_remove_vertex();
        test_undirected_remove_vertex();

        test_directed_get_vertexes();
        test_undirected_get_vertexes();

        test_calculate_amount_of_vertexes();
    }

    void test_directed_add_edge() {
        graph g;
        g.type = Directed;
        g.insert(1, 2);
        g.insert(2, 3);
        g.insert(2, 4);
        assert(g[1] == vector({2}));
        assert(g[2] == vector({3, 4}));
        assert(g[3].empty());
        assert(g[4].empty());
    }

    void test_undirected_add_edge() {
        graph g;
        g.type = Undirected;
        g.insert(1, 2);
        g.insert(2, 3);
        g.insert(2, 4);
        g.insert(3, 4);
        assert(g[1] == vector({2}));
        assert(g[2] == vector({1, 3, 4}));
        assert(g[3] == vector({2, 4}));
        assert(g[4] == vector({2, 3}));
    }

    void test_directed_remove_edge() {
        graph g;
        g.type = Directed;
        g.insert(1, 2);
        g.insert(2, 1);
        g.remove(1, 2);
        assert(g[1].empty());
        assert(g[2] == vector({1}));
    }

    void test_undirected_remove_edge() {
        graph g;
        g.type = Undirected;
        g.insert(1, 2);
        g.remove(2, 1);
        assert(g[1].empty());
        assert(g[2].empty());
    }

    void test_directed_remove_vertex() {
        graph g;
        g.type = Directed;
        g.insert(1, 10);
        g.insert(10, 2);
        g.insert(2, 3);

        g.insert(1, 2);
        g.calculate_amount_of_vertexes();

        g.remove_vertex(10);

        assert(g[1] == vector({2}));
        assert(g[2] == vector({3}));
        assert(g[3].empty());
        assert(g[10].empty());

        assert(g.amount_vertexes == 3);
        assert(g.amount_edges == 2);
    }

    void test_undirected_remove_vertex() {
        graph g;
        g.type = Undirected;
        g.insert(1, 10);
        g.insert(10, 2);
        g.insert(2, 3);

        g.insert(1, 2);
        g.calculate_amount_of_vertexes();
        g.remove_vertex(10);

        assert(g[1] == vector({2}));
        assert(g[2] == vector({3, 1}));
        assert(g[3] == vector({2}));
        assert(g[10].empty());

        assert(g.amount_vertexes == 3);
        assert(g.amount_edges == 2);
    }

    void test_directed_get_vertexes() {
        graph g;
        g.type = Directed;
        g.insert(1, 2);
        g.insert(2, 3);
        g.insert(3, 6);
        g.insert(9, 5);
        g.insert(5, 3);
        assert(g.get_vertexes() == set({1, 2, 3, 5, 6, 9}));
    }

    void test_undirected_get_vertexes() {
        graph g;
        g.type = Undirected;
        g.insert(1, 2);
        g.insert(4, 6);
        g.insert(8, 10);
        g.insert(6, 1);

        g.remove(6, 4);
        assert(g.get_vertexes() == set({1, 2, 6, 8, 10}));
    }

    void test_calculate_amount_of_vertexes() {
        graph g;
        g.type = Directed;
        g.insert(1, 2);
        g.insert(2, 6);
        g.insert(3, 6);
        g.insert(8, 6);
        g.insert(9, 1);
        g.calculate_amount_of_vertexes();
        assert(g.amount_vertexes == 6);
    }
}























