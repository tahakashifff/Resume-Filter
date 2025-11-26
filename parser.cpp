#include "parser.h"
#include "utils.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

// Helper to lowercase & trim a copy of a string
static string lowerTrim(const string& s) {
    return toLower(trim(s));
}

// split a "key: value" line into key and value (both trimmed)
static pair<string, string> splitKeyValue(const string& line) {
    size_t pos = line.find(':');
    if (pos == string::npos) return { trim(line), "" };
    string key = trim(line.substr(0, pos));
    string val = trim(line.substr(pos + 1));
    return { key, val };
}

// Try to parse a number (integer or decimal) from a string. Returns true if found.
static bool parseFirstNumber(const string& s, double& out) {
    // simple regex to find first floating number like 3.7 or 9.0/10 or 3
    try {
        std::smatch m;
        std::regex fracR("([0-9]+\\.?[0-9]*)\\s*\\/\\s*([0-9]+\\.?[0-9]*)");
        if (std::regex_search(s, m, fracR)) {
            double a = stod(m[1].str());
            double b = stod(m[2].str());
            if (b > 0.0) {
                // normalize to 4.0 if denominator > 4
                if (b > 4.0) out = (a / b) * 4.0;
                else out = a;
                return true;
            }
        }
        // simple decimal/integer
        std::regex numR("([0-9]+\\.?[0-9]*)");
        if (std::regex_search(s, m, numR)) {
            out = stod(m[1].str());
            return true;
        }
    }
    catch (...) {
        return false;
    }
    return false;
}

// parse a single resume file into Candidate
static Candidate parseSingleResume(const string& path)
{
    Candidate c;
    c.filepath = path;

    ifstream in(path);
    if (!in.is_open()) {
        cout << "Warning: cannot open " << path << "\n";
        return c;
    }

    vector<string> lines;
    string line;
    string fullText;
    while (getline(in, line)) {
        lines.push_back(line);
        fullText += line + "\n";
    }
    in.close();

    // 1) Name: find a line starting "Name:" or fallback to first non-empty line
    bool nameFound = false;
    for (auto& ln : lines) {
        auto kv = splitKeyValue(ln);
        string keyLower = lowerTrim(kv.first);
        if (keyLower == "name" && !kv.second.empty()) {
            c.name = kv.second;
            nameFound = true;
            break;
        }
    }
    if (!nameFound) {
        // fallback: first non-empty line
        for (auto& ln : lines) {
            string t = trim(ln);
            if (!t.empty()) {
                c.name = t;
                break;
            }
        }
    }

    // 2) GPA: look for lines that mention "gpa", "cgpa", or "grade" and parse numbers
    bool gpaFound = false;
    for (auto& ln : lines) {
        string lower = lowerTrim(ln);
        if (lower.find("gpa") != string::npos || lower.find("cgpa") != string::npos || lower.find("grade") != string::npos) {
            double val = 0.0;
            if (parseFirstNumber(ln, val)) {
                c.gpa = val;
                gpaFound = true;
                break;
            }
        }
    }
    // if not found, try to find fraction anywhere e.g. "9.0/10" in full text
    if (!gpaFound) {
        double val = 0.0;
        if (parseFirstNumber(fullText, val)) {
            // only accept as GPA if value looks reasonable (>= 2.0 or <= 10.0)
            if (val > 0.0 && val <= 10.0) c.gpa = val;
        }
    }

    // 3) Skills: look for "skills" or "technical skills" lines, parse after colon
    bool skillsFound = false;
    for (auto& ln : lines) {
        string lower = lowerTrim(ln);
        if (lower.rfind("skills", 0) == 0 || lower.rfind("technical skills", 0) == 0 || lower.find("technical skills") != string::npos) {
            auto kv = splitKeyValue(ln);
            string val = kv.second;
            if (val.empty()) {
                // maybe skills span the rest of file - ignore complexity for now
                continue;
            }
            vector<string> list = splitByMultiple(val, ",;|");
            for (auto& s : list) {
                string sk = normalizeSkill(s);
                if (!sk.empty()) c.skills.push_back(sk);
            }
            skillsFound = true;
            break;
        }
    }
    // fallback: scan full text for known skill tokens
    if (!skillsFound) {
        string lowerFull = toLower(fullText);
        vector<string> known = { "c++","cpp","java","python","sql","javascript","html","css","git","linux","algorithms","data structures","machine learning","ml" };
        for (auto& k : known) {
            if (containsToken(lowerFull, k)) {
                string nk = normalizeSkill(k);
                bool exists = false;
                for (auto& ex : c.skills) if (toLower(ex) == nk) { exists = true; break; }
                if (!exists) c.skills.push_back(nk);
            }
        }
    }

    // 4) Experience: look for "years" and sum numbers found on lines mentioning "year" or "experience"
    int totalYears = 0;
    for (auto& ln : lines) {
        string lower = lowerTrim(ln);
        if (lower.find("year") != string::npos || lower.find("experience") != string::npos) {
            std::smatch m;
            std::regex numR("([0-9]+)");
            auto begin = std::sregex_iterator(ln.begin(), ln.end(), numR);
            auto end = std::sregex_iterator();
            for (auto it = begin; it != end; ++it) {
                try {
                    totalYears += stoi((*it)[1].str());
                }
                catch (...) {}
            }
        }
    }
    if (totalYears > 0) c.experienceYears = totalYears;

    // 5) Certifications: look for lines starting with "cert" and parse list
    for (auto& ln : lines) {
        string lower = lowerTrim(ln);
        if (lower.rfind("certifications", 0) == 0 || lower.rfind("certificates", 0) == 0 || lower.find("certificate") != string::npos) {
            auto kv = splitKeyValue(ln);
            string val = kv.second;
            if (!val.empty()) {
                vector<string> list = splitByMultiple(val, ",;|");
                for (auto& s : list) {
                    string cc = trim(s);
                    if (!cc.empty()) c.certifications.push_back(cc);
                }
            }
        }
    }
    // fallback: look for known cert tokens inside full text
    {
        string lowerFull = toLower(fullText);
        vector<string> knownCerts = { "aws certified solutions architect", "aws", "azure", "ccna", "oracle", "pmp", "ocjp" };
        for (auto& k : knownCerts) {
            if (containsToken(lowerFull, k)) {
                bool exists = false;
                for (auto& ex : c.certifications) {
                    if (toLower(ex).find(toLower(k)) != string::npos) { exists = true; break; }
                }
                if (!exists) c.certifications.push_back(k);
            }
        }
    }

    return c;
}

vector<Candidate> parseResumesFromFolder(const string& folderPath)
{
    vector<Candidate> result;
    try {
        for (auto& ent : fs::directory_iterator(folderPath)) {
            if (!ent.is_regular_file()) continue;
            string path = ent.path().string();
            if (ent.path().extension() == ".txt") {
                Candidate c = parseSingleResume(path);
                result.push_back(c);
            }
        }
    }
    catch (const std::exception& e) {
        cout << "Error reading folder: " << e.what() << "\n";
    }
    return result;
}

Job parseJobDescription(const string& jobFilePath)
{
    Job job;
    ifstream in(jobFilePath);
    if (!in.is_open()) {
        cout << "Cannot open job description file: " << jobFilePath << "\n";
        return job;
    }
    string line;
    vector<string> lines;
    string content;
    while (getline(in, line)) {
        lines.push_back(line);
        content += line + "\n";
    }
    in.close();

    // parse line-by-line for specific keys
    for (auto& ln : lines) {
        auto kv = splitKeyValue(ln);
        string key = lowerTrim(kv.first);
        string val = kv.second;
        if (key.find("required skills") != string::npos || key == "required skills" || key == "required skill") {
            vector<string> list = splitByMultiple(val, ",;|");
            for (auto& s : list) if (!trim(s).empty()) job.requiredSkills.push_back(normalizeSkill(s));
        }
        else if (key.find("preferred skills") != string::npos || key == "preferred skills" || key == "preferred skill") {
            vector<string> list = splitByMultiple(val, ",;|");
            for (auto& s : list) if (!trim(s).empty()) job.preferredSkills.push_back(normalizeSkill(s));
        }
        else if (key.find("min gpa") != string::npos || key == "min gpa" || key == "min gpa:") {
            double v = 0.0;
            if (parseFirstNumber(val, v)) job.minGPA = v;
        }
        else if (key.find("min experience") != string::npos || key == "min experience") {
            double v = 0.0;
            if (parseFirstNumber(val, v)) job.minExperience = (int)v;
        }
        else if (key.find("required certifications") != string::npos || key.find("required certification") != string::npos) {
            vector<string> list = splitByMultiple(val, ",;|");
            for (auto& s : list) if (!trim(s).empty()) job.requiredCerts.push_back(trim(s));
        }
    }

    // fallback keywords: extract words of length >= 4 from top of content
    {
        string prefix = content.substr(0, min((size_t)1000, content.size()));
        vector<string> tokens = splitByMultiple(prefix, " ,.;:\n");
        for (auto& t : tokens) {
            string tt = trim(t);
            if (tt.size() >= 4) job.keywords.push_back(toLower(tt));
        }
    }

    return job;
}