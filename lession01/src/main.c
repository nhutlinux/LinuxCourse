#include <stdio.h>
#include "myMath.h"

int main()
{
    printf("Addition:  7 + 3 is %.2f\n", add(7,3));
    printf("Subtraction:  7 - 3 is %.2f\n", subtract(7,3));
    printf("Multiplication: 7 * 3 is %.2f\n", multiply(7,3));
    printf("Division: 7 / 3 is %.2f\n", divide(7,3));
    printf("Factorial of 7 is %llu\n", factorial(7));
    printf("Power: 3^2 is %.2f\n", power(3,2));
    printf("Is 7 prime ? %s\n", is_prime(7) ? "Yes" : "No");
    return 0;
}