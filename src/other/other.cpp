#include "other.h"

namespace other {
    vector<int> get_random_n_elements_from_set(set<int> initial, size_t n) {
        vector<int> out;
        std::random_device rd;
        std::mt19937 gen(rd());
        ranges::sample(out, std::back_inserter(out), n, gen);
        return out;
    }
    bool set_greater(const set<int> &a, const set<int> &b) {
        if (a.size() == b.size())
            return *a.begin() < *b.begin();
        return a.size() > b.size();
    }
}
