#include <ostream>

#include "util.h"

std::ostream &operator<<(std::ostream &stream, const Plumber::Vec &v)
{
    stream << "(" << v.getX() << ", " << v.getY() << ")";
    return stream;
}