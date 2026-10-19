#include "lab02/polynomial.hpp"

#include <ostream>
#include <stdexcept>
#include <streambuf>
#include <utility>

#include <gtest/gtest.h>

namespace lab02 {
namespace {

void ExpectCoefficients(const Polynomial& polynomial, const double* expected, int degree) {
    ASSERT_EQ(polynomial.degree(), degree);
    for (int i = 0; i <= degree; ++i) {
        EXPECT_DOUBLE_EQ(polynomial[i], expected[i]);
    }
}

class OutputBuffer : public std::streambuf {
public:
    OutputBuffer() {
        setp(data_, data_ + sizeof(data_) - 1);
    }

    const char* data() const {
        return data_;
    }

private:
    char data_[128]{};
};

TEST(PolynomialTest, DefaultIsZero) {
    const Polynomial polynomial;
    EXPECT_EQ(polynomial.degree(), -1);
    EXPECT_DOUBLE_EQ(polynomial.evaluate(5.0), 0.0);
}

TEST(PolynomialTest, CopiesInputCoefficients) {
    double values[]{1.0, 2.0, 3.0};
    const Polynomial polynomial(values, 2);
    values[0] = 9.0;
    const double expected[]{1.0, 2.0, 3.0};
    ExpectCoefficients(polynomial, expected, 2);
}

TEST(PolynomialTest, RemovesTrailingZeroCoefficients) {
    const double values[]{2.0, 1.0, 0.0, 0.0};
    Polynomial polynomial(values, 3);
    polynomial.trim();
    ExpectCoefficients(polynomial, values, 1);
}

TEST(PolynomialTest, AllZeroCoefficientsGiveZeroPolynomial) {
    const double values[]{0.0, 0.0, 0.0};
    const Polynomial polynomial(values, 2);
    EXPECT_EQ(polynomial.degree(), -1);
}

TEST(PolynomialTest, AcceptsEmptyCoefficientRange) {
    const Polynomial polynomial(nullptr, -1);
    EXPECT_EQ(polynomial.degree(), -1);
}

TEST(PolynomialTest, CopyConstructorMakesIndependentObject) {
    const double values[]{1.0, 2.0};
    Polynomial original(values, 1);
    const Polynomial copy(original);
    original.add(original);
    ExpectCoefficients(copy, values, 1);
    const double doubled[]{2.0, 4.0};
    ExpectCoefficients(original, doubled, 1);
}

TEST(PolynomialTest, MoveConstructorEmptiesSource) {
    const double values[]{1.0, 2.0};
    Polynomial source(values, 1);
    const Polynomial destination(std::move(source));
    ExpectCoefficients(destination, values, 1);
    EXPECT_EQ(source.degree(), -1);
    EXPECT_DOUBLE_EQ(source.evaluate(2.0), 0.0);
    source.add(destination);
    ExpectCoefficients(source, values, 1);
}

TEST(PolynomialTest, CopyAssignmentReplacesBuffer) {
    const double values[]{3.0, 4.0, 5.0};
    Polynomial source(values, 2);
    const double old[]{9.0};
    Polynomial destination(old, 0);
    destination = source;
    source.add(source);
    ExpectCoefficients(destination, values, 2);
}

TEST(PolynomialTest, MoveAssignmentEmptiesSource) {
    const double values[]{3.0, 4.0};
    Polynomial source(values, 1);
    const double old[]{8.0};
    Polynomial destination(old, 0);
    destination = std::move(source);
    ExpectCoefficients(destination, values, 1);
    EXPECT_EQ(source.degree(), -1);
}

TEST(PolynomialTest, SelfCopyAssignmentPreservesValue) {
    const double values[]{1.0, 2.0};
    Polynomial polynomial(values, 1);
    Polynomial& alias = polynomial;
    polynomial = alias;
    ExpectCoefficients(polynomial, values, 1);
}

TEST(PolynomialTest, SelfMoveAssignmentPreservesValue) {
    const double values[]{1.0, 2.0};
    Polynomial polynomial(values, 1);
    Polynomial& alias = polynomial;
    polynomial = std::move(alias);
    ExpectCoefficients(polynomial, values, 1);
}

TEST(PolynomialTest, CopiesMovesAndAssignsZero) {
    Polynomial zero;
    const Polynomial copy(zero);
    const Polynomial moved(std::move(zero));
    const double values[]{7.0};
    Polynomial destination(values, 0);
    destination = copy;
    EXPECT_EQ(copy.degree(), -1);
    EXPECT_EQ(moved.degree(), -1);
    EXPECT_EQ(destination.degree(), -1);
}

TEST(PolynomialTest, SwapExchangesStates) {
    const double values[]{2.0, 3.0};
    Polynomial polynomial(values, 1);
    Polynomial zero;
    polynomial.swap(zero);
    EXPECT_EQ(polynomial.degree(), -1);
    ExpectCoefficients(zero, values, 1);
}

TEST(PolynomialTest, AddsDifferentDegrees) {
    const double left[]{1.0, 2.0};
    const double right[]{0.0, 1.0, 1.0};
    Polynomial polynomial(left, 1);
    const Polynomial other(right, 2);
    polynomial.add(other);
    const double expected[]{1.0, 3.0, 1.0};
    ExpectCoefficients(polynomial, expected, 2);
    ExpectCoefficients(other, right, 2);
}

TEST(PolynomialTest, AdditionCanReduceDegreeOrGiveZero) {
    const double left[]{1.0, 2.0};
    const double right[]{3.0, -2.0};
    Polynomial polynomial(left, 1);
    polynomial.add(Polynomial(right, 1));
    const double expected[]{4.0};
    ExpectCoefficients(polynomial, expected, 0);
    const double negative[]{-4.0};
    polynomial.add(Polynomial(negative, 0));
    EXPECT_EQ(polynomial.degree(), -1);
}

TEST(PolynomialTest, AddsZeroOnEitherSide) {
    const double values[]{1.0, 2.0};
    Polynomial polynomial(values, 1);
    Polynomial zero;
    polynomial.add(zero);
    ExpectCoefficients(polynomial, values, 1);
    zero.add(polynomial);
    ExpectCoefficients(zero, values, 1);
}

TEST(PolynomialTest, MultipliesPolynomials) {
    const double left[]{1.0, 2.0};
    const double right[]{0.0, 1.0, 1.0};
    Polynomial polynomial(left, 1);
    const Polynomial other(right, 2);
    polynomial.mul(other);
    const double expected[]{0.0, 1.0, 3.0, 2.0};
    ExpectCoefficients(polynomial, expected, 3);
    ExpectCoefficients(other, right, 2);
}

TEST(PolynomialTest, MultipliesByItself) {
    const double values[]{1.0, 1.0};
    Polynomial polynomial(values, 1);
    polynomial.mul(polynomial);
    const double expected[]{1.0, 2.0, 1.0};
    ExpectCoefficients(polynomial, expected, 2);
}

TEST(PolynomialTest, MultipliesByZeroOnEitherSide) {
    const double values[]{1.0, 2.0};
    Polynomial polynomial(values, 1);
    Polynomial zero;
    zero.mul(polynomial);
    EXPECT_EQ(zero.degree(), -1);
    polynomial.mul(zero);
    EXPECT_EQ(polynomial.degree(), -1);
}

TEST(PolynomialTest, EvaluatesAtPositiveNegativeAndZeroPoints) {
    const double values[]{1.0, 2.0, 3.0};
    const Polynomial polynomial(values, 2);
    EXPECT_DOUBLE_EQ(polynomial.evaluate(2.0), 17.0);
    EXPECT_DOUBLE_EQ(polynomial.evaluate(-2.0), 9.0);
    EXPECT_DOUBLE_EQ(polynomial.evaluate(0.0), 1.0);
}

TEST(PolynomialTest, DerivativeMatchesVariantExample) {
    const double values[]{1.0, 2.0, 3.0};
    const Polynomial polynomial(values, 2);
    const double expected[]{2.0, 6.0};
    ExpectCoefficients(polynomial.derivative(), expected, 1);
    ExpectCoefficients(polynomial, values, 2);
}

TEST(PolynomialTest, DerivativeOfConstantAndZeroIsZero) {
    const double values[]{5.0};
    EXPECT_EQ(Polynomial(values, 0).derivative().degree(), -1);
    EXPECT_EQ(Polynomial().derivative().degree(), -1);
}

TEST(PolynomialTest, IntegralMatchesVariantExample) {
    const double values[]{0.0, 2.0};
    const Polynomial polynomial(values, 1);
    const double expected[]{0.0, 0.0, 1.0};
    ExpectCoefficients(polynomial.integral(), expected, 2);
    ExpectCoefficients(polynomial, values, 1);
}

TEST(PolynomialTest, IntegralOfConstantAndZero) {
    const double values[]{5.0};
    const double expected[]{0.0, 5.0};
    ExpectCoefficients(Polynomial(values, 0).integral(), expected, 1);
    EXPECT_EQ(Polynomial().integral().degree(), -1);
}

TEST(PolynomialTest, IntegralDividesAllCoefficients) {
    const double values[]{1.0, -3.0, 2.0};
    const double expected[]{0.0, 1.0, -1.5, 2.0 / 3.0};
    ExpectCoefficients(Polynomial(values, 2).integral(), expected, 3);
}

TEST(PolynomialTest, PrintsSignsPowersAndSkipsZeroTerms) {
    const double values[]{-2.0, 1.0, 0.0, -1.0};
    OutputBuffer buffer;
    std::ostream out(&buffer);
    Polynomial(values, 3).print(out);
    EXPECT_STREQ(buffer.data(), "-x^3 + x - 2");
}

TEST(PolynomialTest, PrintsZero) {
    OutputBuffer buffer;
    std::ostream out(&buffer);
    Polynomial().print(out);
    EXPECT_STREQ(buffer.data(), "0");
}

TEST(PolynomialTest_Throw, RejectsInvalidConstructorArguments) {
    const double values[]{1.0};
    EXPECT_THROW(Polynomial(nullptr, 0), std::invalid_argument);
    EXPECT_THROW(Polynomial(values, -2), std::invalid_argument);
}

TEST(PolynomialTest_Throw, RejectsInvalidCoefficientIndex) {
    const double values[]{1.0, 2.0};
    const Polynomial polynomial(values, 1);
    EXPECT_THROW(polynomial[2], std::out_of_range);
    EXPECT_THROW(Polynomial()[0], std::out_of_range);
}

}  // namespace
}  // namespace lab02
