#pragma once

#include <iostream>

#define ASSERT(condition, message)                                                                                     \
    if (condition)                                                                                                     \
        std::cerr << "ASSERT ! " << message << std::endl;

#define ASSERT_RETURN_VOID(condition, message)                                                                         \
    if (condition)                                                                                                     \
    {                                                                                                                  \
        std::cerr << "ASSERT ! " << message << std::endl;                                                              \
        return;                                                                                                        \
    }

#define ASSERT_RETURN_VALUE(condition, message, returnValue)                                                           \
    if (condition)                                                                                                     \
    {                                                                                                                  \
        std::cerr << "ASSERT ! " << message << std::endl;                                                              \
        return returnValue;                                                                                            \
    }