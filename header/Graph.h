#pragma once
#include <vector>
#include <iostream>
#include <limits>
#include <string>
#include <queue>
#include <algorithm>
#include <fstream>
#include <pthread.h>
#include <cmath>
/**
 * @brief 这个头文件定义了图相关的数据结构
 * 为了满足大规模数据，算法所用数据结构将
 * 用邻接数组实现。
 * @attention 这个图是有向图
 */

/**
 * @brief 定义一些会用到的参数
 *
 */
namespace CONFIG
{
    const int MAX_VERTEX_NUM = 58333344;                 //点的最大个数
    const int INF = std::numeric_limits<int>::max(); //定义无法到达的边
} // namespace CONFIG

//边数据结构
struct edge
{
    int to;   //有向边的终点
    int cost; //这条边的开销
};
//临时节点
struct node
{
    int distance, u;
    friend bool operator>(const node &a,const node &b)
    {
        return a.distance > b.distance;
    }
};

class Graph
{
    std::vector<edge> *G;   //邻接数组
    int vertex_num;                                //图的点数
    int edge_num;                                  //图的边数
    int *shortest_distance; //保存算法中计算出来的最短距离
    bool *used;             //检查该点是否判断过
public:
    Graph(int v, int e) : vertex_num(v), edge_num(e)
    {
        G=new std::vector<edge>[CONFIG::MAX_VERTEX_NUM];
        shortest_distance=new int[CONFIG::MAX_VERTEX_NUM];
        used=new bool[CONFIG::MAX_VERTEX_NUM];
    }
    Graph(const std::string &file_name){
        G=new std::vector<edge>[CONFIG::MAX_VERTEX_NUM];
        shortest_distance=new int[CONFIG::MAX_VERTEX_NUM];
        used=new bool[CONFIG::MAX_VERTEX_NUM];
        get_data(file_name);
    }
    ~Graph(){
        delete[] G;
        delete[] shortest_distance;
        delete[] used;
    }
    void get_data(const std::string &file_name);
    void create()
    {
        /**
         * @brief 录入图的数据，构建该图
         *
         */
        std::cin >> vertex_num >> edge_num;
        for (int i = 0; i < edge_num; i++)
        {
            int start;
            edge e;
            std::cin >> start;
            std::cin >> e.to >> e.cost;
            G[start].push_back(e);
        }
    }
    void dijkstra_serial(int source);

    void dijkstra_serial_with_priority_queue(int source);

    void dijkstra_parallel(int source);
};
