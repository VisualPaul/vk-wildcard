// -*-c++-*-
#ifndef LANGUAGE_H
#define LANGUAGE_H
#include <string>
#include <vector>

class Language {
public:
    virtual std::string getName() = 0;
    virtual std::vector<int> process(const std::string &s) = 0;
};

Language *getLanguageByFilename(const std::string &filename);
#endif
