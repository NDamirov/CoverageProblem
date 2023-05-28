#include "field.hpp"

#include "ortools/linear_solver/linear_solver.h"
using namespace operations_research;

Field::Field(std::vector<std::string> in) {
    Resize(in[0].size(), in.size());
    spaces_ = 0;
    for (int i = 0; i < in.size(); ++i) {
        for (int j = 0; j < in[i].size(); ++j) {
            if (in[i][j] == '*') {
                field_[i][j] = 1;
                ++spaces_;
            } else {
                field_[i][j] = -1;
            }
        }
    }
}

std::vector<Field::FieldElement>& Field::operator[](int i) {
    return field_[i];
}

const std::vector<Field::FieldElement>& Field::operator[](int i) const {
    return field_[i];
}

void Field::Resize(int w, int h) {
    width_ = w;
    height_ = h;
    field_.resize(h, std::vector<FieldElement>(w));
}

std::vector<std::vector<Field::FieldElement>> Field::Solve() {
    std::vector<double> obj_coefs(3 * spaces_, 0);
    for (int i = 0; i < 2 * spaces_; ++i) {
        obj_coefs[i] = -1;
    }

    std::vector<double> bounds(2 * spaces_, 0);
    std::vector<std::vector<double>> coefs(2 * spaces_,
                                           std::vector<double>(3 * spaces_, 0));

    int ind = 0;
    int c_ind = 0;
    LOG(INFO) << "Building coefs";
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; ++j) {
            if (field_[i][j] == -1) {
                continue;
            }
            field_[i][j] = ind;
            //------------
            coefs[c_ind][ind] = -1;                     // y_h[i]
            coefs[c_ind + 1][spaces_ + ind] = -1;       // y_v[i]
            coefs[c_ind][spaces_ + spaces_ + ind] = 1;  // x_h[i]
            if (!(j == 0 || field_[i][j - 1] == -1)) {
                coefs[c_ind][spaces_ + spaces_ + ind - 1] = -1;  // x_h[left(i)]
            }
            //------------
            coefs[c_ind + 1][spaces_ + spaces_ + ind] = -1;  // x_h[i]
            bounds[c_ind + 1] = -1;
            if (!(i == 0 || field_[i - 1][j] == -1)) {
                bounds[c_ind + 1] = 0;
                coefs[c_ind + 1][spaces_ + spaces_ + field_[i - 1][j]] = 1;
            }
            ++ind;
            c_ind += 2;
        }
    }

    LOG(INFO) << "Starting solver";
    std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
    if (!solver) {
        LOG(ERROR) << "SCIP solver unavailable.";
        return {};
    }

    std::vector<const MPVariable*> x(3 * spaces_);
    for (int j = 0; j < 3 * spaces_; ++j) {
        x[j] = solver->MakeIntVar(0.0, 1.0, "");
    }

    for (int i = 0; i < 2 * spaces_; ++i) {
        MPConstraint* constraint =
            solver->MakeRowConstraint(-solver->infinity(), bounds[i], "");
        for (int j = 0; j < 3 * spaces_; ++j) {
            constraint->SetCoefficient(x[j], coefs[i][j]);
        }
    }

    MPObjective* const objective = solver->MutableObjective();
    for (int j = 0; j < 3 * spaces_; ++j) {
        objective->SetCoefficient(x[j], obj_coefs[j]);
    }
    objective->SetMaximization();

    const MPSolver::ResultStatus result_status = solver->Solve();

    ind = 2 * spaces_;
    for (int i = 0; i < height_; ++i) {
        for (int j = 0; j < width_; ++j) {
            if (field_[i][j] == -1) {
                continue;
            }
            field_[i][j] = x[ind++]->solution_value();
        }
    }
    return field_;
}
