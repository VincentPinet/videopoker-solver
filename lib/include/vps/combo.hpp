#ifndef VIDEO_POKER_SOLVER_COMBO_HPP_
#define VIDEO_POKER_SOLVER_COMBO_HPP_

#include "vps/enum.hpp"

#include <string>
#include <utility>

namespace vps {

    enum class combo {
        nothing,
        jacks_or_better,
        two_pair,
        three_of_a_kind,
        straight,
        flush,
        full_house,
        four_of_a_kind,
        straight_flush,
        royal_straight_flush
    };

    template <>
    constexpr std::string to_string(combo val) {
        switch (val) {
            case combo::royal_straight_flush: return "royal_straight_flush";
            case combo::straight_flush: return "straight_flush";
            case combo::four_of_a_kind: return "four_of_a_kind";
            case combo::full_house: return "full_house";
            case combo::flush: return "flush";
            case combo::straight: return "straight";
            case combo::three_of_a_kind: return "three_of_a_kind";
            case combo::two_pair: return "two_pair";
            case combo::jacks_or_better: return "jacks_or_better";
            case combo::nothing: return "nothing";
            default: return std::to_string(std::to_underlying(val));
        }
    }

    constexpr double payout(combo val) {
        switch (val) {
            case combo::royal_straight_flush: return 800;
            case combo::straight_flush:       return 50;
            case combo::four_of_a_kind:       return 25;
            case combo::full_house:           return 9;
            case combo::flush:                return 6;
            case combo::straight:             return 4;
            case combo::three_of_a_kind:      return 3;
            case combo::two_pair:             return 2;
            case combo::jacks_or_better:      return 1;
            default: return 0;
        }
    }

} // namespace vps

#endif // VIDEO_POKER_SOLVER_COMBO_HPP_
