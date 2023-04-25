#pragma once

#include <vector>
#include <queue>
#include "structures.hpp"

#include "ortools/constraint_solver/routing.h"
#include "ortools/constraint_solver/routing_enums.pb.h"
#include "ortools/constraint_solver/routing_index_manager.h"
#include "ortools/constraint_solver/routing_parameters.h"

using namespace operations_research;
using namespace std;

class PathGenerator {
public:
    PathGenerator(std::vector<std::vector<int>>&& field) : field_(std::move(field)) {}
    
    void MakePath();
    std::vector<Point> GetPath();

private:
    std::vector<std::vector<int>> field_;
    std::vector<Point> full_path_;

    float DistanceCounter(const Point& x, const Point& y);

    std::vector<Point> GenerateSolution(const RoutingIndexManager& manager, const RoutingModel& routing, 
        const Assignment& solution, const std::vector<Rank>& ranks);

    std::vector<Rank> BuildRanks();
    std::vector<std::vector<float>> BuildDistanceMatrix(const std::vector<Rank>& ranks);
    
    bool IsValidPoint(int x, int y);
    void MakeMiddleTrip(const Point& f, const Point& s);
    void Tsp(std::vector<std::vector<float>>& dists, const std::vector<Rank> ranks);
};