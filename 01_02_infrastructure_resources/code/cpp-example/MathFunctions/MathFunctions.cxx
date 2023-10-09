#include "MathFunctions.h"

#include <cmath>

namespace mathfunctions {
double sqrt(double x) { return std::sqrt(x); }

unsigned int Factorial(unsigned int number) {
  return number <= 1 ? number : Factorial(number - 1) * number;
}

} // namespace mathfunctions
