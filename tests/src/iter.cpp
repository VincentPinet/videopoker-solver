#include <vps/bitcards.hpp>
#include <vps/solver.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <unordered_set>

using namespace vps;

constexpr size_t nck(int n, int k) {
    if (!k) return 1;
    return nck(n - 1, k - 1) * n / k;
}

TEST(bitcards_iter, reconstruct) {

    bitcards expected{"Js Jh 2d 7c Ks"};
    ASSERT_EQ(std::ranges::fold_left(expected, bitcards{}, std::plus<>()), expected);
}

TEST(bitcards_iter, nchoosek) {

    bitcards deck = bitcards::full_deck();
    bitcards hand = bitcards{"As Kh Qd Jc 2s"};

    for (int i = 1; i <= 5; i++) {
        ASSERT_EQ(std::ranges::distance(deck.choose(i)), nck(deck.size(), i));
        ASSERT_EQ(std::ranges::distance(hand.choose(i)), nck(hand.size(), i));
    }
}

TEST(bitcards_iter, equivalence) {

    std::unordered_set<bitcards> seen{};
    for (const auto& hand : bitcards::full_deck().choose(5))
        seen.insert(hand.equivalence());

    ASSERT_EQ(seen.size(), 134459);
}
