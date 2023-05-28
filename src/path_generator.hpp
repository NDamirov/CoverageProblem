#pragma once

#include <queue>
#include <vector>
#include "structures.hpp"

#include "ortools/constraint_solver/routing.h"
#include "ortools/constraint_solver/routing_enums.pb.h"
#include "ortools/constraint_solver/routing_index_manager.h"
#include "ortools/constraint_solver/routing_parameters.h"

using namespace operations_research;

enum class DistanceType {
    AStar,     // slow
    Manhattan  // fast
};

// Class for building path on field with built ranks
class PathGenerator {
public:
    PathGenerator(std::vector<std::vector<int>>&& field,
                  DistanceType type = DistanceType::Manhattan)
        : type_(type), field_(std::move(field)) {}

    // Build path and save in class object
    void MakePath();

    // Returns path. Empty if MakePath was not called
    std::vector<Point> GetPath();

private:
    DistanceType type_;
    std::vector<std::vector<int>> field_;
    std::vector<Point> full_path_;

    float ManhattanDistanceCounter(const Point& x, const Point& y);
    float AStarDistanceCounter(const Point& x, const Point& y);

    std::vector<Point> GenerateSolution(const RoutingIndexManager& manager,
                                        const RoutingModel& routing,
                                        const Assignment& solution,
                                        const std::vector<Rank>& ranks);

    std::vector<Rank> BuildRanks();
    std::vector<std::vector<float>> BuildDistanceMatrix(
        const std::vector<Rank>& ranks);

    bool IsValidPoint(int x, int y);
    void MakeMiddleTrip(const Point& f, const Point& s);
    void Tsp(std::vector<std::vector<float>>& dists,
             const std::vector<Rank> ranks);
};