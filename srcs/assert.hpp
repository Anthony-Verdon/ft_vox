#pragma once

#include "classes/Time/Time.hpp"
#include <iostream>

// @todo need to go through all classes and add a capital letter to all functions
#define ASSERT(condition, message)                                                                                     \
    if (condition)                                                                                                     \
    {                                                                                                                  \
        if (LOG_ASSERT)                                                                                                \
            std::cerr << "[" << Time::getTime() << "] ASSERT ! " << message << std::endl;                              \
    }

#define ASSERT_RETURN_VOID(condition, message)                                                                         \
    if (condition)                                                                                                     \
    {                                                                                                                  \
        if (LOG_ASSERT)                                                                                                \
            std::cerr << "[" << Time::getTime() << "] ASSERT ! " << message << std::endl;                              \
        return;                                                                                                        \
    }

#define ASSERT_RETURN_VALUE(condition, message, returnValue)                                                           \
    if (condition)                                                                                                     \
    {                                                                                                                  \
        if (LOG_ASSERT)                                                                                                \
            std::cerr << "[" << Time::getTime() << "] ASSERT ! " << message << std::endl;                              \
        return returnValue;                                                                                            \
    }