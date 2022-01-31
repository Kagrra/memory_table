#ifndef KGR_MEMORY_TABLE_H
#define KGR_MEMORY_TABLE_H

#include <cstdint>
#include <type_traits>

namespace kgr
{
template<typename AddressType, typename... Ts> struct memory_table
{
    constexpr static std::size_t margin_width = 2;
    constexpr static std::size_t bytes = std::max({sizeof(Ts)...});
    constexpr static std::size_t address_width = 2 /* 0x */ + sizeof(AddressType) * 2 + margin_size;

    // space for each field
    // 8 characters for floating types
    // 8*bytes for bool with spaces between bytes
    // 2*bytes for uint8_t with spaces between bytes (hex display)
    // 3*bytes for decimal types
    // 1*bytes for char with spaces between

    template<typename T> struct width_of
    {
        constexpr static std::size_t value = []() constexpr
        {
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            {
                static_assert(sizeof(T) == bytes);
                return margin_width + 8;
            }
            if constexpr (std::is_same_v<T, bool>)
                return margin_width + bytes * 8 + bytes - 1;

            if constexpr (std::is_same_v<T, uint8_t>)
                return margin_width + bytes * 2 + bytes - 1;

            if constexpr (std::is_same_v<T, char>)
                return margin_width + bytes + bytes - 1;

            if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> //
                          || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> //
                          || std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t>)
            {
                static_assert(sizeof(T) == bytes);
                return margin_width + bytes * 3;
            }

            throw;
        }
        ();
    };

    template<typename T> constexpr static std::size_t width_of_v = width_of<T>::value;

    constexpr static std::size_t line_width = address_width + (width_of<Ts>::value + ...) + sizeof...(Ts) + 2;
};

} // namespace kgr
#endif