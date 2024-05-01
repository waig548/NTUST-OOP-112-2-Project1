#include <iostream>
#include "Vec.h"

namespace Plumber
{

    Vec::Vec() : Vec(0, 0)
    {
    }

    Vec::Vec(int x, int y) : x(x), y(y)
    {
    }

    Vec::Vec(const Vec &other) : Vec(other.x, other.y)
    {
    }

    Vec::~Vec()
    {
    }

    Vec &Vec::operator=(const Vec &other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    int Vec::getX() const
    {
        return x;
    }

    int Vec::getY() const
    {
        return y;
    }

    Vec Vec::operator+(const Vec &other) const
    {
        return add(*this, other);
    }

    Vec Vec::operator-(const Vec &other) const
    {
        return sub(*this, other);
    }

    Vec Vec::operator-() const
    {
        return Vec((-1) * x, (-1) * y);
    }

    bool Vec::operator==(const Vec &other) const
    {
        return equals(*this, other);
    }

    bool Vec::operator!=(const Vec &other) const
    {
        return !(*this == other);
    }

    Vec Vec::add(const Vec &a, const Vec &b)
    {
        return Vec(a.x + b.x, a.y + b.y);
    }

    Vec Vec::sub(const Vec &a, const Vec &b)
    {
        return a + (-b);
    }

    bool Vec::equals(const Vec &a, const Vec &b)
    {
        return a.x == b.x && a.y == b.y;
    }

} // namespace Plumber