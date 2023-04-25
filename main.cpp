#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>

#include "helpers.hpp"
#include "structures.hpp"
#include "field.hpp"
#include "path_generator.hpp"
#include "helpers.hpp"

int main(int argc, char **argv) { 
    if (argc < 3) {
        std::cout << "Invalid input: ./program <in file> <out file> <optional args>";
        return 1;
    }

    std::unordered_set<std::string> arguments;
    for (int i = 3; i < argc; ++i) {
        arguments.insert(std::string(argv[i]));
    }

    std::ofstream result_file(argv[2]);
    Field current = read_field(argv[1]);
    std::vector<std::vector<int>> result = current.Solve();
    SaveRanks(result, result_file);

    if (arguments.find("-ronly") != arguments.end() ||
        arguments.find("--ranks_only") != arguments.end()) {
        result_file.close();
        return 0;
    }

    PathGenerator path_generator(std::move(result));
    path_generator.MakePath();
    std::vector<Point> path = path_generator.GetPath();
    SavePath(path, result_file);
    result_file.close();
    return 0;
}
