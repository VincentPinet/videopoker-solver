#include "vps/solver.hpp"

#include "vps/bitcards.hpp"
#include "vps/combo.hpp"

#include <algorithm>
#include <unordered_map>


namespace {
    
    using distribution = std::unordered_map<vps::combo, std::size_t>;
    
    constexpr size_t nck(int n, int k) {
        if (!k) return 1;
        return nck(n - 1, k - 1) * n / k;
    }

    constexpr double ev(const distribution& dist) {
        double res = 0;
        double sum = 0;
        for (const auto& [k, v] : dist) {
            res += payout(k) * v;
            sum += v;
        }
        return res / sum;
    }

    void add(distribution& a, const distribution& b) {
        for (const auto& [k, v] : b)
            a[k] += v;
    }

    void remove(distribution& a, const distribution& b) {
        for (const auto& [k, v] : b)
            a[k] -= v;
    }

    std::unordered_map<vps::bitcards, distribution> memo{};
    std::unordered_map<vps::bitcards, std::size_t> occ{};

    class init {
    public: 
        init() {
            for(const auto& hand : vps::bitcards::full_deck().choose(5)) {
                vps::combo curr = hand.eval();
                memo[{}][curr]++;
                memo[hand][curr]++;
                occ[hand.equivalence()]++;
            }

            for (size_t k = 1; k < 5; k++)
                for(const auto& hand : vps::bitcards::full_deck().choose(k))
                    for (const auto& outcome : (vps::bitcards::full_deck() - hand).choose(5 - k))
                        memo[hand][(hand + outcome).eval()]++;
        }
    } initializer{};

} // namespace

namespace vps {

    double solver::solve() {
        double res = 0;
        for (const auto& [hand, freq] : occ) {
            double best = payout(hand.eval());
            for (int i = 1; i <= 5; i++) {
                for (const auto& discards : hand.choose(i)) {
                    bitcards holding = hand - discards;
                    distribution curr = memo[holding];
                    for (int j = 1; j <= i; j++) {
                        for (const auto& cards : discards.choose(j)) {
                            if (j % 2) remove(curr, memo[holding + cards]);
                            else add(curr, memo[holding + cards]);
                        }
                    }
                    best = std::max(best, ev(curr));
                }
            }
            res += best * freq / nck(52, 5);
        }
        return res;
    }

} // namespace vps
