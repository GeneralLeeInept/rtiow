#pragma once

#include <cmath>

class Vec3
{
public:
    union
    {
        struct { double x; double y; double z; };
        struct { double v[3]; };
    };

    Vec3() = default;
    Vec3(Vec3&) = default;
    Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    Vec3& operator=(const Vec3&) = default;
    Vec3 operator-() { return Vec3(-x, -y, -z); }

    Vec3& operator +=(const Vec3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; }
    Vec3& operator *=(double t) { x *= t; y *= t; z *= t; }
    Vec3& operator /=(double t) { x /= t; y /= t; z /= t; }

    double lengthSq() const { return x * x + y * y + z * z; }
    double length() const { return std::sqrt(lengthSq()); }
};

inline Vec3 operator*(const Vec3& a, double t)
{
    return Vec3(a.x * t, a.y * t, a.z * t);
}

inline Vec3 operator/(const Vec3& a, double t)
{
    return Vec3(a.x / t, a.y / t, a.z / t);
}

inline Vec3 operator*(const Vec3& a, const Vec3& b)
{
    return Vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

inline Vec3 operator+(const Vec3& a, const Vec3& b)
{
    return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vec3 operator-(const Vec3& a, const Vec3& b)
{
    return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vec3 normalize(const Vec3& a)
{
    return a / a.length();
}

inline Vec3 cross(const Vec3& a, const Vec3& b)
{
    return Vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

inline double dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
