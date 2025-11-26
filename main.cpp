#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <iomanip>

#include "parser.h"
#include "scorer.h"
#include "report.h"

using namespace std;
namespace fs = std::filesystem;

int main()
{
    cout << "Resume Filter - Console version\n";
    cout << "Enter path to folder containing resume .txt files: ";
    string resumes_folder;
    getline(cin, resumes_folder);

    if (resumes_folder.empty()) {
        cout << "No folder provided. Exiting.\n";
        return 1;
    }

    cout << "Enter path to job description file (.txt): ";
    string job_file;
    getline(cin, job_file);

    if (job_file.empty()) {
        cout << "No job description file provided. Exiting.\n";
        return 1;
    }

    // 1) Read resumes
    vector<Candidate> candidates = parseResumesFromFolder(resumes_folder);
    if (candidates.empty()) {
        cout << "No resumes found in folder: " << resumes_folder << "\n";
        return 1;
    }

    // 2) Read job description
    Job job = parseJobDescription(job_file);

    // 3) Score candidates
    scoreAllCandidates(candidates, job);

    // 4) Print results to console
    cout << "\nResults:\n";
    cout << left << setw(30) << "Name"
        << setw(10) << "Grade"
        << setw(10) << "Score"
        << "Matched Skills\n";
    cout << string(80, '-') << "\n";

    for (auto& c : candidates) {
        cout << left << setw(30) << c.name
            << setw(10) << c.grade
            << setw(10) << fixed << setprecision(1) << c.score;

        // show up to first 4 matched skills
        string ms;
        for (size_t i = 0; i < c.matchedSkills.size() && i < 4; ++i) {
            if (i) ms += ", ";
            ms += c.matchedSkills[i];
        }
        cout << ms << "\n";
    }

    // 5) Generate report (selected candidates)
    string report_path = "selected_candidates_report.txt";
    bool ok = writeReport(report_path, candidates, job);
    if (ok) {
        cout << "\nReport written to: " << report_path << "\n";
    }
    else {
        cout << "\nFailed to write report.\n";
    }

    cout << "Done.\n";
    return 0;
}