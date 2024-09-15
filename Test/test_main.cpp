/*
 * test_main.cpp
 *
 *  Created on: September 12, 2024
 *      Author: Andrew Korell
 */

#include <gtest/gtest.h>


int main(int ac, char* av[])
{
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}
