#ifndef VIDEO_POKER_SOLVER_BITCARDS_HPP_
#define VIDEO_POKER_SOLVER_BITCARDS_HPP_

#include "vps/combo.hpp"

#include <immintrin.h>

#include <bit>
#include <cstdint>
#include <limits>
#include <ranges>
#include <string_view>

namespace vps {

    class bitcards {
    public:
        static constexpr bitcards full_deck() { return bitcards{mask_full_deck}; }

        constexpr bitcards() = default;
        constexpr explicit bitcards(std::uint64_t a) : cards{a} {}
        constexpr explicit bitcards(const std::string_view words) : cards{0} {
            for (const auto& word : words | std::views::split(' ')) {
                cards += std::uint64_t{1} << offsets_rank(word[0]) << offsets_suit(word[1]);
            }
        }

        class sentinel {};
        class iterator {
        public:
            using iterator_category = std::input_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = bitcards;

            constexpr iterator() = default;
            constexpr iterator(int k, std::uint64_t cards) : curr{~(~std::uint64_t{0} << k)}, mask{cards} {}

            constexpr value_type operator*() const {
                return bitcards{__builtin_ia32_pdep_di(curr, mask)};
            }

            constexpr iterator& operator++() {
                std::uint64_t tmp = curr | (curr - 1);
                curr = (tmp + 1) | (((~tmp & -~tmp) - 1) >> (std::countr_zero(curr) + 1));
                return *this;
            }

            constexpr void operator++(int) { ++*this; }

            constexpr bool operator==(const sentinel&) const { return curr >> std::popcount(mask); }

        private:
            std::uint64_t curr{};
            std::uint64_t mask{};
        };

        constexpr auto choose(int k) const { return std::ranges::subrange(iterator(k, cards), sentinel()); }

        constexpr iterator begin() const { return iterator(1, cards); }
        constexpr sentinel end() const { return sentinel(); }

        constexpr std::strong_ordering operator<=>(const bitcards& rhs) const = default;

        constexpr bool operator[](std::size_t pos) const { return cards >> pos & 1; }

        constexpr bitcards operator+(const bitcards& rhs) const { return bitcards{cards |  rhs.cards}; }
        constexpr bitcards operator-(const bitcards& rhs) const { return bitcards{cards & ~rhs.cards}; }

        constexpr bitcards& operator+=(const bitcards& rhs) { cards |=  rhs.cards; return *this; }
        constexpr bitcards& operator-=(const bitcards& rhs) { cards &= ~rhs.cards; return *this; }

        constexpr size_t size() const { return std::popcount(cards); }

        constexpr bitcards equivalence() const {
            std::uint64_t a = cards >> offsets_suit('c') & mask_suit;
            std::uint64_t b = cards >> offsets_suit('d') & mask_suit;
            std::uint64_t c = cards >> offsets_suit('h') & mask_suit;
            std::uint64_t d = cards >> offsets_suit('s') & mask_suit;

            if (a < d) std::swap(a, d);
            if (b < c) std::swap(b, c);
            if (a < b) std::swap(a, b);
            if (c < d) std::swap(c, d);
            if (b < c) std::swap(c, b);

            return bitcards{(a << offsets_suit('c')) |
                            (b << offsets_suit('d')) |
                            (c << offsets_suit('h')) |
                            (d << offsets_suit('s'))};
        }

        constexpr combo eval() const {
            std::uint64_t folded = fold2(cards, std::bit_or<>());
            switch (std::popcount(folded)) {
                case 5:
                    if (is_flush(cards))
                        if (is_royal(cards))
                            return combo::royal_straight_flush;
                        else if (is_straight(folded))
                            return combo::straight_flush;
                        else
                            return combo::flush;
                    else if (is_straight(folded) || is_royal(cards))
                        return combo::straight;
                    else
                        return combo::nothing;
                case 4:
                    return is_jacks_or_better(cards) ? combo::jacks_or_better : combo::nothing;
                case 3:
                    return is_two_pair(cards) ? combo::two_pair : combo::three_of_a_kind;
                case 2:
                    return is_four_of_a_kind(cards) ? combo::four_of_a_kind : combo::full_house;
                default:
                    return combo::nothing;
            }
        }

    private:
        std::uint64_t cards{};

        static constexpr std::uint64_t mask_full_deck  = 0x1FFF1FFF1FFF1FFF;
        static constexpr std::uint64_t mask_jacks_plus = 0x1C011C011C011C01;
        static constexpr std::uint64_t mask_face       = 0x1E011E011E011E01;
        static constexpr std::uint64_t mask_suit       =             0xFFFF;

        static constexpr int offsets_suit(char c) {
            switch (c) {
                case 'c':return 0;
                case 'd':return 16;
                case 'h':return 32;
                case 's':return 48;
                default: return 0;
            }
        }

        static constexpr int offsets_rank(char c) {
            switch (c) {
                case 'A': return 0;
                case '2': return 1;
                case '3': return 2;
                case '4': return 3;
                case '5': return 4;
                case '6': return 5;
                case '7': return 6;
                case '8': return 7;
                case '9': return 8;
                case 'T': return 9;
                case 'J': return 10;
                case 'Q': return 11;
                case 'K': return 12;
                default: return 0;
            }
        }

        template <typename U>
        requires std::invocable<U, std::uint64_t, std::uint64_t>
        static constexpr std::uint64_t fold2(std::uint64_t hand, U op) {
            return op(op(hand >> offsets_suit('c') & mask_suit, hand >> offsets_suit('d') & mask_suit),
                      op(hand >> offsets_suit('h') & mask_suit, hand >> offsets_suit('s') & mask_suit));
        }

        static constexpr bool is_straight(std::uint64_t folded) {
            return std::numeric_limits<std::uint64_t>::digits - std::countl_zero(folded) - std::countr_zero(folded) == 5;
        }

        static constexpr bool is_four_of_a_kind(std::uint64_t hand) {
            return fold2(hand, std::bit_and<>());
        }

        static constexpr bool is_two_pair(std::uint64_t hand) {
            return std::popcount(fold2(hand, std::bit_xor<>())) == 1;
        }

        static constexpr bool is_royal(std::uint64_t hand) {
            return std::popcount(hand & mask_face) == 5;
        }

        static constexpr bool is_jacks_or_better(std::uint64_t hand) {
            return std::popcount(fold2(hand & mask_jacks_plus, std::bit_xor<>())) < std::popcount(hand & mask_jacks_plus);
        }

        static constexpr bool is_flush(std::uint64_t hand) {
            return std::popcount(hand >> offsets_suit('c') & mask_suit) == 5 ||
                   std::popcount(hand >> offsets_suit('d') & mask_suit) == 5 ||
                   std::popcount(hand >> offsets_suit('h') & mask_suit) == 5 ||
                   std::popcount(hand >> offsets_suit('s') & mask_suit) == 5;
        }

        friend std::hash<bitcards>;
    };

} // namespace vps

namespace std {
    template <>
    struct hash<vps::bitcards> {
        constexpr size_t operator()(const vps::bitcards& a) const {
            return a.cards;
        }
    };
} // namespace std

#endif // VIDEO_POKER_SOLVER_BITCARDS_HPP_
