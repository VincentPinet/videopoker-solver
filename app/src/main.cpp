#include <vps/bitcards.hpp>
#include <vps/combo.hpp>

#include <ranges>
#include <string>

constexpr vps::bitcards do_stuff(const vps::bitcards& cards) {
    vps::bitcards ret{};
    for (const auto& card : cards | std::views::take(5))
        ret += card;
    return ret;
}

int main()
{
    static constexpr vps::bitcards deck  = vps::bitcards("Ks Kh Kd Kc Qs Qh 9d 9c");
    static constexpr vps::bitcards cards = do_stuff(deck);
    static constexpr vps::combo    res   = cards.eval();
    static constexpr std::string   str   = vps::to_string(res);
    return str[1];
}
