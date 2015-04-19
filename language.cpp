#include "language.h"
#include <regex>

class DefaultLanguage : public Language {
public:
    std::vector<int> process(const std::string &s) override;
};

class OtherLanguage : public DefaultLanguage {
protected:
    OtherLanguage() {}
public:
    std::string getName() override {
        return "Unknown";
    }
private:
    static OtherLanguage *object;
public:
    static OtherLanguage *getInstance() {
        if (object == nullptr)
            object = new OtherLanguage();
        return object;
    }
};

class CXXLanguage : public DefaultLanguage {
public:
    std::string getName() override {
        return "C++";
    }
    std::vector<int> process(const std::string &s) override;
private:
    static CXXLanguage *object;
public:
    static CXXLanguage *getInstance() {
        if (!object)
            object = new CXXLanguage();
        return object;
    }
};

OtherLanguage *OtherLanguage::object;
CXXLanguage *CXXLanguage::object;

Language *getLanguageByFilename(const std::string &s) {
    static std::regex cxx_regex(R"del(.*\.(cc|cxx|cpp|c|C))del");
    if (std::regex_match(s, cxx_regex))
        return CXXLanguage::getInstance();
    else
        return OtherLanguage::getInstance();
}

std::vector<int> DefaultLanguage::process(const std::string &s) {
    std::vector<int> result;
    for (char c : s) {
        if (!isspace(c))
            result.push_back(c);
    }
    return std::move(result);
}

static std::string removeComment(const std::string &s, const std::string &a, const std::string &b) {
    enum State {
        NORMAL, SINGLE, SING_BS, DOUBLE, DBL_BS, COMMENT, CS_BEGIN
    };
    int state = NORMAL;
    int CE_BEGIN = -static_cast<int>(b.size());
    std::string r;
    for (char c : s) {
        if (state == NORMAL) {
            if (c == '\'') {
                r.push_back(c);
                state = SINGLE;
            } else if (c == '"') {
                r.push_back(c);
                state = DOUBLE;
            } else if (c == a[0]) {
                if (a.size() == 1)
                    state = COMMENT;
                else
                    state = CS_BEGIN;
            } else {
                r.push_back(c);
            }
        } else if (state == SINGLE) {
            if (c == '\'')
                state = NORMAL;
            else if (c == '\\')
                state = SING_BS;
            r.push_back(c);
        } else if (state == SING_BS) {
            r.push_back(c);
            state = SINGLE;
        } else if (state == DOUBLE) {
            if (c == '"')
                state = NORMAL;
            else if (c == '\\')
                state = DBL_BS;
            r.push_back(c);
        } else if (state == DBL_BS) {
            r.push_back(c);
            state = DOUBLE;
        } else if (state == COMMENT) {
            if (c == b[0]) {
                if (b.size() == 1)
                    state = NORMAL;
                else
                    state = CE_BEGIN;
            }
        } else if (state > 0) {
            if (c == a[state - CS_BEGIN + 1]) {
                if (state - CS_BEGIN + 2 == static_cast<int>(a.size()))
                    state = COMMENT;
                else
                    ++state;
            } else {
                for (int i = 0; i <= state - CS_BEGIN; ++i)
                    r.push_back(a[i]);
                r.push_back(c);
                state = NORMAL;
            }
        } else {
            if (c == b[state - CE_BEGIN + 1]) {
                if (state - CE_BEGIN + 2 == static_cast<int>(b.size()))
                    state = NORMAL;
                else
                    ++state;
            } else {
                state = COMMENT;
            }                
        }
    }
    return r;
}

std::vector<int> CXXLanguage::process(const std::string &s) {
    return DefaultLanguage::process(removeComment(removeComment(removeComment(s, "#", "\n"), "/*", "*/"), "//", "\n"));
}
