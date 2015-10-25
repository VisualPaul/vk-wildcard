#include "likeness.h"
#include <valarray>
#include <cstdint>
#include <unordered_set>

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

static std::vector<uint64_t> hashVector(const std::vector<int> &a) {
    std::vector<uint64_t> res(a.size() + 1);
    for (std::size_t i = 0; i < a.size(); ++i)
        res[i + 1] = res[i] * 65539ULL + (uint64_t)a[i];
    return res;
}

static uint64_t power(int x) {
    static std::vector<uint64_t> p(1, 1ULL);
    while ((int)p.size() <= x)
        p.push_back(p.back() * 65539ULL);
    return p[x];
}

static uint64_t getHash(const std::vector<uint64_t> &h, int l, int r) {
    return h[r] - power(r - l) * h[l];
}

template <int len>
static double countEqualSubstrings(const std::vector<int> &a, const std::vector<int> &b) {
    if (a.size() < b.size())
        return countEqualSubstrings<len>(b, a);
    auto ha = hashVector(a);
    auto hb = hashVector(b);
    std::unordered_set<uint64_t> s;
    for (int i = 0; i + len <= (int)a.size(); ++i)
        s.insert(getHash(ha, i, i + len));
    int cnt = 0;
    int total = 0;
    for (int i = 0; i + len <= (int)b.size(); i += len) {
        ++total;
        if (s.find(getHash(hb, i, i + len)) != s.end())
            ++cnt;
    }
    if (total < 25)
        return 0.0;
    else
        return (double) cnt / (double) total;
}
    
double getLikeness(const std::string &a, Language *aLang,
                   const std::string &b, Language *bLang) {
    auto s = aLang->process(a);
    auto t = bLang->process(b);
    if (s.size() * 2 <= t.size() || t.size() * 2 <= s.size())
        return 0.0;
    int d = LevenshteinDistance(s, t);
    int minD = std::abs(static_cast<int>(s.size()) - static_cast<int>(t.size()));
    int maxD = static_cast<int>(std::max(s.size(), t.size()));
    d -= minD;
    maxD -= minD;
    double likeness1 = 1.0 - static_cast<double>(d) / static_cast<double>(maxD);
    double likeness2 = countEqualSubstrings<7>(s, t);
        
    return std::max(likeness1, likeness2 * 0.9);
}
