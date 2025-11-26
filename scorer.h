#ifndef SCORER_H
#define SCORER_H

#include "parser.h"
#include <vector>
#include <string>

using namespace std;

void scoreAllCandidates(vector<Candidate>& candidates, const Job& job);
double gradeFromScore(double score); // returns numeric bucket boundaries but we store grade char in candidate
char letterFromScore(double score);

#endif // SCORER_H#pragma once
