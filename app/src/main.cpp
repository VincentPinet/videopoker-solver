#include <vps/solver.hpp>

#include <iomanip>
#include <iostream>

int main() {
    
    double res = vps::solver::solve();
    std::cout << std::setprecision(7) << res << std::endl;
    
    return 0;
}
