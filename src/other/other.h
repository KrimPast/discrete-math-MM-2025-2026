#ifndef OTHER_H
#define OTHER_H

#include <vector>
#include <set>
#include <random>
#include <algorithm>
using namespace std;

namespace other {
    vector<int> get_random_n_elements_from_set(set<int> initial, size_t n);
    bool set_greater(const set<int> &a, const set<int> &b);
}
#endif //OTHER_H
