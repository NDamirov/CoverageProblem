#include <vector>
#include <string>
#include "or-tools/include/ortools/graph/assignment.h"

template<typename FieldElement>
class Field {
public:
    Field<int>(std::vector<std::string> in) {
        Resize(in.size(), in[0].size());
        for (int i = 0; i < in.size(); ++i) {
            for (int j = 0; j < in[i].size(); ++j) {
                if (in[i][j] == '*') {
                    field_[i][j] = 1;
                } else {
                    field_[i][j] = 0;
                }
            }
        }
    }

    std::vector<FieldElement>& operator[](int i) {
        return field_[i];
    }

    const std::vector<FieldElement>& operator[](int i) const {
        return field_[i];
    }

    void Resize(int w, int h) {
        width_ = w;
        height_ = h;
        field_.resize(h, std::vector<FieldElement>(w));
    }

    std::vector<std::vector<int>> CreateMILPH() {
        int n = width_ * height_;
        std::vector<std::vector<int>> coefs(n, std::vector<int>(n));

        for (int i = 0; i < width_; ++i) {
            for (int j = 0; j < height_; ++j) {
                int ind = i * width_ + j;
                int left = i * width_ + (j - 1);
                coefs[ind][ind] = 1;
                coefs[ind][left] = -1;
            }
        }

        return coefs;
    }

    std::vector<std::vector<int>> CreateMILPV() {
        int n = width_ * height_;
        std::vector<std::vector<int>> coefs(n, std::vector<int>(n));

        for (int i = 0; i < width_; ++i) {
            for (int j = 0; j < height_; ++j) {
                int ind = i * width_ + j;
                int up = (i - 1) * width_ + j;
                coefs[ind][ind] = 1;
                coefs[ind][up] = -1;
            }
        }

        return coefs;
    }

private:
    int width_;
    int height_;
    std::vector<std::vector<FieldElement>> field_;
};

#include <fstream>

Field<int> read_field(const std::string& file_name) {
    std::ifstream fin(file_name);
    std::string s;
    std::vector<std::string> input;
    while (std::getline(fin, s)) {
        if (s.empty()) {
            break;
        }
        input.push_back(s);
    }
    return Field<int>(input);
}

int main() { 
    Field<int> current = read_field("field.txt");
    return 0;
}





