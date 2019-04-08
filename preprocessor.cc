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
            if (macro_name.find('(') != string::npos)
                process_function(macro_name.substr(0, macro_name.find('(') + 1));
            else
                notFunctionHandler(macro_name);
            iterator++;
        }
        processed_code.append(line).push_back('\n');
    }

    bool process_function(const string &name) {
        if (name.empty() || line.find(name) == string::npos || macro_value.empty())
            return false;
        int left_parenthesis_index = macro_name.find('(');
        int right_parenthesis_index = macro_name.find(')');
        string arg =
                macro_name.substr(
                        left_parenthesis_index + 1,
                        right_parenthesis_index - left_parenthesis_index - 1);
        string function_name = macro_name.substr(0, left_parenthesis_index);
        int index;
        string arg_input;
        if ((index = line.find(function_name)) != string::npos) {
            left_parenthesis_index = line.find('(', index);
            right_parenthesis_index = line.find(')', index);
            arg_input = line.substr(left_parenthesis_index + 1,
                                    right_parenthesis_index - left_parenthesis_index - 1);
            int replace_index;
            string tmpValue = macro_value;
            if ((replace_index = macro_value.find("##")) != string::npos) {
                tmpValue.replace(0, replace_index + 3, arg_input);
            } else if ((replace_index = macro_value.find_last_of("\"#")) != string::npos) {
                arg = "\"#" + arg;
                tmpValue.replace(replace_index, arg.length(), "\"" + arg_input + "\"");
            } else {
                replace_index = macro_value.find(arg);
                tmpValue.replace(replace_index, arg.length(), arg_input);
            }
            line.replace(index, right_parenthesis_index - index + 1, tmpValue);
        }
        return true;
    }

    void notFunctionHandler(const string &name) {
        if (name.empty() || line.find(name) == string::npos || macro_value.empty())
            return;
        int index;
        if ((index = line.find(name)) != string::npos)
            line.replace(index, name.length(), macro_value);
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
        should_read = (macro_name == "1");
    }

    void endifHandler() {
        if (!should_read_stack.empty()) {
            should_read = should_read_stack.top();
            should_read_stack.pop();
        }
    }

    bool includeOtherFile(string filename) {
        if (filename == "iostream") return false;
        filename = "test/" + filename;
        string file;
        ifstream is(filename);
        if (!is.is_open()) {
            cout << "Broken input " + filename;
            return false;
        } else {
            string reading_line;
            while (getline(is, reading_line)) file.append(reading_line).push_back('\n');
            is.close();
        }
        stack<bool> temStack = should_read_stack;
        bool tmpBool = should_read;
        while (!should_read_stack.empty()) should_read_stack.pop();
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
        while (tmpStr != nullptr) {
            resultVec.emplace_back(string(tmpStr));
            tmpStr = strtok(nullptr, pattern.c_str());
        }
        delete[] strTmp;
        return resultVec;
    }
};
