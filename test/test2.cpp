//
// WARNING: You are not allowed to modify this file!
//
#include "iostream"
#include "test2.h"

#define FALSE 0
#define TRUE 1
#define PRODUCT (a*b)
#define EXPRESS PRODUCT+TRUE

int main() {


    a = 8;
    b = 2;
    int div = divide(a, b);
    if (div == 4) {
        std::cout << "#include pass!" << std::endl;
    }


#define PART1

#ifdef PART1
    std::cout << "#define check1 pass!" << std::endl;
    std::cout << "#ifdef pass!" << std::endl;
#endif

#if TRUE
    std::cout << "#define check2 pass!" << std::endl;
#else
    std::cout<<"#define check2 fail!"<<std::endl;
#endif

#ifdef TRUE
    std::cout << "#define check3 pass!" << std::endl;
#endif

    int p = PRODUCT;
    if (p == 16) {
        std::cout << "#define check4 pass!" << std::endl;
    } else {
        std::cout << "#define check4 fail!" << std::endl;
    }

    int e = EXPRESS;
    if (e == 17) {
        std::cout << "#define check5 pass!" << std::endl;
    } else {
        std::cout << "#define check5 fail!" << std::endl;
    }

#undef PART1
#ifdef PART1
    std::cout<<"#undef fail!"<<std::endl;
#else
    std::cout << "#undef pass! " << std::endl;

#endif


#ifndef PART1
    std::cout << "#ifndef pass!" << std::endl;
#endif

#if FALSE
    std::cout<<"#if fail!"<<std::endl;
#else
#if 1
    std::cout << "#if pass!" << std::endl;
#else
    std::cout<<"#if fail!"<<std::endl;
#endif
    std::cout << "#else pass!" << std::endl;
#endif


#define PART2
#ifdef PART2
#define FUNC(arg) (2*arg)
    int d = FUNC(a);
    if (d == 2 * a) {
        std::cout << "PART 2 pass!!!!!" << std::endl;
    } else {
        std::cout << "PART 2 fail!!!!!" << std::endl;
    }
#endif


#define PART3
#ifdef PART3
#define Cong(arg) "# "#arg
    std::cout << Cong(pass) << "!!!!!" << std::endl;

#define Token_con(arg) arg ## _suffix
    int a_suffix = 1;
    if (Token_con(a)) {
        std::cout << "## pass!!!!!" << std::endl;
    }

#endif

    return 0;
}
