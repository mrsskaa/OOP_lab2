#include <iostream>
#include <limits>

#include "lab02/polynomial.hpp"

namespace {

constexpr int kMaxDegree = 1000;

void ClearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Ошибка ввода.\n";
}

bool ReadPolynomial(lab02::Polynomial& polynomial) {
    int degree{};
    std::cout << "Степень от -1 до " << kMaxDegree << " (-1 — нулевой многочлен): ";
    if (!(std::cin >> degree) || degree < -1 || degree > kMaxDegree) {
        if (!std::cin.eof() && !std::cin.bad()) {
            ClearInput();
        }
        return false;
    }

    double coefficients[kMaxDegree + 1]{};
    for (int i = 0; i <= degree; ++i) {
        std::cout << "Коэффициент при x^" << i << ": ";
        if (!(std::cin >> coefficients[i])) {
            if (!std::cin.eof() && !std::cin.bad()) {
                ClearInput();
            }
            return false;
        }
    }
    polynomial = lab02::Polynomial(coefficients, degree);
    return true;
}

}  // namespace

int main() {
    lab02::Polynomial polynomial;
    while (true) {
        std::cout << "\nТекущий многочлен: ";
        polynomial.print(std::cout);
        std::cout
            << "\n1. Ввести многочлен\n2. Прибавить многочлен\n3. Умножить на многочлен\n"
               "4. Значение при x\n5. Производная\n6. Интеграл (C = 0)\n0. Выход\nКоманда: ";
        int command{};
        if (!(std::cin >> command)) {
            if (std::cin.eof() || std::cin.bad()) {
                return 0;
            }
            ClearInput();
            continue;
        }

        switch (command) {
            case 0:
                return 0;
            case 1:
            case 2:
            case 3: {
                lab02::Polynomial other;
                if (!ReadPolynomial(other)) {
                    break;
                }
                if (command == 1) {
                    polynomial.swap(other);
                } else if (command == 2) {
                    polynomial.add(other);
                } else {
                    polynomial.mul(other);
                }
                break;
            }
            case 4: {
                double x{};
                std::cout << "x = ";
                if (!(std::cin >> x)) {
                    if (!std::cin.eof() && !std::cin.bad()) {
                        ClearInput();
                    }
                    break;
                }
                std::cout << "p(x) = " << polynomial.evaluate(x) << '\n';
                break;
            }
            case 5:
                std::cout << "Производная: ";
                polynomial.derivative().print(std::cout);
                std::cout << '\n';
                break;
            case 6:
                std::cout << "Интеграл (C = 0): ";
                polynomial.integral().print(std::cout);
                std::cout << '\n';
                break;
            default:
                std::cout << "Нет такой команды.\n";
                break;
        }
    }
}
