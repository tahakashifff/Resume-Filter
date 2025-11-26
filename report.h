#ifndef REPORT_H
#define REPORT_H

#include "parser.h"
#include <string>
#include <vector>

using namespace std;

bool writeReport(const string& outPath, const vector<Candidate>& candidates, const Job& job);

#endif // REPORT_H#pragma once
