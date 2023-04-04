#include <ostream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "ortools/linear_solver/linear_solver.h"
#include "ortools/constraint_solver/routing.h"
#include "ortools/constraint_solver/routing_enums.pb.h"
#include "ortools/constraint_solver/routing_index_manager.h"
#include "ortools/constraint_solver/routing_parameters.h"

using namespace operations_research;
using namespace std;

struct Point {
    int x;
    int y;
};

struct Rank {
    Point f;
    Point s;
};

std::ostream& operator<<(std::ostream& stream, const Point& x) {
    stream << x.x << " " << x.y;
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Rank& x) {
    stream << x.f << " " << x.s;
    return stream;
}

void PrintSolution(const RoutingIndexManager& manager, const RoutingModel& routing, 
    const Assignment& solution, const std::vector<Rank>& ranks) {
    int64_t index = routing.Start(0);
    stringstream result;
    int size = 0;
    vector<bool> used_ranks(ranks.size(), false);
    vector<int> route;
    route.reserve(2 * ranks.size());
    while (routing.IsEnd(index) == false) {
        int idx = manager.IndexToNode(index).value();
        route.push_back(idx);
        index = solution.Value(routing.NextVar(index));
    }
    if (route[1] != 1) {
        route.insert(route.begin(), route.back());
        route.pop_back();
    }
    for (int idx : route) {
        LOG(INFO) << idx << " -> ";
        if (!used_ranks[idx / 2]) {
            used_ranks[idx / 2] = true;
            Rank curr = ranks[idx / 2];
            if (!(idx & 1)) {
                swap(curr.f, curr.s);
            }
            result << curr << '\n';
            ++size;
        }
    }
    cout << size << '\n';
    cout << result.str();
    cout.flush();
}

template<typename FieldElement>
class Field {
public:
    Field<int>(std::vector<std::string> in) {
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

    std::vector<std::vector<FieldElement>> Solve() {
        std::vector<double> obj_coefs(3 * spaces_, 0);
        for (int i = 0; i < 2 * spaces_; ++i) {
            obj_coefs[i] = -1;
        }

        std::vector<double> bounds(2 * spaces_, 0);
        std::vector<std::vector<double>> coefs(2 * spaces_, std::vector<double>(3 * spaces_, 0));

        int ind = 0;
        int c_ind = 0;
        LOG(INFO) << "Building coefs";
        for (int i = 0; i < height_; i++) {
            for (int j = 0; j < width_; ++j){ 
                if (field_[i][j] == -1) {
                    continue;
                }
                field_[i][j] = ind;
                //------------
                coefs[c_ind][ind] = coefs[c_ind + 1][spaces_ + ind] = -1; // y_h[i], y_v[i]
                coefs[c_ind][spaces_ + spaces_ + ind] = 1; // x_h[i]
                if (!(j == 0 || field_[i][j - 1] == -1)) {
                    coefs[c_ind][spaces_ + spaces_ + ind - 1] = -1; // x_h[left(i)]
                }
                //------------
                coefs[c_ind + 1][spaces_ + spaces_ + ind] = -1; // x_h[i]
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
            MPConstraint* constraint = solver->MakeRowConstraint(-solver->infinity(), bounds[i], "");
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

private:
    int width_;
    int height_;
    int spaces_;
    std::vector<std::vector<FieldElement>> field_;
    std::vector<std::vector<FieldElement>> sol_;
};

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

class PathGenerator {
public:
    PathGenerator(std::vector<std::vector<int>>&& field) : field_(std::move(field)) {}
    
    void MakePath() {
        std::vector<Rank> ranks = BuildRanks();
        std::vector<std::vector<float>> coefs = BuildDistanceMatrix(ranks);
        LOG(INFO) << "Starting TSP";
        Tsp(coefs, ranks);
    }

private:
    std::vector<std::vector<int>> field_;

    float DistanceCounter(const Point& x, const Point& y) {
        return abs(x.x - y.x) + abs(x.y - y.y);
    }

    std::vector<Rank> BuildRanks() {
        std::vector<Rank> ranks;
        int height = field_.size();
        int width = field_[0].size();
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (field_[i][j] == 1 && (j == 0 || field_[i][j - 1] != 1)) { // horizontal
                    int length = 1;
                    while (j + length < width && field_[i][j + length] == 1) {
                        length++;
                    }
                    ranks.push_back({{i, j}, {i, j + length - 1}});
                } else if (field_[i][j] == 0 && (i == 0 || field_[i - 1][j] != 0)) {
                    int length = 1;
                    while (i + length < height && field_[i + length][j] == 0) {
                        length++;
                    }
                    ranks.push_back({{i, j}, {i + length - 1, j}});
                }
            }
        }
        return ranks;
    }

    std::vector<std::vector<float>> BuildDistanceMatrix(const std::vector<Rank>& ranks) {
        std::vector<std::vector<float>> result(2 * ranks.size(), std::vector<float>(2 * ranks.size(), 0));
        LOG(INFO) << ranks[0];
        LOG(INFO) << ranks[1];
        LOG(INFO) << ranks[2];
        LOG(INFO) << ranks[3];
        for (int i = 0; i < ranks.size(); ++i) {
            for (int j = 0; j < ranks.size(); ++j) {
                if (i == j) {
                    continue;
                }
                result[i + i + 1][j + j] = DistanceCounter(ranks[i].f, ranks[j].s);
                result[i + i + 1][j + j + 1] = DistanceCounter(ranks[i].f, ranks[j].f);
                result[i + i][j + j] = DistanceCounter(ranks[i].s, ranks[j].s);
                result[i + i][j + j + 1] = DistanceCounter(ranks[i].s, ranks[j].f);
            }
        }

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                LOG(INFO) << i << " " << j << " " << result[i][j];
            }
        }
        return result;
    }

    void Tsp(std::vector<std::vector<float>>& dists, const std::vector<Rank> ranks) {
        const RoutingIndexManager::NodeIndex depot{0};
        RoutingIndexManager manager(dists.size(), 1, depot);

        RoutingModel routing(manager);

        const int transit_callback_index = routing.RegisterTransitCallback(
            [&dists, &manager](int64_t from_index,
                                        int64_t to_index) -> int64_t {
                // Convert from routing variable Index to distance matrix NodeIndex.
                auto from_node = manager.IndexToNode(from_index).value();
                auto to_node = manager.IndexToNode(to_index).value();
                return dists[from_node][to_node];
            });

        routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index);
        RoutingSearchParameters searchParameters = DefaultRoutingSearchParameters();
        searchParameters.set_first_solution_strategy(
            FirstSolutionStrategy::PATH_CHEAPEST_ARC);

        const Assignment* solution = routing.SolveWithParameters(searchParameters);
        PrintSolution(manager, routing, *solution, ranks);
    }
};

int main() { 
    Field<int> current = read_field("field.txt");
    std::vector<std::vector<int>> result = current.Solve();
    // -------------- OUTPUT START --------------
    std::cout << result.size() << " " << result[0].size() << '\n';
    for (const std::vector<int>& x : result) {
        for (int y : x) {
            if (y == -1) {
                cout << "#";
            } else if (y == 0) {
                cout << "|";
            } else {
                cout << "-";
            }
        }
        cout << "\n";
    }
    cout.flush();
    // -------------- OUTPUT FINISH --------------
    PathGenerator path_generator(std::move(result));
    path_generator.MakePath();
    return 0;
}
