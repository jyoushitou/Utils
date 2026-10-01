#include "Utils.h"
#include <iostream>

/// @brief 程序的启动函数
/// @return 返回程序的执行码
int main()
{

    Utils::serviceID.store(ServiceID::Test);


    std::cout << "===================" << std::endl;
    std::cout << "This test of Utils" << std::endl;
    std::cout << "===================" << std::endl;

    std::cout << "===================" << std::endl;
    std::cout << "Utils over" << std::endl;
    std::cout << "===================" << std::endl;
    return 0;
}