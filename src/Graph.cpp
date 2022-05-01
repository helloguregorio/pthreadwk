#include "../header/Graph.h"
#include "../header/Timer.h"
void Graph::get_data(const std::string &file_name)
{
    Timer time("reading file "+file_name);
    std::fstream fin(file_name);
    char ch;
    int start, end, cost; //分别是起点，终点和开销
    std::string tmp_str;
    for (int i = 0; i < 4; i++)
    {
        getline(fin, tmp_str);
    }
    fin >> tmp_str;
    fin >> tmp_str;
    fin >> vertex_num >> edge_num; //得到点数
    std::cout<<"The graph has "<<vertex_num<<" vertices and "<<edge_num<<"edges"<<std::endl;
    getline(fin, tmp_str);
    getline(fin, tmp_str);
    getline(fin, tmp_str);
    for (int i = 0; i < edge_num; i++)
    {
        fin >> ch >> start >> end >> cost;
        G[start - 1].push_back({end - 1, cost});
    }
    fin.close();
}

void Graph::dijkstra_serial(int source)
{
    //完成dijkstra算法的初始化
    std::fill(shortest_distance, shortest_distance + vertex_num, CONFIG::INF);
    std::fill(used, used + vertex_num, false);
    shortest_distance[source] = 0;
    while (true)
    {
        int v = -1;
        for (int i = 0; i < vertex_num; i++)
        {
            //该循环目的是寻找最小距离
            if (!used[i] && (v == -1 || shortest_distance[i] < shortest_distance[v]))
                v = i;
        }
        if (v == -1)
            break;
        used[v] = true;
        for (size_t i = 0; i < G[v].size(); i++)
        {
            edge e = G[v][i];
            if (shortest_distance[v] != CONFIG::INF)
                shortest_distance[e.to] = std::min(shortest_distance[e.to], shortest_distance[v] + e.cost);
        }
    }
}

void Graph::dijkstra_serial_with_priority_queue(int source)
{
    //完成dijkstra算法的初始化
    std::fill(shortest_distance, shortest_distance + vertex_num, CONFIG::INF);
    std::fill(used, used + vertex_num, false);
    shortest_distance[source] = 0;
    std::priority_queue<node, std::vector<node>, std::greater<node>> heap;
    heap.push({0, source}); //塞入源点
    while (!heap.empty())
    {
        int u = heap.top().u;
        heap.pop();
        if (used[u])
            continue;
        used[u] = true;
        for (auto &ed : G[u])
        {
            auto tmp_cost = shortest_distance[u] + ed.cost;
            if (shortest_distance[ed.to] > tmp_cost)
            {
                shortest_distance[ed.to] = tmp_cost;
                heap.push({shortest_distance[ed.to], ed.to});
            }
        }
    }
}