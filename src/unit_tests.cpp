#include <gtest/gtest.h>

#include "BST.hpp"
#include "driver.hpp"

#include <iostream>

TEST(BST, InsertAndIterators)
{
    BST<int> bst;
    
    int left_bound = 0, right_bound = 5;

    for (int i = right_bound; i >= left_bound; --i)
        bst.insert(i);
    
    int i = left_bound;
    for (auto elem : bst)
        EXPECT_TRUE(elem == i++);
}

TEST(BST, BeginEnd)
{
    BST<int> bst;
    EXPECT_TRUE(bst.begin() == bst.end());

    bst.insert(19);
    EXPECT_FALSE(bst.begin() == bst.end());
    EXPECT_TRUE(std::next(bst.begin()) == bst.end());
}

TEST(BST, UpperLowerBound)
{
    BST<int> bst;
    std::set<int> set;
    EXPECT_TRUE(bst.lower_bound(19) == bst.end());
    EXPECT_TRUE(set.lower_bound(19) == set.end());

    EXPECT_TRUE(bst.upper_bound(19) == bst.end());
    EXPECT_TRUE(set.lower_bound(19) == set.end());

    bst.insert(1); set.insert(1);
    bst.insert(3); set.insert(3);
    bst.insert(5); set.insert(5);

    EXPECT_TRUE(*(bst.lower_bound(3)) == 3);
    EXPECT_TRUE(*(set.lower_bound(3)) == 3);

    EXPECT_TRUE(*(bst.upper_bound(3)) == 5);
    EXPECT_TRUE(*(set.upper_bound(3)) == 5);

    EXPECT_TRUE(*(bst.lower_bound(2)) == 3);
    EXPECT_TRUE(*(set.lower_bound(2)) == 3);    

    EXPECT_TRUE(*(bst.upper_bound(2)) == 3);
    EXPECT_TRUE(*(set.upper_bound(2)) == 3);
}

TEST(Driver, ExceptionUnknownCmd)
{
    std::stringstream inp, out;
    inp << "k 19 a 5 6";
    EXPECT_THROW((Driver::driver<std::set<int>, int>(inp, out)), Driver::UnknownCmdE);
}

TEST(Driver, ExceptionWrongInput)
{
    std::stringstream inp1, inp2, out;
    inp1 << "k e-5";
    EXPECT_THROW((Driver::driver<std::set<int>, int>(inp1, out)), Driver::WrongInputE);
    
    inp2 << "q 7 abc";
    EXPECT_THROW((Driver::driver<std::set<int>, int>(inp2, out)), Driver::WrongInputE);
}