#include "iostream"

int a;
int b;

int divide(int arg1, int arg2) {
    return arg1 / arg2;
}

int main() {

    a = 8;
    b = 2;
    int div = divide(a, b);
    if (div == 4) {
        std::cout << "#include pass!" << std::endl;
    }
    std::cout << "#define check1 pass!" << std::endl;
    std::cout << "#ifdef pass!" << std::endl;
    std::cout << "#define check2 pass!" << std::endl;
    std::cout << "#define check3 pass!" << std::endl;
    int p = (a * b);
    if (p == 16) {
        std::cout << "#define check4 pass!" << std::endl;
    } else {
        std::cout << "#define check4 fail!" << std::endl;
    }
    int e = (a * b) + 1;
    if (e == 17) {
        std::cout << "#define check5 pass!" << std::endl;
    } else {
        std::cout << "#define check5 fail!" << std::endl;
    }
    std::cout << "#undef pass! " << std::endl;

    std::cout << "#ifndef pass!" << std::endl;
    std::cout << "#if pass!" << std::endl;
    std::cout << "#else pass!" << std::endl;
    int d = (2 * a);
    if (d == 2 * a) {
        std::cout << "PART 2 pass!!!!!" << std::endl;
    } else {
        std::cout << "PART 2 fail!!!!!" << std::endl;
    }
    std::cout << "# ""pass" << "!!!!!" << std::endl;
    int a_suffix = 1;
    if (a_suffix) {
        std::cout << "## pass!!!!!" << std::endl;
    }

    return 0;
}
