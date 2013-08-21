#ifndef CSV_PRASER_H
#define CSV_PRASER_H


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

/**
* Parses CSV file line
* @author DM
* @param record     vector of csv elements from one line returned by the function
* @param line       contains the line read form CSV that will be parsed
* @param delimiter  delimiter used to separate the entries in the csv file
* @date 19.04.2013
*/
void csvline(vector<string> &record, const string& line, char delimiter);

#endif // CSV_PRASER_H
