#pragma once
#include <cmath>

inline double curve_fn(double x) {
    return x * x * x * x;
}

inline double curve_derivative(double x) {
    return 4 * x * x * x;
}

inline bool collide(const double x, const double y) {
    return y <= curve_fn(x);
}


