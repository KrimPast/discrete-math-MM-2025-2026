#ifndef TESTS_H
#define TESTS_H

namespace analyzer_tests {
    void tests();
    void test_unidirected_amount_vertexes();
    void test_undirected_local_clustering_coefficient();
    void test_directed_local_clustering_coefficient();
    void test_amount_opened_triplets();
    void test_amount_closed_triplets();
    void test_vertex_degrees();
    void test_CSC_and_fraction_equality();
    void test_CC_and_fraction_equality();
}

namespace graph_tests {
    void tests();

    void test_directed_add_edge();
    void test_undirected_add_edge();

    void test_directed_remove_edge();
    void test_undirected_remove_edge();

    void test_directed_remove_vertex();
    void test_undirected_remove_vertex();

    void test_directed_get_vertexes();
    void test_undirected_get_vertexes();

    void test_calculate_amount_of_vertexes();

};

#endif // TESTS_H
