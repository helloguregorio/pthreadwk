#include <iostream>
#include "../header/Graph.h"
#include "../header/Timer.h"

int main()
{
    const std::string file_name = "res/USA-road-d.NY.gr";
    std::ofstream test_data("./outcomes/test_data.txt",std::ios::app);
    std::streambuf *oldcout;
    oldcout = std::cout.rdbuf(test_data.rdbuf());
    Graph g(file_name);
    {
        Timer time("Algorithm");
        //g.dijkstra_serial_with_priority_queue(42);
        //g.dijkstra_serial(42);
        //g.dijkstra_parallel(42);
        g.dijkstra_parallel_static(42);
    }
    std::cout << std::endl;
    std::cout.rdbuf(oldcout);
    test_data.close();
}