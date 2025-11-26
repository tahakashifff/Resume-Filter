#include "scorer.h"
#include "utils.h"
#include <algorithm>
#include <cmath>

using namespace std;

/*
 Scoring strategy (static weights):
 - Skills: 50 points (required skills weighted higher)
 - Experience: 20 points
 - GPA: 15 points
 - Certifications: 10 points
 - Keywords/other: 5 points

 Mapping:
 85-100 -> A
 70-84  -> B
 55-69  -> C
 <55    -> D

 Note: thresholds and weights are static as requested.
*/

static const double SKILLS_WEIGHT = 50.0;
static const double EXPERIENCE_WEIGHT = 20.0;
static const double GPA_WEIGHT = 15.0;
static const double CERT_WEIGHT = 10.0;
static const double KEYWORD_WEIGHT = 5.0;

static const double MAX_EXPERIENCE_CONSIDERED = 10.0; // cap years to avoid huge advantage

// helper to compute intersection size (normalized)
static vector<string> intersectNormalized(const vector<string>& a, const vector<string>& b)
{
    vector<string> res;
    for (auto& x : a) {
        string nx = normalizeSkill(x);
        for (auto& y : b) {
            string ny = normalizeSkill(y);
            if (nx == ny) {
                res.push_back(nx);
                break;
            }
        }
    }
    return res;
}

void scoreAllCandidates(vector<Candidate>& candidates, const Job& job)
{
    for (auto& c : candidates) {
        double score = 0.0;

        // Skills scoring
        int reqCount = (int)job.requiredSkills.size();
        int prefCount = (int)job.preferredSkills.size();
        double skillsScore = 0.0;

        if (reqCount + prefCount == 0) {
            // no skill info in job -> small default
            skillsScore = SKILLS_WEIGHT * 0.2;
        }
        else {
            // required matches
            vector<string> reqMatched = intersectNormalized(c.skills, job.requiredSkills);
            vector<string> prefMatched = intersectNormalized(c.skills, job.preferredSkills);

            // required share gets bigger portion of SKILLS_WEIGHT
            double reqPart = SKILLS_WEIGHT * 0.7;
            double prefPart = SKILLS_WEIGHT * 0.3;

            if (reqCount > 0) {
                skillsScore += reqPart * ((double)reqMatched.size() / (double)reqCount);
            }
            if (prefCount > 0) {
                skillsScore += prefPart * ((double)prefMatched.size() / (double)prefCount);
            }

            // record matched skills names (unique)
            c.matchedSkills = reqMatched;
            for (auto& ps : prefMatched) {
                bool found = false;
                for (auto& x : c.matchedSkills) if (x == ps) { found = true; break; }
                if (!found) c.matchedSkills.push_back(ps);
            }
        }
        score += skillsScore;

        // Experience scoring
        double expScore = 0.0;
        double years = (double)c.experienceYears;
        if (years > 0) {
            double cap = min(years, MAX_EXPERIENCE_CONSIDERED);
            // if job.minExperience present, give full points when >= minExperience
            if (job.minExperience > 0) {
                if (years >= job.minExperience) {
                    expScore = EXPERIENCE_WEIGHT;
                }
                else {
                    // proportion of required
                    expScore = EXPERIENCE_WEIGHT * (years / (double)job.minExperience);
                }
            }
            else {
                // otherwise scale to cap
                expScore = EXPERIENCE_WEIGHT * (cap / MAX_EXPERIENCE_CONSIDERED);
            }
        }
        score += expScore;

        // GPA scoring
        double gpaScore = 0.0;
        if (c.gpa > 0.0) {
            if (job.minGPA > 0.0) {
                if (c.gpa >= job.minGPA) gpaScore = GPA_WEIGHT;
                else {
                    // partial proportional; assume 0..job.minGPA maps to 0..GPA_WEIGHT
                    double ratio = c.gpa / job.minGPA;
                    ratio = max(0.0, min(1.0, ratio));
                    gpaScore = GPA_WEIGHT * ratio;
                }
            }
            else {
                // no job min -> give partial based on 4.0 scale
                double ratio = min(c.gpa / 4.0, 1.0);
                gpaScore = GPA_WEIGHT * ratio;
            }
        }
        score += gpaScore;

        // Certifications scoring
        double certScore = 0.0;
        if (!job.requiredCerts.empty()) {
            for (auto& rc : job.requiredCerts) {
                for (auto& pc : c.certifications) {
                    if (containsToken(toLower(pc), toLower(rc))) {
                        certScore += CERT_WEIGHT / (double)job.requiredCerts.size();
                        c.matchedCerts.push_back(pc);
                    }
                }
            }
        }
        else {
            // give some credit for any certification
            if (!c.certifications.empty()) certScore = min(CERT_WEIGHT, (double)c.certifications.size() * 2.0);
        }
        score += certScore;

        // Keywords/other
        double keywordScore = 0.0;
        int matchedKW = 0;
        for (auto& kw : job.keywords) {
            if (containsToken(toLower(c.filepath), kw)) matchedKW++;
            // also could search other fields, but keep simple:
            if (containsToken(toLower(c.name), kw)) matchedKW++;
        }
        if (!job.keywords.empty()) {
            // small bonus proportional to matched keywords
            keywordScore = KEYWORD_WEIGHT * ((double)matchedKW / (double)max(1, (int)job.keywords.size()));
            if (keywordScore > KEYWORD_WEIGHT) keywordScore = KEYWORD_WEIGHT;
        }
        score += keywordScore;

        // final clamp
        if (score > 100.0) score = 100.0;
        if (score < 0.0) score = 0.0;

        c.score = score;
        c.grade = letterFromScore(score);
    }
}

double gradeFromScore(double score)
{
    return score; // placeholder: scores are numeric; grading done by letterFromScore
}

char letterFromScore(double score)
{
    if (score >= 85.0) return 'A';
    if (score >= 70.0) return 'B';
    if (score >= 55.0) return 'C';
    return 'D';
}