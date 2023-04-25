#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <sstream>

#include "helpers.hpp"
#include "structures.hpp"
#include "field.hpp"
#include "path_generator.hpp"
#include "helpers.hpp"

using namespace std;

int main(int argc, char **argv) { 
    std::ofstream result_file(argv[2]);
    Field current = read_field(argv[1]);
    std::vector<std::vector<int>> result = current.Solve();
    SaveRanks(result, result_file);

    PathGenerator path_generator(std::move(result));
    path_generator.MakePath();
    std::vector<Point> path = path_generator.GetPath();
    SavePath(path, result_file);
    result_file.close();
    return 0;
}
