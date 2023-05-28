#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "field.hpp"

class Field {
public:
    using FieldElement = int;

    Field(std::vector<std::string> in);

    std::vector<FieldElement>& operator[](int i);
    const std::vector<FieldElement>& operator[](int i) const;

    void Resize(int w, int h);
    std::vector<std::vector<FieldElement>> Solve();

private:
    int width_;
    int height_;
    int spaces_;
    std::vector<std::vector<FieldElement>> field_;
    std::vector<std::vector<FieldElement>> sol_;
};
