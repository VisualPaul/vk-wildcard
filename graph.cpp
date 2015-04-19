#include "graph.h"

using std::vector;
using std::size_t;

namespace {
void dfsComponent(const vector<vector<size_t>> &g, vector<unsigned char> &used, size_t v, vector<size_t> &comp) {
    used[v] = true;
    comp.push_back(v);
    for (size_t u : g[v]) {
        if (!used[u])
            dfsComponent(g, used, u, comp);
    }
}
}

std::vector<std::vector<std::size_t>> findComponents(const std::vector<std::vector<std::size_t>> &g) {
    size_t n = g.size();
    vector<unsigned char> used(n, false);
    vector<vector<size_t>> res;
    for (size_t i = 0; i < n; ++i) {
        if (!used[i]) {
            res.emplace_back();
            dfsComponent(g, used, i, res.back());
        }
    }
    return res;
}
