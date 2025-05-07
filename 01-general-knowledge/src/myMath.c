#include <stdio.h>

double add(double a, double b)
{
    return (a + b);
}

double subtract(double a, double b)
{
    return (a - b);
}

double multiply(double a, double b)
{
    return (a * b);
}

double divide(double a, double b)
{
    if (b == 0) {
        printf("Error: Sorry, can not devided by zero !!!\n");
        return 0;
    }
    return (a / b);
}

unsigned long long factorial(int n)
{
    if (n < 0) {
        printf("Error: Sorry, factorial does not exist for negative numbers !!!\n");
        return 0;
    }
    unsigned long long result = 1;
    for(int i = 1; i <= n; i++){
        result *= i;
    }
    return result;
}

double power(double base, int exponent)
{
    if (exponent == 0) {
        return 1;
    }
    double result = 1.0;
    if (exponent < 0) {
        for (int i = exponent; i < 0 ; i++) {
            result *= base;
        }
        result = 1 / result;
    } else {
        for (int i = 0; i < exponent ; i++) {
            result *= base;
        }
    }

    return result;
}

int is_prime(int n)
{
    if ((n == 0) || (n == 1)) {
        return 1;
    }
    int result = 1;
    for (int i = 2; i <= n / 2; i++) {
        if (!(n % i)) {
            result = 0;
            break;
        }
    }
    return result;
}
