#ifndef VIDEO_POKER_SOLVER_COMBO_HPP_
#define VIDEO_POKER_SOLVER_COMBO_HPP_

#include "vps/enum.hpp"

#include <cstdint>

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
        royal_straight_flush,
    };

    template<>
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
            default: return std::to_string(static_cast<std::uint32_t>(val));
        }
    }

} // namespace vps

#endif // VIDEO_POKER_SOLVER_COMBO_HPP_
