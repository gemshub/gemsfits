#ifndef CSV_PRASER_H
#define CSV_PRASER_H


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void csvline(vector<string> &record, const string& line, char delimiter);

#endif // CSV_PRASER_H
