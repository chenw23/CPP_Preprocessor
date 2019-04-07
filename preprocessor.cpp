#include <cstring>
#include "preprocessor.h"

string processedCode;
map<string, string> macros;
string macroName;
string macroValue;
string line;
bool isShouldRead = true;
stack<bool> shouldReadStack;

string preProcess(string rawCode) {
    vector<string> rawCodes = splitString(rawCode, "\n");
    int length = rawCodes.size();
    for (int i = 0; i < length; i++) {
        line = rawCodes[i];
        instructionJudge();
    }
    return processedCode;
}

void instructionJudge() {
    if (line.find("//") != string::npos) return;
    if (line.find("#") == 0) {
        string tmpString = line.substr(line.find(" ", 2) + 1);
        if (line.find("else") != string::npos) {
            elseHandler();
            return;
        } else if (line.find("endif") != string::npos) {
            endifHandler();
            return;
        } else if (line.find("ifdef") != string::npos) {
            macroName = tmpString;
            ifdefHandler();
            return;
        } else if (line.find("ifndef") != string::npos) {
            macroName = tmpString;
            ifndefHander();
            return;
        } else if (line.find("undef") != string::npos) {
            macroName = tmpString;
            undefHandler();
            return;
        }
        if (isShouldRead) {
            if (line.find("include") != string::npos) {
                macroName = tmpString;
                includeHandler();
                return;
            } else if (line.find("define") != string::npos) {
                int index = tmpString.find(" ");
                macroName = tmpString.substr(0, index);
                macroValue = tmpString.substr(index + 1);
                defineHandler();
                return;
            } else if (line.find("if") != string::npos) {
                macroName = tmpString;
                ifHandler();
                return;
            }
        }
    } else if (isShouldRead) normalInstructionHandler();
}

void normalInstructionHandler() {
    map<string, string>::iterator iterator;
    iterator = macros.begin();
    while (iterator != macros.end()) {
        macroName = iterator->first;
        macroValue = iterator->second;
        string name, functionName;
        if (macroName.find("(") != string::npos)
            functionName = macroName.substr(0, macroName.find("(") + 1);
        else name = macroName;
        functionHandler(functionName);
        notFunctionHandler(name);
        iterator++;
    }
    processedCode.append(line).push_back('\n');
}

bool functionHandler(string name) {
    if (name.compare("") == 0 || line.find(name) == string::npos || macroValue.compare("") == 0)
        return false;
    int indexOfLeftParenthesis = macroName.find("(");
    int indexOfRightParenthesis = macroName.find(")");
    string argOrigin =
            macroName.substr(indexOfLeftParenthesis + 1,
                             indexOfRightParenthesis - indexOfLeftParenthesis - 1);
    string functionName = macroName.substr(0, indexOfLeftParenthesis);
    int index;
    string argInput;
    if ((index = line.find(functionName)) != string::npos) {
        indexOfLeftParenthesis = line.find("(", index);
        indexOfRightParenthesis = line.find(")", index);
        argInput = line.substr(indexOfLeftParenthesis + 1,
                               indexOfRightParenthesis - indexOfLeftParenthesis - 1);
        int tempIndex;
        string tmpValue;
        if ((tempIndex = macroValue.find("##")) != string::npos) {
            tmpValue = macroValue;
            tmpValue.replace(0, tempIndex + 3, argInput);
        } else if ((tempIndex = macroValue.find_last_of("\"#")) != string::npos) {
            tmpValue = macroValue;
            argOrigin = "\"#" + argOrigin;
            tmpValue.replace(tempIndex, argOrigin.length(), "\"" + argInput + "\"");
        } else {
            tempIndex = macroValue.find(argOrigin);
            tmpValue = macroValue;
            tmpValue.replace(tempIndex, argOrigin.length(), argInput);
        }
        line.replace(index, indexOfRightParenthesis - index + 1, tmpValue);
    }
    return true;
}

void notFunctionHandler(string name) {
    if (name.compare("") == 0 || line.find(name) == string::npos || macroValue.compare("") == 0) {
        return;
    }
    int index;
    if ((index = line.find(name)) != string::npos) {
        line.replace(index, name.length(), macroValue);
    }
}

void includeHandler() {
    string filename = macroName.substr(1, macroName.length() - 2);
    if (macroName.find("<") == 0) {
        processedCode.append("#include ").append(macroName).push_back('\n');
    } else {
        if (!includeOtherFile(filename)) {
            processedCode.append("#include ").append(macroName).push_back('\n');
        }
    }
}

void defineHandler() {
    macros.erase(macroName);
    while (macros.count(macroValue)) {
        macroValue = macros[macroValue];
    }
    macros.insert(map<string, string>::value_type(macroName, macroValue));
}

void undefHandler() {
    macros.erase(macroName);
}

void ifdefHandler() {
    shouldReadStack.push(isShouldRead);
    isShouldRead = (macros.count(macroName) != 0);
}

void elseHandler() {
    isShouldRead = !isShouldRead;
}

void ifndefHander() {
    shouldReadStack.push(isShouldRead);
    isShouldRead = (macros.count(macroName) == 0);
}

void ifHandler() {
    shouldReadStack.push(isShouldRead);
    while (macros.count(macroName) != 0) {
        macroName = macros[macroName];
    }
    isShouldRead = (!macroName.compare("1"));
}

void endifHandler() {
    if (!shouldReadStack.empty()) {
        isShouldRead = shouldReadStack.top();
        shouldReadStack.pop();
    }
}

bool includeOtherFile(string filename) {
    if (!filename.compare("iostream")) return false;
    filename = "test/" + filename;
    string file;
    ifstream is(filename);
    if (!is.is_open()) {
        cout << "Broken input " + filename;
        return false;
    } else {
        string line;
        while (getline(is, line)) file.append(line).push_back('\n');
        is.close();
    }
    stack<bool> temStack = shouldReadStack;
    bool tmpBool = isShouldRead;
    while (!shouldReadStack.empty())
        shouldReadStack.pop();
    preProcess(file);
    shouldReadStack = temStack;
    isShouldRead = tmpBool;
    return true;
}

vector<string> splitString(const string &str, const string &pattern) {
    char *strTmp = new char[strlen(str.c_str()) + 1];
    strcpy(strTmp, str.c_str());
    vector<string> resultVec;
    char *tmpStr = strtok(strTmp, pattern.c_str());
    while (tmpStr != NULL) {
        resultVec.push_back(string(tmpStr));
        tmpStr = strtok(NULL, pattern.c_str());
    }
    delete[] strTmp;
    return resultVec;
}

void reset() {
    processedCode = "";
    macros.clear();
    isShouldRead = true;
    while (!shouldReadStack.empty()) shouldReadStack.pop();
}
