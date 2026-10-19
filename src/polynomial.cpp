#include "lab02/polynomial.hpp"

#include <algorithm>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace lab02 {

Polynomial::Polynomial() = default;

Polynomial::Polynomial(int degree) {
    degree_ = degree;
    if (degree >= 0) {
        coefficients_ = new double[static_cast<std::size_t>(degree) + 1]{};
    }
}

Polynomial::Polynomial(const double* coefficients, int degree) {
    if (degree < -1 || (degree >= 0 && coefficients == nullptr)) {
        throw std::invalid_argument("Invalid polynomial coefficients or degree");
    }
    while (degree >= 0 && coefficients[degree] == 0.0) {
        --degree;
    }
    if (degree >= 0) {
        coefficients_ = new double[static_cast<std::size_t>(degree) + 1];
        std::copy_n(coefficients, static_cast<std::size_t>(degree) + 1, coefficients_);
    }
    degree_ = degree;
}

Polynomial::Polynomial(const Polynomial& other) : Polynomial(other.coefficients_, other.degree_) {}

Polynomial::Polynomial(Polynomial&& other)
    : degree_(std::exchange(other.degree_, -1)), coefficients_(std::exchange(other.coefficients_, nullptr)) {}

Polynomial& Polynomial::operator=(Polynomial other) {
    swap(other);
    return *this;
}

Polynomial::~Polynomial() {
    delete[] coefficients_;
}

void Polynomial::swap(Polynomial& other) {
    std::swap(degree_, other.degree_);
    std::swap(coefficients_, other.coefficients_);
}

int Polynomial::degree() const {
    return degree_;
}

double Polynomial::operator[](std::size_t index) const {
    if (degree_ < 0 || index > static_cast<std::size_t>(degree_)) {
        throw std::out_of_range("Coefficient index is out of range");
    }
    return coefficients_[index];
}

void Polynomial::trim() {
    int new_degree = degree_;
    while (new_degree >= 0 && coefficients_[new_degree] == 0.0) {
        --new_degree;
    }
    if (new_degree == degree_) {
        return;
    }
    Polynomial result(new_degree);
    if (new_degree >= 0) {
        std::copy_n(coefficients_, static_cast<std::size_t>(new_degree) + 1, result.coefficients_);
    }
    swap(result);
}

void Polynomial::add(const Polynomial& other) {
    Polynomial result(std::max(degree_, other.degree_));
    for (int i = 0; i <= result.degree_; ++i) {
        if (i <= degree_) {
            result.coefficients_[i] += coefficients_[i];
        }
        if (i <= other.degree_) {
            result.coefficients_[i] += other.coefficients_[i];
        }
    }
    result.trim();
    swap(result);
}

void Polynomial::mul(const Polynomial& other) {
    if (degree_ < 0 || other.degree_ < 0) {
        Polynomial result;
        swap(result);
        return;
    }
    if (degree_ > std::numeric_limits<int>::max() - other.degree_) {
        throw std::length_error("Polynomial degree is too large");
    }
    Polynomial result(degree_ + other.degree_);
    for (int i = 0; i <= degree_; ++i) {
        for (int j = 0; j <= other.degree_; ++j) {
            result.coefficients_[i + j] += coefficients_[i] * other.coefficients_[j];
        }
    }
    result.trim();
    swap(result);
}

double Polynomial::evaluate(double x) const {
    double result{};
    for (int i = degree_; i >= 0; --i) {
        result = result * x + coefficients_[i];
    }
    return result;
}

void Polynomial::print(std::ostream& out) const {
    if (degree_ < 0) {
        out << 0;
        return;
    }
    bool first = true;
    for (int i = degree_; i >= 0; --i) {
        const double value = coefficients_[i];
        if (value == 0.0) {
            continue;
        }
        if (value < 0.0) {
            out << (first ? "-" : " - ");
        } else if (!first) {
            out << " + ";
        }
        const double magnitude = value < 0.0 ? -value : value;
        if (i == 0 || magnitude != 1.0) {
            out << magnitude;
        }
        if (i > 0) {
            out << 'x';
            if (i > 1) {
                out << '^' << i;
            }
        }
        first = false;
    }
}

Polynomial Polynomial::derivative() const {
    if (degree_ <= 0) {
        return Polynomial();
    }
    Polynomial result(degree_ - 1);
    for (int i = 1; i <= degree_; ++i) {
        result.coefficients_[i - 1] = coefficients_[i] * i;
    }
    result.trim();
    return result;
}

Polynomial Polynomial::integral() const {
    if (degree_ < 0) {
        return Polynomial();
    }
    if (degree_ == std::numeric_limits<int>::max()) {
        throw std::length_error("Polynomial degree is too large");
    }
    Polynomial result(degree_ + 1);
    for (int i = 0; i <= degree_; ++i) {
        result.coefficients_[i + 1] = coefficients_[i] / (i + 1.0);
    }
    result.trim();
    return result;
}

}  // namespace lab02
