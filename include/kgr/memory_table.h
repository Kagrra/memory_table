#ifndef KGR_MEMORY_TABLE_H
#define KGR_MEMORY_TABLE_H

#include <array>
#include <cstdint>
#include <string_view>
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

    // returns array with label in center
    template<int I> static constexpr std::array<char, I> in_center(const char* label)
    {
        auto strlen = [](const char* str) constexpr->size_t
        {
            size_t i = 0;
            while (str[++i]) { }
            return i;
        };

        std::array<char, I> arr {};
        arr.fill(' ');
        const int margin = (arr.size() - strlen(label)) / 2;

        for (int i = margin; i < margin + strlen(label); ++i) { arr[i] = label[i - margin]; }
        return arr;
    }

    // returns label as array for selected type
    template<typename T> static constexpr auto label()
    {
        if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
        {
            return in_center<width_of_v<T>>("float");
        }

        // here we want numbered bits
        if constexpr (std::is_same_v<T, bool>)
        {
            // becouse constexpr
            std::array<std::array<char, 2>, 8> bool_label
                = {{{' ', '0'}, {'7', ' '}, {' ', '8'}, {'1', '5'}, {'1', '6'}, {'2', '3'}, {'2', '4'}, {'3', '1'}}};
            std::array<char, width_of_v<bool>> arr;
            arr.fill(' ');
            int bool_state = 0;
            int bool_label_num = 0;
            for (int i = arr.size() - 1; i >= 0; i--)
            {
                switch (bool_state)
                {
                    case 0:
                    {
                        // space at the beginning of byte
                        bool_state = 1;
                        break;
                    }
                    case 1:
                    {
                        arr[i] = bool_label[bool_label_num][1];
                        bool_state++;
                        break;
                    }
                    case 2:
                    {
                        arr[i] = bool_label[bool_label_num][0];
                        bool_label_num++;
                        bool_state++;
                        break;
                    }
                    case 7:
                    {
                        arr[i] = bool_label[bool_label_num][1];
                        bool_state++;
                        break;
                    }
                    case 8:
                    {
                        arr[i] = bool_label[bool_label_num][0];
                        bool_label_num++;
                        bool_state = 0;
                        break;
                    }
                    default:
                    {
                        bool_state++;
                    }
                }
            }
            return arr;
        }

        if constexpr (std::is_same_v<T, char>)
        {
            return in_center<width_of_v<char>>("char");
        }

        if constexpr (std::is_same_v<T, uint8_t>)
        {
            return in_center<width_of_v<T>>("hex");
        }

        if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> //
                      || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> //
                      || std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t>)
        {
            return in_center<width_of_v<T>>("dec");
        }
    };

    constexpr static std::array<char, line_width> header_as_array = []() constexpr
    {
        std::array<char, 1> divider {'|'};
        std::array<char, line_width> arr;

        auto ptr = arr.begin();

        auto concat = [&](auto txt) {
            ptr = std::copy(divider.begin(), divider.end(), ptr);
            ptr = std::copy(txt.begin(), txt.end(), ptr);
        };

        concat(in_center<address_size>("addr"));
        (concat(label<Ts>()), ...);
        ptr = std::copy(divider.begin(), divider.end(), ptr);
        return arr;
    }
    ();

    constexpr static std::string_view header {header_as_array.data(), header_as_array.size()};
};

} // namespace kgr
#endif