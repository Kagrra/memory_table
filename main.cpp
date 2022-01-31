#include "include/kgr/memory_table.h"
#include <cstdint>
#include <iostream>
#include <string_view>

int main()
{
    using table_1 = kgr::memory_table<uint16_t, bool, uint8_t, uint32_t, float>;
    std::cout << table_1::header << '\n';
}