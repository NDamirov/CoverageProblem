#include "path_generator.hpp"

void PathGenerator::MakePath() {
    std::vector<Rank> ranks = BuildRanks();
    std::vector<std::vector<float>> coefs = BuildDistanceMatrix(ranks);
    LOG(INFO) << "Starting TSP";
    Tsp(coefs, ranks);
}

std::vector<Point> PathGenerator::GetPath() {
    return full_path_;
}
float PathGenerator::ManhattanDistanceCounter(const Point& x, const Point& y) {
    return abs(x.x - y.x) + abs(x.y - y.y);
}

float PathGenerator::AStarDistanceCounter(const Point& f, const Point& s) {
    if (f.x == s.x && f.y == s.y) {
        return 0;
    }
    int w = field_[0].size();
    std::vector<std::vector<double>> dist(
        field_.size(), std::vector<double>(field_[0].size(), 1e9));
    s td::queue<int> q;

    dist[s.x][s.y] = 0;
    q.push(w * s.x + s.y);
    while (dist[f.x][f.y] > 1000000. && !q.empty()) {
        int last = q.front();
        q.pop();
        int x = last / w;
        int y = last - x * w;
        if (IsValidPoint(x - 1, y) && dist[x - 1][y] > dist[x][y] + 1) {
            dist[x - 1][y] = dist[x][y] + 1;
            q.push(w * (x - 1) + y);
        }
        if (IsValidPoint(x + 1, y) && dist[x + 1][y] > dist[x][y] + 1) {
            dist[x + 1][y] = dist[x][y] + 1;
            q.push(w * (x + 1) + y);
        }
        if (IsValidPoint(x, y - 1) && dist[x][y - 1] > dist[x][y] + 1) {
            dist[x][y - 1] = dist[x][y] + 1;
            q.push(w * x + (y - 1));
        }
        if (IsValidPoint(x, y + 1) && dist[x][y + 1] > dist[x][y] + 1) {
            dist[x][y + 1] = dist[x][y] + 1;
            q.push(w * x + (y + 1));
        }
    }
    LOG(INFO) << "Distance is: " << dist[f.x][f.y];
    return dist[f.x][f.y];
}

std::vector<Point> PathGenerator::GenerateSolution(
    const RoutingIndexManager& manager, const RoutingModel& routing,
    const Assignment& solution, const std::vector<Rank>& ranks) {
    int64_t index = routing.Start(0);

    std::vector<bool> used_ranks(ranks.size(), false);
    std::vector<int> route;
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
    std::vector<Point> result;
    for (int idx : route) {
        if (!used_ranks[idx / 2]) {
            used_ranks[idx / 2] = true;
            Rank curr = ranks[idx / 2];
            if (!(idx & 1)) {
                std::swap(curr.f, curr.s);
            }
            result.push_back(curr.f);
            result.push_back(curr.s);
        }
    }
    return result;
}

std::vector<Rank> PathGenerator::BuildRanks() {
    std::vector<Rank> ranks;
    int height = field_.size();
    int width = field_[0].size();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (field_[i][j] == 1 &&
                (j == 0 || field_[i][j - 1] != 1)) {  // horizontal
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

std::vector<std::vector<float>> PathGenerator::BuildDistanceMatrix(
    const std::vector<Rank>& ranks) {
    std::vector<std::vector<float>> result(
        2 * ranks.size(), std::vector<float>(2 * ranks.size(), 0));
    for (int i = 0; i < ranks.size(); ++i) {
        for (int j = 0; j < ranks.size(); ++j) {
            if (i == j) {
                continue;
            }
            if (type_ == DistanceType::Manhattan) {
                result[i + i + 1][j + j] =
                    ManhattanDistanceCounter(ranks[i].f, ranks[j].s);
                result[i + i + 1][j + j + 1] =
                    ManhattanDistanceCounter(ranks[i].f, ranks[j].f);
                result[i + i][j + j] =
                    ManhattanDistanceCounter(ranks[i].s, ranks[j].s);
                result[i + i][j + j + 1] =
                    ManhattanDistanceCounter(ranks[i].s, ranks[j].f);
            } else {
                result[i + i + 1][j + j] =
                    AStarDistanceCounter(ranks[i].f, ranks[j].s);
                result[i + i + 1][j + j + 1] =
                    AStarDistanceCounter(ranks[i].f, ranks[j].f);
                result[i + i][j + j] =
                    AStarDistanceCounter(ranks[i].s, ranks[j].s);
                result[i + i][j + j + 1] =
                    AStarDistanceCounter(ranks[i].s, ranks[j].f);
            }
        }
    }
    return result;
}

bool PathGenerator::IsValidPoint(int x, int y) {
    return (x >= 0) && (y >= 0) && (x < field_.size()) &&
           (y < field_[0].size()) && (field_[x][y] != -1);
}

void PathGenerator::MakeMiddleTrip(const Point& f, const Point& s) {
    if (f.x == s.x && f.y == s.y) {
        return;
    }
    int w = field_[0].size();
    static std::vector<std::vector<int>> prev(
        field_.size(), std::vector<int>(field_[0].size(), 1e9));
    std::vector<std::vector<char>> dist(field_.size(),
                                        std::vector<char>(field_[0].size(), 1));

    std::queue<int> q;
    prev[s.x][s.y] = 0;
    q.push(w * s.x + s.y);
    while (dist[f.x][f.y] == 1 && !q.empty()) {
        int last = q.front();
        q.pop();
        int x = last / w;
        int y = last - x * w;
        if (IsValidPoint(x - 1, y) && dist[x - 1][y]) {
            dist[x - 1][y] = 0;
            prev[x - 1][y] = w * x + y;
            q.push(w * (x - 1) + y);
        }
        if (IsValidPoint(x + 1, y) && dist[x + 1][y]) {
            dist[x + 1][y] = 0;
            prev[x + 1][y] = w * x + y;
            q.push(w * (x + 1) + y);
        }
        if (IsValidPoint(x, y - 1) && dist[x][y - 1]) {
            dist[x][y - 1] = 0;
            prev[x][y - 1] = w * x + y;
            q.push(w * x + (y - 1));
        }
        if (IsValidPoint(x, y + 1) && dist[x][y + 1]) {
            dist[x][y + 1] = 0;
            prev[x][y + 1] = w * x + y;
            q.push(w * x + (y + 1));
        }
    }
    int tx = f.x;
    int ty = f.y;
    while (tx != s.x || ty != s.y) {
        int curr = prev[tx][ty];
        tx = curr / w;
        ty = curr - tx * w;
        full_path_.push_back({tx, ty});
    }
    full_path_.push_back(s);
}

void PathGenerator::Tsp(std::vector<std::vector<float>>& dists,
                        const std::vector<Rank> ranks) {
    const RoutingIndexManager::NodeIndex depot{0};
    RoutingIndexManager manager(dists.size(), 1, depot);

    RoutingModel routing(manager);

    const int transit_callback_index = routing.RegisterTransitCallback(
        [&dists, &manager](int64_t from_index, int64_t to_index) -> int64_t {
            auto from_node = manager.IndexToNode(from_index).value();
            auto to_node = manager.IndexToNode(to_index).value();
            return dists[from_node][to_node];
        });

    routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index);
    RoutingSearchParameters searchParameters = DefaultRoutingSearchParameters();
    searchParameters.set_first_solution_strategy(
        FirstSolutionStrategy::PATH_CHEAPEST_ARC);

    const Assignment* solution = routing.SolveWithParameters(searchParameters);
    std::vector<Point> rank_solution =
        GenerateSolution(manager, routing, *solution, ranks);
    full_path_ = {rank_solution[0]};
    for (int i = 1; i < rank_solution.size(); ++i) {
        MakeMiddleTrip(rank_solution[i - 1], rank_solution[i]);
    }
}