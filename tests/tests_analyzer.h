#ifndef TESTS_ANALYZER_H
#define TESTS_ANALYZER_H

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

#endif // TESTS_ANALYZER_H
