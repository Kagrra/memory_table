#include "include/kgr/memory_table.h"
#include <cstdint>
#include <iostream>
#include <string_view>

int main()
{
    using table_1 = kgr::memory_table<uint16_t, bool, uint8_t, uint32_t, float>;
    std::cout << table_1::header << '\n';
    table_1::print_data(0x69, {1, 2, 3, 4});
    table_1::print_data(0x70, 1234);
    table_1::print_data(0x71, 1.04f);
}