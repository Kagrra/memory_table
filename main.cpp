#include "include/kgr/memory_table.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <string_view>

int main()
{
    std::cout << "\n\nGeneral usage:\n";
    {
        using table = kgr::memory_table<uint16_t, bool, uint8_t, uint32_t, float>;
        std::cout << table::header << '\n';
        table::print_data(0x69, {1, 2, 3, 4});
        table::print_data(0x70, 1234);
        table::print_data(0x71, 1.04f);
    }

    std::cout << "\n\nstd::array<int, 20> dump example:\n";
    {
        std::array<int, 20> test {};
        std::iota(test.begin(), test.end(), 1);

        using table = kgr::memory_table<uintptr_t, bool, int, uint8_t>;
        std::cout << table::header << '\n';
        for (const auto& v : test) { table::print_data(reinterpret_cast<uintptr_t>(&v), v); }
    }

    std::cout << "\n\nstd::array<float, 20> dump example with names:\n";
    {
        std::array<float, 20> test {};
        std::iota(test.begin(), test.end(), 100.5f);

        using table = kgr::memory_table<uintptr_t, float, bool, uint8_t>;
        std::cout << "|    name    " << table::header << '\n';
        int i = 0;
        for (const auto& v : test)
        {
            std::cout << "| " << std::setw(10) << std::setfill(' ') << "REG" + std::to_string(i++) << ' ';
            table::print_data(reinterpret_cast<uintptr_t>(&v), v);
        }
    }
}