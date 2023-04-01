#ifndef VIDEO_POKER_SOLVER_ENUM_HPP_
#define VIDEO_POKER_SOLVER_ENUM_HPP_

#include <iosfwd>
#include <string>
#include <type_traits>

template <typename T>
concept to_stringable = requires (T t) { to_string(t); };

template <typename T>
requires std::is_enum<T>::value && to_stringable<T>
std::ostream& operator<<(std::ostream& os, T value) {
    return os << to_string(value);
}

namespace vps {

    template<class T>
    requires std::is_enum<T>::value
    constexpr std::string to_string(T value);

} // namespace vps

#endif // VIDEO_POKER_SOLVER_ENUM_HPP_
