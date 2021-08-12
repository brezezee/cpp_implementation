#pragma once 
#include <cstdio>

struct count_block {
    size_t shared_count = 1;
    size_t weak_count = 0;
};
