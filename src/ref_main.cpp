#include <iostream>

#include <set>
#include "driver.hpp"

int main()
{
    try
    {
        Driver::driver<std::set<int>, int>(std::cin, std::cout);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    return 0;
}