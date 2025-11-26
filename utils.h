#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

using namespace std;

string toLower(string s);
string trim(const string& s);
vector<string> splitByMultiple(const string& s, const string& delims);
bool containsToken(const string& text, const string& token);
string normalizeSkill(const string& s);

#endif // UTILS_H#pragma once
