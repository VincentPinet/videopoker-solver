#include <vps/solver.hpp>

#include <gtest/gtest.h>

#include <cmath>

TEST(solver, solve) {

    ASSERT_LT(std::abs(vps::solver::solve() - 0.995439), 1e-7);
}
