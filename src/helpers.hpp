#pragma once

#include <fstream>
#include <vector>
#include "field.hpp"
#include "structures.hpp"

// Reads field from input file
// Returns constructed Field object
Field read_field(const std::string& file_name);

// Saves ranks to output file
void SaveRanks(const std::vector<std::vector<int>>& ranks, std::ofstream& fout);

// Saves path to output file
void SavePath(const std::vector<Point>& path, std::ofstream& fout);