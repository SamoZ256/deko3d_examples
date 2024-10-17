#pragma once

#include <cstring>

#include <switch.h>

template<typename T>
inline T align(T num, T alignment) {
    return (num + alignment - 1) & ~(alignment - 1);
}
