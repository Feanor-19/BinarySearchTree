#include <iostream>

#include "BST.hpp"
#include "driver.hpp"

int main()
{
    try
    {
        Driver::driver<BST<int>, int>(std::cin, std::cout);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    return 0;
}