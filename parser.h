#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

using namespace std;

struct Candidate {
    string name;
    string education;
    double gpa = -1.0;          // -1 means not found
    vector<string> skills;
    int experienceYears = 0;
    vector<string> certifications;
    string filepath;

    // results filled by scoring stage
    double score = 0.0;
    char grade = 'D';
    vector<string> matchedSkills;
    vector<string> matchedCerts;
};

struct Job {
    vector<string> requiredSkills;
    vector<string> preferredSkills;
    double minGPA = 0.0;
    int minExperience = 0;
    vector<string> requiredCerts;
    vector<string> keywords;
};

vector<Candidate> parseResumesFromFolder(const string& folderPath);
Job parseJobDescription(const string& jobFilePath);

#endif // PARSER_H#pragma once
