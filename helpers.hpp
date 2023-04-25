#pragma once

#include <fstream>
#include <vector>
#include "structures.hpp"

void SaveRanks(const std::vector<std::vector<int>>& ranks, std::ofstream& fout);
void SavePath(const std::vector<Point>& path, std::ofstream& fout);