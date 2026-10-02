#include "Utils.h"
#include <iostream>
#include <atomic>

std::atomic<ServiceID> Utils::serviceID{ServiceID::Test};

void testTime()
{
    std::cout << "===================" << std::endl;
    std::cout << "This test of time in Utils" << std::endl;
    std::cout << "===================" << std::endl;

    std::cout <<std::endl<< "now time is " << Utils::Time::nowTime() << std::endl<<std::endl;

    time_t nowTime = Utils::Time::nowTime();

    //std::cout<<

    std::cout << "===================" << std::endl;
    std::cout << "time in Utils over" << std::endl;
    std::cout << "===================" << std::endl;
}

/// @brief 程序的启动函数
/// @return 返回程序的执行码
int main()
{

    std::cout << "===================" << std::endl;
    std::cout << "This test of Utils" << std::endl;
    std::cout << "===================" << std::endl;
    std::cout << "Go to you test?" << std::endl;
    while (1)
    {
        std::cout << "0.close Test" << std::endl;
        std::cout << "1.Test time" << std::endl;
        int key = 0;
        std::cin >> key;
        switch (key)
        {
        case 0:
        {
            goto close;
        }
        case 1:
            testTime();
            break;
        }
    }
close:
    std::cout << "===================" << std::endl;
    std::cout << "Utils test over" << std::endl;
    std::cout << "===================" << std::endl;
    return 0;
}