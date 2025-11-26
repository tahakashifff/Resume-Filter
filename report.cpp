#include "report.h"
#include <fstream>
#include <iomanip>

using namespace std;

// static threshold: select B or above
static bool isSelected(const Candidate& c) {
    return (c.grade == 'A' || c.grade == 'B');
}

bool writeReport(const string& outPath, const vector<Candidate>& candidates, const Job& job)
{
    ofstream out(outPath);
    if (!out.is_open()) return false;

    out << "Selected Candidates Report\n";
    out << "==========================\n\n";
    out << "Criteria used (static):\n";
    out << "- Grades considered: A, B\n";
    out << "- Weights: Skills 50, Experience 20, GPA 15, Certs 10, Keywords 5\n\n";

    int total = 0, selected = 0;
    for (auto& c : candidates) {
        total++;
        if (isSelected(c)) {
            selected++;
            out << "Name: " << c.name << "\n";
            out << "Score: " << fixed << setprecision(1) << c.score << "   Grade: " << c.grade << "\n";
            out << "Experience (years): " << c.experienceYears << "\n";
            out << "GPA: " << ((c.gpa > 0.0) ? to_string(c.gpa) : string("N/A")) << "\n";

            out << "Matched Skills: ";
            if (c.matchedSkills.empty()) out << "None";
            else {
                for (size_t i = 0; i < c.matchedSkills.size(); ++i) {
                    if (i) out << ", ";
                    out << c.matchedSkills[i];
                }
            }
            out << "\n";

            out << "Matched Certifications: ";
            if (c.matchedCerts.empty()) out << "None";
            else {
                for (size_t i = 0; i < c.matchedCerts.size(); ++i) {
                    if (i) out << ", ";
                    out << c.matchedCerts[i];
                }
            }
            out << "\n";

            out << "Source file: " << c.filepath << "\n";
            out << "--------------------------\n";
        }
    }

    out << "\nSummary:\n";
    out << "Total candidates processed: " << total << "\n";
    out << "Selected (A or B): " << selected << "\n";

    out.close();
    return true;
}