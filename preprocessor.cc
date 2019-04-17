/**
 * @file preprocessor.cc
 * @brief The implementation of the preprocessor
 *
 * @copyright This file is part of the lab in Object-oriented
 * programming course, a student project by Wang, Chen
 *
 * @author Wang, Chen(Student ID:16307110064)
 * @date April 8th, 2019
 *
 */
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <stack>
#include <cstring>

using namespace std;

/**
 * @brief The encapsulated implementation of the preprocessor
 *
 * @details
 * The class for the implementation of the preprocessor, only the constructor,
 * destructor and an entrance for input raw code string and output processed
 * string are public methods.
 * \par
 * A instance should be instantiated before utilizing these methods and the
 * pre_process method should be called to accomplish the task.
 *
 */
class Preprocessor {
public:
    /**
     * A trivial constructor because there is no configuration needed in the
     * construction process
     */
    Preprocessor() = default;

    /**
     * The entrance method for the outside class to get the processed code
     *
     * @param raw_code The entire string of the input file
     * @return The entire string of the output file
     */
    string pre_process(const string &raw_code) {
        vector<string> raw_codes = Preprocessor::split_string(raw_code, "\n");
        for (auto &code:raw_codes) {
            line = code;
            process_instruction();
        }
        return processed_code;
    }

    virtual ~Preprocessor() {
        processed_code.erase();
        macro_name.erase();
        macro_value.erase();
        line.erase();
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
            macro_name = line.substr(line.find(' ', 2) + 1);
            if (line.find("else") != string::npos) {
                should_read = !should_read;
                return;
            } else if (line.find("endif") != string::npos) {
                macro_end_if();
                return;
            } else if (line.find("ifdef") != string::npos) {
                macro_if_def();
                return;
            } else if (line.find("ifndef") != string::npos) {
                macro_if_not_def();
                return;
            } else if (line.find("undef") != string::npos) {
                macros.erase(macro_name);
                return;
            }
            if (should_read) {
                if (line.find("include") != string::npos) {
                    macro_include();
                    return;
                } else if (line.find("define") != string::npos) {
                    int index = macro_name.find(' ');
                    macro_value = macro_name.substr(index + 1);
                    macro_name = macro_name.substr(0, index);
                    macro_define();
                    return;
                } else if (line.find("if") != string::npos) {
                    macro_if();
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
                replace_function(macro_name.substr(0, macro_name.find('(') + 1));
            else
                replace_macro(macro_name);
            iterator++;
        }
        processed_code.append(line).push_back('\n');
    }

    bool replace_function(const string &name) {
        if (name.empty() || line.find(name) == string::npos || macro_value.empty())
            return false;
        int left_parenthesis_index = macro_name.find('(');
        int right_parenthesis_index = macro_name.find(')');
        string arg = macro_name.substr(
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
            string replaced_text = macro_value;
            if ((replace_index = macro_value.find("##")) != string::npos)
                replaced_text.replace(0, replace_index + 3, arg_input);
            else if ((replace_index = macro_value.find_last_of("\"#")) != string::npos) {
                arg = "\"#" + arg;
                replaced_text.replace(replace_index, arg.length(), "\"" + arg_input + "\"");
            } else {
                replace_index = macro_value.find(arg);
                replaced_text.replace(replace_index, arg.length(), arg_input);
            }
            line.replace(index, right_parenthesis_index - index + 1, replaced_text);
        }
        return true;
    }

    void replace_macro(const string &name) {
        if (name.empty() || line.find(name) == string::npos || macro_value.empty())
            return;
        int index;
        if ((index = line.find(name)) != string::npos)
            line.replace(index, name.length(), macro_value);
    }

    void macro_include() {
        string filename = macro_name.substr(1, macro_name.length() - 2);
        if (macro_name.find('<') == 0 || !includeOtherFile(filename))
            processed_code.append("#include ").append(macro_name).push_back('\n');
    }

    void macro_define() {
        macros.erase(macro_name);
        while (macros.count(macro_value)) macro_value = macros[macro_value];
        macros.insert(map<string, string>::value_type(macro_name, macro_value));
    }

    void macro_if_def() {
        should_read_stack.push(should_read);
        should_read = (macros.count(macro_name) != 0);
    }

    void macro_if_not_def() {
        should_read_stack.push(should_read);
        should_read = (macros.count(macro_name) == 0);
    }

    void macro_if() {
        should_read_stack.push(should_read);
        while (macros.count(macro_name) != 0) macro_name = macros[macro_name];
        should_read = (macro_name == "1");
    }

    void macro_end_if() {
        if (!should_read_stack.empty()) {
            should_read = should_read_stack.top();
            should_read_stack.pop();
        }
    }

    bool includeOtherFile(string filename) {
        if (filename == "iostream") return false;
        filename = "test/" + filename;
        string code;
        ifstream is(filename);
        if (!is.is_open()) {
            cout << "Broken input " + filename;
            return false;
        } else {
            string reading_line;
            while (getline(is, reading_line)) code.append(reading_line).push_back('\n');
            is.close();
        }
        stack<bool> temp_stack = should_read_stack;
        bool temp_bool = should_read;
        while (!should_read_stack.empty()) should_read_stack.pop();
        pre_process(code);
        should_read_stack = temp_stack;
        should_read = temp_bool;
        return true;
    }

    static vector<string> split_string(const string &str, const string &pattern) {
        char *char_array = new char[strlen(str.c_str()) + 1];
        strcpy(char_array, str.c_str());
        vector<string> vector;
        char *string1 = strtok(char_array, pattern.c_str());
        while (string1 != nullptr) {
            vector.emplace_back(string(string1));
            string1 = strtok(nullptr, pattern.c_str());
        }
        delete[] char_array;
        return vector;
    }
};
