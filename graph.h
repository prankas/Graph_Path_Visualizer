#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <QPointF>

struct Edge { int to; double weight; };

class Graph
{
public:
    void addNode(int id, const QPointF &pos);
    void addEdge(int from, int to, double weight = 1.0);

    std::vector<int> shortestPath(int start, int end) const;
    std::vector<std::vector<int>> connectedComponents() const;

    const std::unordered_map<int, QPointF>& nodePositions() const;
    const std::unordered_map<int, std::vector<Edge>>& adjacencyList() const;

private:
    std::unordered_map<int, QPointF> positions;
    std::unordered_map<int, std::vector<Edge>> adj;
};

#endif
