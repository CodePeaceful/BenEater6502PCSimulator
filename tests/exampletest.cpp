#include <toTest.hpp>
#include <gtest/gtest.h>
#include <tuple>
#include <vector>

TEST(AddTest, test1and1) {
    std::vector<std::tuple<int, int, int>> data {
        { 2, 1, 1 },
        { 0, 1, -1 }
    };
    for (const auto& [expected, param1, param2] : data) {
        const auto actual = add(param1, param2);
        ASSERT_EQ(expected, actual);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}