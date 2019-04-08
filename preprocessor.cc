#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <stack>
#include <cstring>

using namespace std;

class Preprocessor {
public:
    Preprocessor() = default;

    string pre_process(const string &raw_code) {
        vector<string> raw_codes = Preprocessor::split_string(raw_code, "\n");
        int length = raw_codes.size();
        for (int i = 0; i < length; i++) {
            line = raw_codes[i];
            process_instruction();
        }
        return processed_code;
    }

    virtual ~Preprocessor() {
        processed_code = "";
        macros.clear();
        should_read = true;
        while (!should_read_stack.empty()) should_read_stack.pop();
    };

private:
    string processed_code;
    map<string, string> macros;
    string macro_name;
    string macro_value;
    string line;
    bool should_read = true;
    stack<bool> should_read_stack;

    void process_instruction() {
        if (line.find("//") != string::npos) return;
        if (line.find('#') == 0) {
            string macro_content = line.substr(line.find(' ', 2) + 1);
            if (line.find("else") != string::npos) {
                elseHandler();
                return;
            } else if (line.find("endif") != string::npos) {
                endifHandler();
                return;
            } else if (line.find("ifdef") != string::npos) {
                macro_name = macro_content;
                ifdefHandler();
                return;
            } else if (line.find("ifndef") != string::npos) {
                macro_name = macro_content;
                ifndefHander();
                return;
            } else if (line.find("undef") != string::npos) {
                macro_name = macro_content;
                undefHandler();
                return;
            }
            if (should_read) {
                if (line.find("include") != string::npos) {
                    macro_name = macro_content;
                    includeHandler();
                    return;
                } else if (line.find("define") != string::npos) {
                    int index = macro_content.find(' ');
                    macro_name = macro_content.substr(0, index);
                    macro_value = macro_content.substr(index + 1);
                    defineHandler();
                    return;
                } else if (line.find("if") != string::npos) {
                    macro_name = macro_content;
                    ifHandler();
                    return;
                }
            }
        } else if (should_read) process_normal_code();
    }

    void process_normal_code() {
        map<string, string>::iterator iterator;
        iterator = macros.begin();
        while (iterator != macros.end()) {
            macro_name = iterator->first;
            macro_value = iterator->second;
            string name, functionName;
            if (macro_name.find('(') != string::npos)
                functionName = macro_name.substr(0, macro_name.find('(') + 1);
            else name = macro_name;
            functionHandler(functionName);
            notFunctionHandler(name);
            iterator++;
        }
        processed_code.append(line).push_back('\n');
    }

    bool functionHandler(string name) {
        if (name.compare("") == 0 || line.find(name) == string::npos || macro_value.compare("") == 0)
            return false;
        int indexOfLeftParenthesis = macro_name.find('(');
        int indexOfRightParenthesis = macro_name.find(')');
        string argOrigin =
                macro_name.substr(indexOfLeftParenthesis + 1,
                                  indexOfRightParenthesis - indexOfLeftParenthesis - 1);
        string functionName = macro_name.substr(0, indexOfLeftParenthesis);
        int index;
        string argInput;
        if ((index = line.find(functionName)) != string::npos) {
            indexOfLeftParenthesis = line.find('(', index);
            indexOfRightParenthesis = line.find(')', index);
            argInput = line.substr(indexOfLeftParenthesis + 1,
                                   indexOfRightParenthesis - indexOfLeftParenthesis - 1);
            int tempIndex;
            string tmpValue;
            if ((tempIndex = macro_value.find("##")) != string::npos) {
                tmpValue = macro_value;
                tmpValue.replace(0, tempIndex + 3, argInput);
            } else if ((tempIndex = macro_value.find_last_of("\"#")) != string::npos) {
                tmpValue = macro_value;
                argOrigin = "\"#" + argOrigin;
                tmpValue.replace(tempIndex, argOrigin.length(), "\"" + argInput + "\"");
            } else {
                tempIndex = macro_value.find(argOrigin);
                tmpValue = macro_value;
                tmpValue.replace(tempIndex, argOrigin.length(), argInput);
            }
            line.replace(index, indexOfRightParenthesis - index + 1, tmpValue);
        }
        return true;
    }

    void notFunctionHandler(string name) {
        if (name.compare("") == 0 || line.find(name) == string::npos || macro_value.compare("") == 0) {
            return;
        }
        int index;
        if ((index = line.find(name)) != string::npos) {
            line.replace(index, name.length(), macro_value);
        }
    }

    void includeHandler() {
        string filename = macro_name.substr(1, macro_name.length() - 2);
        if (macro_name.find('<') == 0 || !includeOtherFile(filename))
            processed_code.append("#include ").append(macro_name).push_back('\n');
    }

    void defineHandler() {
        macros.erase(macro_name);
        while (macros.count(macro_value)) macro_value = macros[macro_value];
        macros.insert(map<string, string>::value_type(macro_name, macro_value));
    }

    void undefHandler() {
        macros.erase(macro_name);
    }

    void ifdefHandler() {
        should_read_stack.push(should_read);
        should_read = (macros.count(macro_name) != 0);
    }

    void elseHandler() {
        should_read = !should_read;
    }

    void ifndefHander() {
        should_read_stack.push(should_read);
        should_read = (macros.count(macro_name) == 0);
    }

    void ifHandler() {
        should_read_stack.push(should_read);
        while (macros.count(macro_name) != 0) macro_name = macros[macro_name];
        should_read = (!macro_name.compare("1"));
    }

    void endifHandler() {
        if (!should_read_stack.empty()) {
            should_read = should_read_stack.top();
            should_read_stack.pop();
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
        stack<bool> temStack = should_read_stack;
        bool tmpBool = should_read;
        while (!should_read_stack.empty())
            should_read_stack.pop();
        pre_process(file);
        should_read_stack = temStack;
        should_read = tmpBool;
        return true;
    }

    static vector<string> split_string(const string &str, const string &pattern) {
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
};
