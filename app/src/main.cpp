#include <vps/bitcards.hpp>
#include <vps/combo.hpp>

#include <string>

int main()
{
    static constexpr vps::combo res = vps::bitcards("Ks Qs Js Ts 9s").eval();
    static constexpr std::string str = vps::to_string(res);

    return str[0];
}
