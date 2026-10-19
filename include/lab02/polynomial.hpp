#pragma once

#include <cstddef>
#include <iosfwd>

namespace lab02 {

class Polynomial {
public:
    Polynomial();
    Polynomial(const double* coefficients, int degree);
    Polynomial(const Polynomial& other);
    Polynomial(Polynomial&& other);
    Polynomial& operator=(Polynomial other);
    ~Polynomial();

    void swap(Polynomial& other);
    int degree() const;
    double operator[](std::size_t index) const;
    void trim();
    void add(const Polynomial& other);
    void mul(const Polynomial& other);
    double evaluate(double x) const;
    void print(std::ostream& out) const;
    Polynomial derivative() const;
    Polynomial integral() const;

private:
    explicit Polynomial(int degree);

    int degree_ = -1;
    double* coefficients_ = nullptr;
};

}  // namespace lab02
