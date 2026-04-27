#ifndef OTHER_H
#define OTHER_H

#include <set>
#include <algorithm>
#include <unordered_set>

using namespace std;

namespace other {
    vector<int> get_random_n_elements_from_set(const set<int>& initial, size_t n);
    bool set_greater(const set<int> &a, const set<int> &b);
    bool degree_greater(const pair<int, size_t> &a, const pair<int, size_t> &b);
    int random_element(const set<int> &s);
}
#endif //OTHER_H
