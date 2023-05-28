#pragma once

#include <iostream>

struct Point {
    int x;
    int y;
};

struct Rank {
    Point f;
    Point s;
};

std::ostream& operator<<(std::ostream& stream, const Point& x);
std::ostream& operator<<(std::ostream& stream, const Rank& x);