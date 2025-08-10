//
// Created by mBlueberry on 07.08.2025.
//

// Logger.cpp
#include "Logger.h"

#ifdef __ANDROID__
#include <android/log.h>
#define TAG "CubeCraft"
#endif

#include <iostream>

void Logger::info(const std::string& msg) {
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_INFO, TAG, "%s", msg.c_str());
#else
    std::cout << "[INFO] " << msg << std::endl;
#endif
}

void Logger::error(const std::string& msg) {
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_ERROR, TAG, "%s", msg.c_str());
#else
    std::cerr << "[ERROR] " << msg << std::endl;
#endif
}

void Logger::debug(const std::string& msg) {
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_ERROR, TAG, "%s", msg.c_str());
#else
    std::cerr << "[DEBUG] " << msg << std::endl;
#endif
}
