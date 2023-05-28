#include "structures.hpp"

std::ostream& operator<<(std::ostream& stream, const Point& x) {
    stream << x.x << " " << x.y;
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Rank& x) {
    stream << x.f << " " << x.s;
    return stream;
}