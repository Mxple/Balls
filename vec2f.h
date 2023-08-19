#pragma once

inline double inv_sqrt( float n ) {
   const double threehalfs = 1.5F;
   double y = n;
   
   long i = * ( long * ) &y;

   i = 0x5fe6eb50c7b537a9 - ( i >> 1 );
   y = * ( double * ) &i;
   
   y = y * ( threehalfs - ( (n * 0.5F) * y * y ) );
   
   return y;
}

struct Vec2f {
    double x, y;

    Vec2f() {
        x = 0;
        y = 0;
    }

    Vec2f(double xx, float yy) {
        x = xx;
        y = yy;
    }

    double dot(Vec2f& other) {
        return x * other.x + y * other.y;
    }

    Vec2f& scale(double scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vec2f& normalize() {
        double inverse_sqrt = inv_sqrt(x * x + y * y);
        x *= inverse_sqrt;
        y *= inverse_sqrt;
        return *this;
    }

    Vec2f operator + (const Vec2f &other) const { return Vec2f(x + other.x, y + other.y); }
    Vec2f operator - (const Vec2f &other) const { return Vec2f(x - other.x, y - other.y); }
    Vec2f& operator += (const Vec2f &other) { x += other.x, y += other.y; return *this; } 
    Vec2f operator * (const double &f) const { return Vec2f(x * f, y * f); }
};
