#pragma once

#include <fstream>
#include <vector>
#include "field.hpp"
#include "structures.hpp"

Field read_field(const std::string& file_name);

void SaveRanks(const std::vector<std::vector<int>>& ranks, std::ofstream& fout);
void SavePath(const std::vector<Point>& path, std::ofstream& fout);