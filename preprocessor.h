#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <stack>

using namespace std;

void reset();

string preProcess(string rawCode);

void instructionJudge();

void normalInstructionHandler();

bool functionHandler(string name);

void notFunctionHandler(string name);

void includeHandler();

void defineHandler();

void undefHandler();

void ifdefHandler();

void elseHandler();

void ifndefHander();

void ifHandler();

void endifHandler();

bool includeOtherFile(string filename);

vector<string> splitString(const string &str, const string &pattern);
