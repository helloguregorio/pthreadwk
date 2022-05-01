#pragma once
#include <chrono>
#include <iostream>
#include <string>
/**
 * @brief 一个计时器，用来在linux和windows算一下高斯消去法的时间
 *
 */
class Timer
{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;  //计时起点
    std::chrono::time_point<std::chrono::high_resolution_clock> finish; //计时终点
    std::string info;

public:
    Timer(const std::string &info) : info(info)
    {
        start = std::chrono::high_resolution_clock::now();
    }
    ~Timer()
    {
        finish = std::chrono::high_resolution_clock::now();
        using namespace std::literals;
        auto use_time = finish - start;
        std::cout << info << " time use " << use_time / 1ms << "ms." << std::endl;
    }
};