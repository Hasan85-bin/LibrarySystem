// ConfigManager.h
#pragma once
#include <string>
#include <memory>
#include "iniReader\\INIReader.h"

// Static ConfigManager class
class ConfigManager {
private:
    static std::unique_ptr<INIReader> reader;
    static std::string configFile;

    ConfigManager() = delete;
    ~ConfigManager() = delete;

public:
    // All methods are static
    static bool loadConfig(const std::string& filename = "Config.ini");
    static bool saveConfig();

    // Specific getters for LibrarySystem
    static void getBorrowLimit();
    static void getLoanPeriod();
    static void getReservationPeriod();
    static void getDailyFineRate();
    static void getMaxFine();

    // Helper methods
private:
    static bool isLoaded() { return reader != nullptr && reader->ParseError() == 0; }
    static int64_t getInt(const std::string& section, const std::string& key, int64_t defaultValue);
    static double getReal(const std::string& section, const std::string& key, double defaultValue);
};