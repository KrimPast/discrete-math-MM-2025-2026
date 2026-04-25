#include <stdexcept>
#include <random>

#include "other.h"

namespace other {
    vector<int> get_random_n_elements_from_set(const set<int>& initial, size_t n) {
        if (n > initial.size())
            throw runtime_error("get_random_n_elements_from_set: Initial size lesser than size of required vector");

        vector<int> out;
        std::random_device rd;
        std::mt19937 gen(rd());
        ranges::sample(initial, std::back_inserter(out), n, gen);
        return out;
    }
    bool set_greater(const set<int> &a, const set<int> &b) {
        if (a.size() == b.size())
            return *a.begin() < *b.begin();
        return a.size() > b.size();
    }
    bool degree_greater(const pair<int, size_t> &a, const pair<int, size_t> &b) {
        return a.first > b.first;
    }
}
