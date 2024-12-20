#include <iostream>

#include "BST.hpp"

#include <set>

int main()
{
    BST<int> bst;
    std::set<int> test_set;

    auto insert_in_both = [&bst, &test_set](int val) {bst.insert(val); test_set.insert(val);};

    for (int i = 0; i < 20; i+=2)
    {
        insert_in_both(i);
    }

    insert_in_both(-19);
    insert_in_both(19);
    insert_in_both(42);

    bst.dump(std::cout); std::cout << std::endl;

    std::cout << "min: " << *(bst.begin()) << std::endl;
    std::cout << "max: " << *(--bst.end()) << std::endl;

    std::cout << "acsending order: " << std::endl;
    for (auto elem : bst)
        std::cout << elem << std::endl;

    std::cout << "descending order: " << std::endl;
    for (auto it = bst.end(); it != bst.begin(); )
        std::cout << *(--it) << std::endl;

    auto print_lower_bound = []<typename C>(const C& container, int val) 
    {
        auto lower_bound_it = container.lower_bound(val);
        if (lower_bound_it != container.end())
            std::cout << "lower bound of " << val << " is " << *(lower_bound_it) << std::endl;
        else
            std::cout << "lower bound of " << val << " doesn't exist" << std::endl;
    };

    auto print_upper_bound = []<typename C>(const C& container, int val) 
    {
        auto lower_bound_it = container.upper_bound(val);
        if (lower_bound_it != container.end())
            std::cout << "upper bound of " << val << " is " << *(lower_bound_it) << std::endl;
        else
            std::cout << "upper bound of " << val << " doesn't exist" << std::endl;
    };

    int s_val = 56;
    print_upper_bound(bst, s_val);
    print_lower_bound(bst, s_val);
    print_upper_bound(test_set, s_val);
    print_lower_bound(test_set, s_val);

    return 0;
}