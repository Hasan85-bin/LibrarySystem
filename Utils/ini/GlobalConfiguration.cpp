// filepath: c:\Users\Farnam\Documents\CodeBlocks\LibrarySystem\LibrarySystem\LibrarySystem\LibrarySystem\Utils\ini\GlobalConfiguration.cpp
#include "GlobalConfiguration.h"
#include "ConfigManager.h"
#include <iostream>

int librarian_borrow_limit = 100;
int regular_user_borrow_limit = 5;
int librarian_loan_period = 60;
int regular_user_loan_period = 14;
int reservation_period = 7;
double daily_fine_rate = 1.0;
double max_fine = 50.0;

bool loadGlobalConfigurationFromIni() {
    try{ 
        ConfigManager::loadConfig("Config.ini");
        ConfigManager::getBorrowLimit();
        ConfigManager::getLoanPeriod();
        ConfigManager::getReservationPeriod();
        ConfigManager::getDailyFineRate();
        ConfigManager::getMaxFine();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading configuration: " << e.what() << std::endl;
        return false;
    }

}