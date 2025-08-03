#include "ConfigManager.h"
#include "iniReader\\INIReader.h"
#include "GlobalConfiguration.h"
#include <stdexcept>
#include <fstream>

// Static member definitions
std::unique_ptr<INIReader> ConfigManager::reader = nullptr;
std::string ConfigManager::configFile = "Config.ini";

bool ConfigManager::loadConfig(const std::string& filename) {
    configFile = filename;
    reader = std::make_unique<INIReader>(configFile);
    return reader->ParseError() == 0;
}

int64_t ConfigManager::getInt(const std::string& section, const std::string& key,
                             int64_t defaultValue) {
    if (!isLoaded()) return defaultValue;
    return reader->GetInteger(section, key, defaultValue);
}

double ConfigManager::getReal(const std::string& section, const std::string& key,
                            double defaultValue) {
    if (!isLoaded()) return defaultValue;
    return reader->GetReal(section, key, defaultValue);
}

void ConfigManager::getBorrowLimit() {
    librarian_borrow_limit = static_cast<int>(getInt("UserLimits", "librarian_borrow_limit", 100));
    regular_user_borrow_limit = static_cast<int>(getInt("UserLimits", "regular_user_borrow_limit", 5));
}

void ConfigManager::getLoanPeriod() {
    librarian_loan_period = static_cast<int>(getInt("LoanPeriods", "librarian_loan_period", 60));
    regular_user_loan_period = static_cast<int>(getInt("LoanPeriods", "regular_user_loan_period", 14));
}

void ConfigManager::getReservationPeriod() {
    reservation_period = static_cast<int>(getInt("LoanPeriods", "reservation_period", 7));
}

void ConfigManager::getDailyFineRate() {
    daily_fine_rate = getReal("Fines", "daily_fine_rate", 1.0);
}

void ConfigManager::getMaxFine() {
    max_fine = getReal("Fines", "max_fine", 50.0);
}

bool ConfigManager::saveConfig() {
    std::ofstream configStream("Config.ini");
    if (!configStream.is_open()) {
        return false;
    }

    // Write User Limits section
    configStream << "[UserLimits]\n";
    configStream << "regular_user_borrow_limit=" << regular_user_borrow_limit << "\n";
    configStream << "librarian_borrow_limit=" << librarian_borrow_limit << "\n\n";

    // Write Loan Periods section
    configStream << "[LoanPeriods]\n";
    configStream << "regular_user_loan_period=" << regular_user_loan_period << "\n";
    configStream << "librarian_loan_period=" << librarian_loan_period << "\n";
    configStream << "reservation_period=" << reservation_period << "\n\n";

    // Write Fines section
    configStream << "[Fines]\n";
    configStream << "daily_fine_rate=" << daily_fine_rate << "\n";
    configStream << "max_fine=" << max_fine << "\n";

    configStream.close();

    // Reload the configuration to ensure consistency
    return loadConfig(configFile);
}