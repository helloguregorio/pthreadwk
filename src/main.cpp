#include <iostream>
#include "../header/Graph.h"
#include "../header/Timer.h"

int main()
{
    const std::string file_name = "USA-road-d.NY.gr";
    std::ofstream test_data("./outcomes/test_data.txt");
    std::streambuf *oldcout;
    oldcout = std::cout.rdbuf(test_data.rdbuf());
    Graph g("res/" + file_name);
    {
        Timer time("Algorithm");
        g.dijkstra_serial_with_priority_queue(42);
    }
    std::cout.rdbuf(oldcout);
    test_data.close();
    std::cout << std::endl;
}