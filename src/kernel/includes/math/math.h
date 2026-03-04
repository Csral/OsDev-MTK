#ifndef _CRT_STDMATH_H_
#define _CRT_STDMATH_H_

#include "../kernel.h"

long int pow(int num, int exp) {

    if (num == 0) return 0;
    else if (num == 1) return 1;

    if (exp < 0) {
        print("Negative exponents not supported!\n");
        return 1;
    }

    // unsigned char is_exp_neg = 0;

    // handle this with pow_double function
    // add pow2 function

    if (exp == 0) return 1;
    // if (exp < 0) is_exp_neg = 1;

    long int ans = num;
    long int base = 1;

    while (exp > 1) {

        if (exp % 2 == 1) {
            base *= ans;
            exp--;
        }

        ans *= ans;
        exp /= 2;

    }

    ans *= base;
    return ans;

};

#endif