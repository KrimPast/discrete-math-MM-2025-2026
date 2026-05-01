#include <stdexcept>
#include <random>

#include "other.h"

vector<int> other::get_random_n_elements(const set<int>& initial, size_t n) {
    if (n > initial.size())
        throw runtime_error("get_random_n_elements_from_set: Initial size lesser than size of required vector");
    vector<int> out;
    vector initial_vector(initial.begin(), initial.end());
    ranges::sample(initial_vector, std::back_inserter(out), static_cast<long>(n), gen);
    return out;
}
vector<int> other::get_random_n_elements(const unordered_set<int>& initial, size_t n) {
    if (n > initial.size())
        throw runtime_error("get_random_n_elements_from_unordered_set: Initial size lesser than size of required vector");

    vector<int> out;
    vector initial_vector(initial.begin(), initial.end());
    ranges::sample(initial_vector, std::back_inserter(out), static_cast<long>(n), gen);
    return out;
}
void other::shuffle_vector(vector<int>& initial) {
    ranges::shuffle(initial, gen);
}
bool other::set_greater(const set<int> &a, const set<int> &b) {
    if (a.size() == b.size())
        return *a.begin() < *b.begin();
    return a.size() > b.size();
}
bool other::degree_greater(const pair<int, size_t> &a, const pair<int, size_t> &b) {
    return a.first > b.first;
}
int other::random_element(const set<int> &s) {
    if (s.empty()) throw runtime_error("random_element: empty set");
    return get_random_n_elements(s, 1)[0];
}
int other::random_element(const unordered_set<int> &s) {
    if (s.empty()) throw runtime_error("random_element: empty set");
    return get_random_n_elements(s, 1)[0];
}

