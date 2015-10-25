#include <string>
#include <ctime>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "language.h"
#include "graph.h"
#include "likeness.h"

#ifndef MAIN_CONSTANT
#define MAIN_CONSTANT 0.74
#endif

#ifdef NO_TIME
#define HAVE_TIME clock() < 29 * CLOCKS_PER_SEC
#else
#define HAVE_TIME true
#endif

namespace {
std::string readFile(const std::string &filename) {
    std::string result;
    std::ifstream in(filename.c_str());
    int c;
    while ((c = in.get()) != EOF)
        result.push_back(static_cast<char>(c));
    result.shrink_to_fit();
    return result;
}

std::vector<std::string> getFileNames() {
    std::ifstream in("input.txt");
    int n;
    in >> n;
    std::vector<std::string> result(n);
    for (int i = 0; i < n; ++i)
        in >> result[i];
    return result;
}
}

int debugLevel;

int main(int argc, char **argv) {
    if (argc != 1) {
        debugLevel = atoi(argv[1]);
    } else {
        debugLevel = 0;
    }
    std::ios_base::sync_with_stdio(false);
    std::vector<std::string> files = getFileNames();
    std::vector<std::string> codes(files.size());
    for (std::size_t i = 0; i < files.size(); ++i)
        codes[i] = readFile(files[i]);
    std::size_t n = files.size();
    if (debugLevel != 4) {
        std::vector<std::vector<std::size_t>> graph(n);
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = i + 1; j < n && HAVE_TIME; ++j) {
                double likeness = getLikeness(codes[i], getLanguageByFilename(files[i]),
                                              codes[j], getLanguageByFilename(files[j]));
                if (debugLevel >= 2)
                    std::cout << files[i] << ' ' << files[j] << ' ' << likeness << '\n';
                if (likeness > MAIN_CONSTANT) {
                    if (debugLevel == 1)
                        std::cout << files[i] << ' ' << files[j] << ' ' << likeness << '\n';
                    graph[i].push_back(j);
                    graph[j].push_back(i);
                }
            }
        }
        auto comps = findComponents(graph);
        comps.erase(std::remove_if(comps.begin(), comps.end(), [] (const std::vector<std::size_t> &g) {
                    return g.size() == 1;
                }), comps.end());
        std::ofstream out("output.txt");
        out << comps.size() << '\n';
        for (std::size_t i = 0; i < comps.size(); ++i) {
            for (std::size_t j = 0; j < comps[i].size(); ++j) {
                out << files[comps[i][j]] << " \n"[j + 1 == comps[i].size()];
            }
        }
    } else {
        for (size_t i = 0; i < files.size(); ++i) {
            std::ofstream out(files[i] + ".out");
            for (int x : getLanguageByFilename(files[i])->process(codes[i])) {
                out << x << ' ';
            }
        }
    }
}
