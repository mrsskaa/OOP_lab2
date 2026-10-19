#include <iostream>

#include "lab02/polynomial.hpp"

int main() {
    const double coefficients[]{1.0, 2.0, 3.0};
    const lab02::Polynomial polynomial(coefficients, 2);
    std::cout << "p(x) = ";
    polynomial.print(std::cout);
    std::cout << "\np(2) = " << polynomial.evaluate(2.0);
    std::cout << "\np'(x) = ";
    polynomial.derivative().print(std::cout);
    std::cout << "\nIntegral (C = 0): ";
    polynomial.integral().print(std::cout);
    std::cout << '\n';
}
