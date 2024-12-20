#include <iostream>

#include "BST.hpp"

#include <set>

int main()
{
    BST<int> bst;

    for (int i = 0; i < 10; i++)
    {
        bst.insert(i);
    }
    bst.dump(std::cout); std::cout << std::endl;


    std::set<int> test;
    test.end();

    return 0;
}