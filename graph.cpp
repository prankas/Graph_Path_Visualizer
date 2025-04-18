#include "Graph.h"
#include <queue>
#include <limits>
#include <algorithm>

void Graph::addNode(int id, const QPointF &pos)
{
    positions[id] = pos;
    adj[id];  // ensure entry exists
}

void Graph::addEdge(int from, int to, double weight)
{
    adj[from].push_back({to, weight});
    adj[to].push_back({from, weight});  // undirected
}

std::vector<int> Graph::shortestPath(int start, int end) const
{
    std::unordered_map<int, double> dist;
    std::unordered_map<int, int> prev;
    for (auto &p : positions)
        dist[p.first] = std::numeric_limits<double>::infinity();

    dist[start] = 0;
    using PD = std::pair<double,int>;
    std::priority_queue<PD, std::vector<PD>, std::greater<PD>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        if (u == end) break;
        for (auto &e : adj.at(u)) {
            double nd = d + e.weight;
            if (nd < dist[e.to]) {
                dist[e.to] = nd;
                prev[e.to] = u;
                pq.push({nd, e.to});
            }
        }
    }

    std::vector<int> path;
    if (start == end) { path.push_back(start); return path; }
    if (!prev.count(end)) return {};
    for (int at = end; at != start; at = prev[at])
        path.push_back(at);
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<std::vector<int>> Graph::connectedComponents() const
{
    std::unordered_map<int,bool> vis;
    for (auto &p : positions) vis[p.first] = false;

    std::vector<std::vector<int>> comps;
    for (auto &p : positions) {
        int u = p.first;
        if (vis[u]) continue;
        std::vector<int> comp;
        std::queue<int> q;
        q.push(u);
        vis[u] = true;
        while (!q.empty()) {
            int v = q.front(); q.pop();
            comp.push_back(v);
            for (auto &e : adj.at(v)) {
                if (!vis[e.to]) {
                    vis[e.to] = true;
                    q.push(e.to);
                }
            }
        }
        comps.push_back(comp);
    }
    return comps;
}

const std::unordered_map<int, QPointF>& Graph::nodePositions() const
{
    return positions;
}

const std::unordered_map<int, std::vector<Edge>>& Graph::adjacencyList() const
{
    return adj;
}
