#include <vps/bitcards.hpp>
#include <vps/combo.hpp>

#include <gtest/gtest.h>

using namespace vps;

TEST(bitcards_eval, royal_straight_flush) {
  
    static auto expected = to_string(combo::royal_straight_flush);

    ASSERT_EQ(to_string(bitcards("As Ks Qs Js Ts").eval()), expected);
    ASSERT_EQ(to_string(bitcards("Ah Kh Qh Jh Th").eval()), expected);
    ASSERT_EQ(to_string(bitcards("Ad Kd Qd Jd Td").eval()), expected);
    ASSERT_EQ(to_string(bitcards("Ac Kc Qc Jc Tc").eval()), expected);

    ASSERT_NE(to_string(bitcards("Ks Qs Js Ts 9s").eval()), expected);
    ASSERT_NE(to_string(bitcards("Ah Kh Qh Jh 2h").eval()), expected);
    ASSERT_NE(to_string(bitcards("Ad Kc Qc Jc Tc").eval()), expected);
}

TEST(bitcards_eval, straight_flush) {

    static auto expected = to_string(combo::straight_flush);

    ASSERT_EQ(to_string(bitcards("As 2s 3s 4s 5s").eval()), expected);
    ASSERT_EQ(to_string(bitcards("8h 9h Th Jh Qh").eval()), expected);

    ASSERT_NE(to_string(bitcards("Kd Ad 2d 3d 4d").eval()), expected);
    ASSERT_NE(to_string(bitcards("Ac Kc Qc Jc Tc").eval()), expected);
}

TEST(bitcards_eval, four_of_a_kind) {

    static auto expected = to_string(combo::four_of_a_kind);

    ASSERT_EQ(to_string(bitcards("As Ah Ad Ac 2s").eval()), expected);
    
    ASSERT_NE(to_string(bitcards("As Ah Ad 2c 2s").eval()), expected);
    ASSERT_NE(to_string(bitcards("As Ah 2d 2c Ks").eval()), expected);
}

TEST(bitcards_eval, full_house) {

    static auto expected = to_string(combo::full_house);

    ASSERT_EQ(to_string(bitcards("Ks Kh Kd Qc Qs").eval()), expected);
}

TEST(bitcards_eval, flush) {

    static auto expected = to_string(combo::flush);

    ASSERT_EQ(to_string(bitcards("2s 7s Ts Js As").eval()), expected);
  }

TEST(bitcards_eval, straight) {

    static auto expected = to_string(combo::straight);

    ASSERT_EQ(to_string(bitcards("As Kh Qd Jc Ts").eval()), expected);
    ASSERT_EQ(to_string(bitcards("As 2h 3d 4c 5s").eval()), expected);
    ASSERT_EQ(to_string(bitcards("8c 9c Tc Jc Qh").eval()), expected);
    
    ASSERT_NE(to_string(bitcards("2d Ac Kc Qc Jc").eval()), expected);
}

TEST(bitcards_eval, three_of_a_kind) {

    static auto expected = to_string(combo::three_of_a_kind);

    ASSERT_EQ(to_string(bitcards("As Ah Ad 2c 3s").eval()), expected);
}

TEST(bitcards_eval, two_pair) {

    static auto expected = to_string(combo::two_pair);

    ASSERT_EQ(to_string(bitcards("As Ah Qd Qc Ks").eval()), expected);
}

TEST(bitcards_eval, jacks_or_better) {

    static auto expected = to_string(combo::jacks_or_better);

    ASSERT_EQ(to_string(bitcards("Js Jh 2d 3c 4s").eval()), expected);
    ASSERT_EQ(to_string(bitcards("Js Jh Qd Kc As").eval()), expected);
    ASSERT_EQ(to_string(bitcards("As Ah 2h 3h 4h").eval()), expected);
    ASSERT_EQ(to_string(bitcards("Ac Ad Kd Qd Td").eval()), expected);
    
    ASSERT_NE(to_string(bitcards("Ts Th Kd Qc Js").eval()), expected);
    ASSERT_NE(to_string(bitcards("As 2h 2d 3c 4s").eval()), expected);
}

TEST(bitcards_eval, nothing) {

    static auto expected = to_string(combo::nothing);

    ASSERT_EQ(to_string(bitcards("2s 5h 7d Tc Ks").eval()), expected);
}
