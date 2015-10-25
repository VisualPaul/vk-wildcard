#include "language.h"
#include <regex>
#include <unordered_map>

class DefaultLanguage : public Language {
protected:
    virtual bool isIdentifier(char c) {
        return isalnum(c) || c == '_';
    }
public:
    DefaultLanguage() {}
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

#define DEFINE_LANGUAGE(class_name, language_name)              \
    class class_name : public DefaultLanguage {                 \
    public:                                                     \
    std::string getName() override {                            \
        return language_name;                                   \
    }                                                           \
    std::vector<int> process(const std::string &s) override;    \
    private:                                                    \
    static class_name *object;                                  \
    class_name() {}                                             \
    public:                                                     \
    static class_name *getInstance() {                          \
        if (!object)                                            \
            object = new class_name();                          \
        return object;                                          \
    }                                                           \
    };                                                          \
    class_name *class_name::object;

DEFINE_LANGUAGE(CXXLanguage,    "C++");
DEFINE_LANGUAGE(PythonLanguage, "Python");
DEFINE_LANGUAGE(JavaLanguage,   "Java");
DEFINE_LANGUAGE(CSharpLanguage, "C#");
DEFINE_LANGUAGE(PascalLanguage, "Pascal");
OtherLanguage *OtherLanguage::object;

Language *getLanguageByFilename(const std::string &s) {
    static std::regex cxx_regex(R"del(.*\.(cc|cxx|cpp|c|C))del");
    static std::regex java_regex(R"del(.*\.java)del");
    static std::regex python_regex(R"del(.*\.py)del");
    static std::regex csharp_regex(R"del(.*\.cs)del");
    static std::regex pascal_regex(R"del(.*\.pas)del");
    if (std::regex_match(s, cxx_regex))
        return CXXLanguage::getInstance();
     else if (std::regex_match(s, java_regex))
        return JavaLanguage::getInstance();
    else if (std::regex_match(s, python_regex))
        return PythonLanguage::getInstance();
    else if (std::regex_match(s, pascal_regex))
        return PascalLanguage::getInstance();
    else if (std::regex_match(s, csharp_regex))
        return CSharpLanguage::getInstance();
    else
        return OtherLanguage::getInstance();
}

static int getNumber(const std::string &s) {
    static std::unordered_map<std::string, int> m;
    static int freeId = 0;
    auto it = m.find(s);
    if (it == m.end()) {
        return m[s] = freeId++;
    } else {
        return it->second;
    }
}

std::string standardPreprocess(const std::string &ss) {
    using namespace std::regex_constants;
    static std::regex my_regex(R"del(^.*(template|read|write|next).*$)del", ECMAScript | icase);
    return std::regex_replace(ss, my_regex, "");
}

std::vector<int> DefaultLanguage::process(const std::string &ss) {
    std::string s = standardPreprocess(ss);
    enum State {
        NORMAL, IDENTIFIER
    };
    State state = NORMAL;
    std::vector<int> result;
    std::string identifier;
    auto finalize = [&] () {
        state = NORMAL;
        result.push_back(getNumber(identifier));
        identifier.clear();
    };
    for (char c : s) {
        if (state == NORMAL) {
            if (isspace(c)) {
                continue;
            } else if (isIdentifier(c)) {
                identifier.push_back(c);
                state = IDENTIFIER;
            } else {
                result.push_back(getNumber(std::string(1, c)));
            }
        } else if (state == IDENTIFIER) {
            if (isspace(c)) {
                finalize();
            }  else if (isIdentifier(c)) {
                identifier.push_back(c);
            } else {
                finalize();
                result.push_back(getNumber(std::string(1, c)));
            }
        }
    }
    /*
    std::unordered_map<int, int> lastMention;
    for (size_t i = 0; i < result.size(); ++i) {
        int oldResult = result[i];
        if (lastMention.find(result[i]) != lastMention.end()) {
            result[i] = lastMention[result[i]] - static_cast<int>(i);
        }
        lastMention[oldResult] = (int)i;
    }
    */
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
                if (b.size() == 1) {
                    if (b == "\n")
                        r.push_back(c);
                    state = NORMAL;
                } else
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

inline std::string removeCXXComments(const std::string &s) {
    return removeComment(removeComment(s, "/*", "*/"), "//", "\n");
}

std::vector<int> CXXLanguage::process(const std::string &s) {
    return DefaultLanguage::process(removeComment(removeCXXComments(s), "#", "\n"));
}


std::vector<int> JavaLanguage::process(const std::string &s) {
    return DefaultLanguage::process(removeComment(removeCXXComments(s), "import", ";"));
}

std::vector<int> PascalLanguage::process(const std::string &s) {
    std::string t = removeComment(removeCXXComments(s), "import", ";");
    for (char &c : t)
        c = (char)tolower(c);
    return DefaultLanguage::process(t);
}

std::vector<int> PythonLanguage::process(const std::string &s) {
    return DefaultLanguage::process(removeComment(s, "#", "\n"));
}

std::vector<int> CSharpLanguage::process(const std::string &s) {
    return DefaultLanguage::process(removeCXXComments(s));
}
