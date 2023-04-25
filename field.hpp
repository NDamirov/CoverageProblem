#pragma once

#include "structures.hpp"
#include "field.hpp"
#include <vector>
#include <string>
#include <fstream>

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

Field read_field(const std::string& file_name);