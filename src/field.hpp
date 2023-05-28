#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "field.hpp"

// Builds field representation from input file
// Gives opportunity to build ranks
class Field {
public:
    using FieldElement = int;

    Field(std::vector<std::string> in);

    std::vector<FieldElement>& operator[](int i);
    const std::vector<FieldElement>& operator[](int i) const;

    void Resize(int w, int h);

    // Build ranks
    std::vector<std::vector<FieldElement>> Solve();

private:
    int width_;
    int height_;
    int spaces_;
    std::vector<std::vector<FieldElement>> field_;
    std::vector<std::vector<FieldElement>> sol_;
};
