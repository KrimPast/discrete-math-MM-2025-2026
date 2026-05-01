#ifndef OTHER_H
#define OTHER_H

#include <set>
#include <algorithm>
#include <memory>
#include <unordered_set>
#include <random>

using namespace std;

class other {
public:
    static vector<int> get_random_n_elements(const set<int>& initial, size_t n);
    static vector<int> get_random_n_elements(const unordered_set<int>& initial, size_t n);
    static void shuffle_vector(vector<int>& initial);
    static bool set_greater(const set<int> &a, const set<int> &b);
    static bool degree_greater(const pair<int, size_t> &a, const pair<int, size_t> &b);
    static int random_element(const set<int> &s);
    static int random_element(const unordered_set<int> &s);
private:
    static inline random_device rd;
    static inline auto gen = mt19937(rd());
};
#endif //OTHER_H
