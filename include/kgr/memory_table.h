#ifndef KGR_MEMORY_TABLE_H
#define KGR_MEMORY_TABLE_H

#include <array>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <type_traits>

namespace kgr
{
template<typename AddressType, typename... Ts> class memory_table
{
private:
    constexpr static std::size_t margin_width = 2;
    constexpr static std::size_t bytes = std::max({sizeof(Ts)...});
    constexpr static std::size_t address_width = 2 /* 0x */ + sizeof(AddressType) * 2 + margin_width;
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
                return margin_width + 12;
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

    constexpr static std::size_t table_width_ = address_width + (width_of_v<Ts> + ...) + sizeof...(Ts) + 2;

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
            // because constexpr
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

    constexpr static std::array<char, table_width_> header_as_array_ = []() constexpr
    {
        std::array<char, 1> divider {'|'};
        std::array<char, table_width_> arr;

        auto ptr = arr.begin();

        auto concat = [&](auto txt) {
            ptr = std::copy(divider.begin(), divider.end(), ptr);
            ptr = std::copy(txt.begin(), txt.end(), ptr);
        };

        concat(in_center<address_width>("addr"));
        (concat(label<Ts>()), ...);
        ptr = std::copy(divider.begin(), divider.end(), ptr);
        return arr;
    }
    ();

    template<typename T> static auto print(const std::array<uint8_t, bytes>& data) -> void
    {
        if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
        {
            T temp;
            memcpy(&temp, data.data(), sizeof(T));
            std::cout << ' ' << std::setw(width_of_v<T> - 2) << temp << " |";
        }

        if constexpr (std::is_same_v<T, bool>)
        {
            std::cout << " ";
            for (int i = bytes - 1; i >= 0; i--)
            {
                std::bitset<8> x(data[i]);
                std::cout << x << ' ';
            }
            std::cout << "|";
        }

        if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> //
                      || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> //
                      || std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t>)
        {
            T temp {};
            memcpy(&temp, data.data(), sizeof(T));
            std::cout << " " << std::dec << std::setw(width_of_v<T> - margin_width) << temp << " |";
        }

        if constexpr (std::is_same_v<T, char>)
        {
            std::cout << " ";
            for (int i = bytes - 1; i >= 0; i--) { std::cout << (char)data[i] << " "; }
            std::cout << "|";
        }

        if constexpr (std::is_same_v<T, uint8_t>)
        {
            std::cout << " ";
            for (int i = bytes - 1; i >= 0; i--) std::cout << std::setw(2) << std::hex << (int)data[i] << " ";
            std::cout << "|";
        }
    };

public:
    constexpr static std::string_view header {header_as_array_.data(), header_as_array_.size()};
    constexpr static std::size_t line_width {table_width_};

    static void print_data(AddressType address, std::array<uint8_t, bytes> data)
    {
        std::cout << "| 0x" << std::setw(sizeof(AddressType) * 2) << std::setfill('0') << std::hex << (int)address
                  << " |";
        (print<Ts>(data), ...);
        std::cout << '\n';
    }

    template<typename T> static void print_data(AddressType address, T data)
    {
        static_assert(sizeof(T) == bytes);
        std::array<uint8_t, bytes> temp {};
        memcpy(temp.data(), &data, bytes);
        print_data(address, temp);
    }
};

} // namespace kgr
#endif