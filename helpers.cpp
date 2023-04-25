#include "helpers.hpp"

void SaveRanks(const std::vector<std::vector<int>>& ranks, std::ofstream& fout) {
    fout << ranks.size() << " " << ranks[0].size() << '\n';
    for (const std::vector<int>& x : ranks) {
        for (int y : x) {
            if (y == -1) {
                fout << "#";
            } else if (y == 0) {
                fout << "|";
            } else {
                fout << "-";
            }
        }
        fout << "\n";
    }
    fout.flush();
}

void SavePath(const std::vector<Point>& path, std::ofstream& fout) {
    fout << path.size() << '\n';
    for (const auto& point : path) {
        fout << point << '\n';
    }
    fout.flush();
}