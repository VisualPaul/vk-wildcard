#include <bits/stdc++.h>
using namespace std;

typedef map<string, vector<string>> graph;

map<string, vector<string>> createGraph(const string &filename) {
    map<string, vector<string>> res;
    ifstream in(filename);
    int groups;
    in >> groups;
    while (in.get() != '\n') {}
    for (int i = 0; i < groups; ++i) {
        string group;
        getline(in, group);
        istringstream gin(group);
        vector<string> v;
        string s;
        while (gin >> s) {
            v.push_back(s);
        }
        for (size_t i = 0; i < v.size(); ++i) {
            for (size_t j = 0; j < v.size(); ++j) {
                if (i != j)
                    res[v[i]].push_back(v[j]);
            }
        }
    }
    return res;
}

map<string, int> getComponentMark(graph &g) {
    map<string, int> result;
    int lastComponent = 1;
    function<void(const string &)> dfs = [&] (const string &s) {
        result[s] = lastComponent;
        for (auto t : g[s]) {
            if (result[t] == 0)
                dfs(t);
        }
    };
    for (auto sp : g) {
        auto s = sp.first;
        if (result[s] == 0) {
            dfs(s);
            ++lastComponent;
        }
    }
    return result;
}

int countEdges(graph &g) {
    int res = 0;
    for (const auto &s : g) {
        res += static_cast<int>(g[s.first].size());
    }
    return res / 2;          
}

int getScore(graph &myGraph, graph &juryGraph) {
    auto mk = getComponentMark(juryGraph);
    int lastFakeMark = -1;
    for (auto sp : myGraph) {
        auto s = sp.first;
        if (mk[s] == 0)
            mk[s] = lastFakeMark--;
    }
    for (auto sp : myGraph) {
        auto s = sp.first;
        for (const string &t : myGraph[s]) {
            if (mk[s] != mk[t])
                return 0;
        }
    }
    return countEdges(myGraph) * 100 / countEdges(juryGraph);
}

int main() {
    int totalScore = 0;
    for (int i = 1; i <= 10; ++i) {
        char buf[3];
        sprintf(buf, "%02d", i);
        string number(buf);
        auto juryGraph = createGraph("tests/" + number + ".a");
        auto myGraph = createGraph("tests/" + number + "/output.txt");
        int score = getScore(myGraph, juryGraph);
        totalScore += score;
        cout << "result on " << number << " test is " << score << "\n";
    }
    cout << "total: " << totalScore << endl;
}
