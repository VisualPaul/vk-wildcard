#include "likeness.h"
#include <valarray>

static int LevenshteinDistance(const std::vector<int> &a,
                               const std::vector<int> &b) {
    if (a.size() < b.size())
        return LevenshteinDistance(b, a);
    if (a.empty())
        return static_cast<int>(b.size());
    if (b.empty())
        return static_cast<int>(a.size());
    std::vector<int> v0(b.size() + 1);
    std::vector<int> v1(b.size() + 1);
    std::iota(v0.begin(), v0.end() - 1, 0);
    for (size_t i = 0; i < a.size(); ++i) {
        v1[0] = static_cast<int>(i + 1);
        for (size_t j = 0; j < b.size(); ++j) {
            v1[j + 1] = std::min(v1[j] + 1, std::min(v0[j + 1] + 1, v0[j] + (a[i] != b[j])));
        }
        v0.swap(v1);
    }
    return v0[b.size()];
}
    
double getLikeness(const std::string &a, Language *aLang,
                   const std::string &b, Language *bLang) {
    auto s = aLang->process(a);
    auto t = bLang->process(b);
    int d = LevenshteinDistance(s, t);
    int minD = std::abs(static_cast<int>(s.size()) - static_cast<int>(t.size()));
    int maxD = static_cast<int>(std::max(s.size(), t.size()));
    d -= minD;
    maxD -= minD;
    return 1.0 - static_cast<double>(d) / static_cast<double>(maxD);
}
