#include "../header/Graph.h"
#include "../header/Timer.h"
void Graph::get_data(const std::string &file_name)
{
    Timer time("reading file " + file_name);
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
    std::cout << "The graph has " << vertex_num << " vertices and " << edge_num << " edges." << std::endl;
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

void *threadFunc(void *param)
{
    thread_param_t *p = reinterpret_cast<thread_param_t *>(param);
    auto dis = p->dis;
    auto used = p->used;
    int start = p->start;
    int end = p->end;
    int smaller = -1;
    for (int i = start; i < end; i++)
    {
        if (!used[i] && (smaller == -1 || dis[i] < dis[smaller]))
            smaller = i;
    }
    p->smallest = smaller;
    return nullptr;
}
sem_t sem_main;
sem_t sem_work_start[CONFIG::THREAD_COUNT];
sem_t sem_work_end[CONFIG::THREAD_COUNT];
void *threadFunc_static(void *param)
{
    thread_param_t *p = reinterpret_cast<thread_param_t *>(param);
    auto dis = p->dis;
    auto used = p->used;
    int id = p->id;
    int start = p->start;
    int end = p->end;
    int smaller = -1;
    for (int i = 0; i < 264346; i++)
    {
        sem_wait(&sem_work_start[id]);
        for (int i = start; i < end; i++)
        {
            if (!used[i] && (smaller == -1 || dis[i] < dis[smaller]))
                smaller = i;
        }
        p->smallest = smaller;
        sem_post(&sem_main);
        sem_wait(&sem_work_end[id]);
    }
    pthread_exit(NULL);
}

void Graph::dijkstra_parallel(int source)
{
    //完成dijkstra算法的初始化
    std::fill(shortest_distance, shortest_distance + vertex_num, CONFIG::INF);
    std::fill(used, used + vertex_num, false);
    shortest_distance[source] = 0;
    while (true)
    {
        int v;
        int tmp_shortest = CONFIG::MAX_VERTEX_NUM;
        pthread_t threads[CONFIG::THREAD_COUNT];
        thread_param_t thread_arr[CONFIG::THREAD_COUNT]{};
        for (int i = 0; i < CONFIG::THREAD_COUNT; i++)
        {
            thread_arr[i].dis = shortest_distance;
            thread_arr[i].used = used;
            thread_arr[i].start = i * (vertex_num / CONFIG::THREAD_COUNT);
            if (i == CONFIG::THREAD_COUNT - 1)
            {
                thread_arr[i].end = vertex_num;
            }
            else
            {
                thread_arr[i].end = (i + 1) * (vertex_num / CONFIG::THREAD_COUNT);
            }
            pthread_create(&threads[i], NULL, threadFunc, reinterpret_cast<void *>(&thread_arr[i]));
        }
        for (int i = 0; i < CONFIG::THREAD_COUNT; i++)
        {
            pthread_join(threads[i], NULL);
        }
        for (int i = 0; i < CONFIG::THREAD_COUNT; i++)
        {
            if (!used[thread_arr[i].smallest] &&
                (thread_arr[i].smallest == -1 || shortest_distance[thread_arr[i].smallest] < tmp_shortest))
            {
                v = thread_arr[i].smallest;
                tmp_shortest = shortest_distance[thread_arr[i].smallest];
            }
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
void Graph::dijkstra_parallel_static(int source)
{
    std::fill(shortest_distance, shortest_distance + vertex_num, CONFIG::INF);
    std::fill(used, used + vertex_num, false);
    shortest_distance[source] = 0;
    sem_init(&sem_main, 0, 0);
    int v;
    int tmp_shortest = CONFIG::MAX_VERTEX_NUM;
    for (int i = 0; i < CONFIG::THREAD_COUNT; i++)
    {
        sem_init(&sem_work_start[i], 0, 0);
        sem_init(&sem_work_end[i], 0, 0);
    }
    pthread_t threads[CONFIG::THREAD_COUNT];
    thread_param_t thread_arr[CONFIG::THREAD_COUNT]{};
    for (int i = 0; i < CONFIG::THREAD_COUNT; i++)
    {
        thread_arr[i].dis = shortest_distance;
        thread_arr[i].used = used;
        thread_arr[i].start = i * (vertex_num / CONFIG::THREAD_COUNT);
        if (i == CONFIG::THREAD_COUNT - 1)
        {
            thread_arr[i].end = vertex_num;
        }
        else
        {
            thread_arr[i].end = (i + 1) * (vertex_num / CONFIG::THREAD_COUNT);
        }
        pthread_create(&threads[i], NULL, threadFunc_static, reinterpret_cast<void *>(&thread_arr[i]));
    }
    while (true)
    {
        for (int i = 0; i < CONFIG::THREAD_COUNT; i++)
        {
            sem_post(&sem_work_start[i]);
        }
        int v = -1;
        for (int i = 0; i < vertex_num; i++)
        {
            //该循环目的是寻找最小距离
            if (!used[i] && (v == -1 || shortest_distance[i] < shortest_distance[v]))
                v = i;
        }
        for (int i = 0; i < CONFIG::THREAD_COUNT; i++)
        {
            sem_wait(&sem_main);
        }
        for (int i = 0; i < CONFIG::THREAD_COUNT; i++)
        {
            sem_post(&sem_work_end[i]);
        }
        for (int i = 0; i < CONFIG::THREAD_COUNT; i++)
        {
            if (!used[thread_arr[i].smallest] &&
                (thread_arr[i].smallest == -1 || shortest_distance[thread_arr[i].smallest] < tmp_shortest))
            {
                v = thread_arr[i].smallest;
                tmp_shortest = shortest_distance[thread_arr[i].smallest];
            }
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
    for(int i=0;i<CONFIG::THREAD_COUNT;i++){
        pthread_join(threads[i],NULL);
    }
}