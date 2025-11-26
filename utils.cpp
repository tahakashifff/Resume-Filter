#include "utils.h"
#include <algorithm>
#include <cctype>
#include <sstream>

using namespace std;

string toLower(string s)
{
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return tolower(c); });
    return s;
}

string trim(const string& s)
{
    size_t start = 0;
    while (start < s.size() && isspace((unsigned char)s[start])) start++;
    if (start == s.size()) return "";
    size_t end = s.size() - 1;
    while (end > start && isspace((unsigned char)s[end])) end--;
    return s.substr(start, end - start + 1);
}

vector<string> splitByMultiple(const string& s, const string& delims)
{
    vector<string> res;
    string token;
    for (char c : s) {
        if (delims.find(c) != string::npos) {
            if (!token.empty()) {
                res.push_back(trim(token));
                token.clear();
            }
        }
        else {
            token.push_back(c);
        }
    }
    if (!token.empty()) res.push_back(trim(token));
    return res;
}

// simple token-contained checker (word boundary not enforced, simple use)
bool containsToken(const string& text, const string& token)
{
    if (token.empty()) return false;
    string lt = toLower(text);
    string lk = toLower(token);
    return lt.find(lk) != string::npos;
}

// normalize skill text: lowercase, trim, replace common synonyms if needed
string normalizeSkill(const string& s)
{
    string t = toLower(s);
    t = trim(t);
    // basic synonyms
    if (t == "cpp") return "c++";
    if (t == "js") return "javascript";
    return t;
}