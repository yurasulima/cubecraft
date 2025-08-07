//
// Created by mBlueberry on 07.08.2025.
//

// Logger.h
#pragma once

#include <string>

class Logger {
public:
    static void info(const std::string& msg);
    static void error(const std::string& msg);
};
