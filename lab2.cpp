/**
 * @file lab2.cpp
 * @brief This is the executable entrance file of the simple C/C++
 * preprocessor application
 *
 * @copyright This file is part of the lab in Object-oriented
 * programming course, a student project by Wang, Chen
 *
 * @author Wang, Chen(Student ID:16307110064)
 * @date April 8th, 2019
 *
 */

#include "preprocessor.cc"

string get_unprocessed_code(int number);

void put_processed_code(int number, string code);

void run_test(int test_case_number);

int main() {
    for (int test_case_number = 1; test_case_number <= 2; test_case_number++) {
        run_test(test_case_number);
    }
    return 0;
}

void run_test(int test_case_number) {
    string raw_code = get_unprocessed_code(test_case_number);
    Preprocessor preprocessor;
    string processed_code = preprocessor.pre_process(raw_code);
    put_processed_code(test_case_number, processed_code);
}

/**
 * @brief Read the files to be processed from file and get as a string
 *
 * The input files should be in the ./test folder and having the naming
 * method: test*.cpp, where * can be a number from 1 to 2.
 *
 * @param number The number of the current test case
 * @return The content of the file, all in one string
 */
string get_unprocessed_code(int number) {
    string filename = "test/test" + to_string(number) + ".cpp";
    string file;
    ifstream is(filename);
    if (!is.is_open()) {
        cout << "Broken input file.";
    } else {
        string line;
        while (getline(is, line)) {
            file.append(line).push_back('\n');
        }
        is.close();
    }
    return file;
}

/**
 * @brief Put the string to the destination file as the result of the
 * pre-processing process.
 *
 * The output files will be in the ./test folder and having the naming
 * method: test*.out.cpp, where * can be a number from 1 to 2.
 *
 * @param number The number of the current test case
 * @param code The string of the pre-processed code to be put to the
 * destination file
 */
void put_processed_code(int number, string code) {
    string filename = "test/test" + to_string(number) + ".out.cpp";
    ofstream os(filename);
    if (!os.is_open()) {
        cout << "Broken output file.";
    } else {
        os << code;
        os.close();
    }
}
