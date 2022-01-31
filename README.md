# memory_table
Class generating memory dump in the form of table.

Designed to show register memory of various peripherals, but usable in general memory dumps.

# Intended use
```c++
using table_1 = kgr::memory_table<uint16_t, bool, uint8_t, uint32_t, float>;
std::cout << table_1::header << '\n';
table_1::print_data(0x69, {1, 2, 3, 4});
table_1::print_data(0x70, 1234);
table_1::print_data(0x71, 1.04f);
```

Expected result
```
|  addr  | 31    24 23    16 15     8 7      0 |     hex     |     dec      |    float     |
| 0x0069 | 00000100 00000011 00000010 00000001 | 04 03 02 01 | 000067305985 | 01.53999e-36 |
| 0x0070 | 00000000 00000000 00000100 11010010 | 00 00 04 d2 | 000000001234 | 001.7292e-42 |
| 0x0071 | 00111111 10000101 00011110 10111000 | 3f 85 1e b8 | 001065688760 | 000000001.04 |
```

# Example
```sh
g++ main.cpp -std=c++20 -O3
```
