# memory_table
Class generating memory dump in the form of table.

Designed to show register memory of various peripherals, but usable in general memory dumps.

# Intended use
```c++
using my_table = memory_table<uint16_t, bool, uint16_t, uint8_t>;
std::cout << my_table.labels;
std::cout << my_table(address, value);
```

Expected result
```
| addr | 15     8 7      0 |  dec   |  hex  |
| 0x70 | 10101110 10000100 | 044676 | ae 84 |
| 0x72 | 10001001 01111000 | 035192 | 89 78 |
| 0x74 | 00011000 00110000 | 006192 | 18 30 |
```

