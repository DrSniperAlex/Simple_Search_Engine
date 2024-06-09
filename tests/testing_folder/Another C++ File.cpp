// #A
#include <iostream>
#include <vector>
#include <set>

void DFS(int v, std::vector<std::vector<int>>& graph, std::vector<int>& time_in, std::vector<int>& parent, std::vector<bool>& visited, std::set<int>& tochki_SCH, int timer, int p) {
    time_in[v] = timer++;
    parent[v] = timer++;
    visited[v] = true;
    int children = 0;

    for (auto u : graph[v]) {
        if (u == p) continue;
        
        if (visited[u]) {
            parent[v] = std::min(parent[v], time_in[u]);
        } else {
            DFS(u, graph, time_in, parent, visited, tochki_SCH, timer, v);
            parent[v] = std::min(parent[v], parent[u]);

            if ((parent[u] >= time_in[v]) && (p != -1)) {
                tochki_SCH.insert(v + 1);
            }
            ++children;
        }
    }

    if ((p == -1) && (children > 1)) {
        tochki_SCH.insert(v + 1);
    }
}

int main() {
    int n, m;
    std::cin >> n >> m;

    std::vector<std::vector<int>> graph(n);
    std::vector<int> time_in(n), parent(n);
    std::set<int> tochki_SCH;
    std::vector<bool> visited(n);
    int timer = 0;

    int start, end;
    for (int i = 0; i < m; i++) {
        std::cin >> start >> end;
        graph[end - 1].push_back(start - 1);
        graph[start - 1].push_back(end - 1);
    }

    int p = -1;
    for (int i = 0; i < n; i++)
        if (!visited[i])
            DFS(i, graph, time_in, parent, visited, tochki_SCH, timer, p);

    if (tochki_SCH.size() != 0) {
        for (int tochka : tochki_SCH) {
            std::cout << tochka << " ";
        }
    } else {
        std::cout << "-1";
    }
    
    return 0;
}