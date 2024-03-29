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
 *
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

    /**
     * The destructor cleans up the data fields used in this class
     * Basically, it erases all the strings, clears the map structure
     * and pops all the element in the stack.
     */
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
    /**
     * The string containing all the processed code, line by line
     */
    string processed_code;
    /**
     * A map of all the macro definitions
     */
    map<string, string> macros;
    /**
     * The current processing macro name
     */
    string macro_name;
    /**
     * The current processing macro value
     */
    string macro_value;
    /**
     * The current reading line in the raw code
     */
    string line;
    /**
     * The flag signature indicating the if-else control
     */
    bool should_read = true;
    /**
     * The stack is used to handle nested condition controls
     * Each nested logic control will have one bool element put
     * into the stack and the stack will pop a bool element when
     * one control logic meets the end, i.e. the endif statement
     */
    stack<bool> should_read_stack;

    /**
     * @brief The main routine of the processing logic this method
     * is called for each line in the raw code.
     *
     * @arg The contend of the line is not passed as an argument but
     * stored in the class field because other methods will also
     * use the value.
     *
     * @details It has the following functions:
     * 1) Skips the statements starting with "//", which will be
     *      regarded as comments;
     * 2) Regards any line starting with "#" as a macro and process
     *      the macro correspondingly, the characters between the
     *      sharp symbol and the first whitespace will be regarded
     *      as the name of the macro and different kind of macros
     *      are treated separately;
     * 3) Any line not meeting the two requirements above are
     *      treated as general codes and will be processed to
     *      handle the needs of macro substitution, logic control
     *      and so on.
     *
     * @retval The processed code for each raw line will be put in
     * the class variable @code{processed_code}.
     */
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

    /**
     * @brief The main handler for the normal code processing.
     *
     * @arg The contend of the line is not passed as an argument but
     * stored in the class field because other methods will also
     * use the value.
     *
     * @details The iterator of the map structure is used to determine
     * if there are any macro in the code line.
     * Furthermore, this separates the functional macros(those with
     * parenthesis and arguments) from the un-functional macros(those
     * without parenthesis). In each condition, an additional method is
     * called to replace the corresponding macro.
     *
     * @retval The processed code for each raw line will be put in
     * the class variable @code{processed_code}.
     */
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

    /**
     * @brief Replaces the functional macro appeared in codes with its
     * definitions
     *
     * @details String operation is used to replace the macros while
     * retaining the arguments as the component of the macro
     * @param name The name of the macro
     */
    void replace_function(const string &name) {
        if (name.empty() || line.find(name) == string::npos || macro_value.empty())
            return;
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
    }

    /**
     * @brief Replaces the non-functional macro appeared in codes with
     * its definitions
     *
     * @details String operation is used to replace the macros while
     * retaining the arguments as the component of the macro
     * @param name The name of the macro
     */
    void replace_macro(const string &name) {
        if (name.empty() || line.find(name) == string::npos || macro_value.empty())
            return;
        int index;
        if ((index = line.find(name)) != string::npos)
            line.replace(index, name.length(), macro_value);
    }

    /**
     * @brief The methods below are all the handlers of each curcumstance
     */
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

    /**
     * @brief Includes header files at the beginning of the file
     *
     * @details This method has the following behaviour in our lab
     * 1) All library header files are left as they are to the output
     *      file;
     * 2) In the test file, only "iostream" library header file is
     *      considered;
     * 3) Other non-library header files are processed normally;
     * 4) Non-library header files are in the test folder;
     * 5) In my version of implementation, the non-library header
     *      files are processed recursively.
     * @param filename The name of the header file
     * @return If the header file is not a library file and the reading
     * of the file is successful, then returns @code{true}; otherwise
     * @code{false}.
     */
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

    /**
     * The helper static method splitting the input string into vectors
     * @param str The string to be split
     * @param pattern The deliminator of the splitting
     * @return The vector containing each part of the string
     */
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
