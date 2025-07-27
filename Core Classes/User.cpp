#include "User.h"
#include <iostream>
#include <algorithm>

// User Base Class Implementation
User::User(int userId, const std::string& username, const std::string& password, UserRole role)
    : userId(userId), username(username), password(password), role(role), status(UserStatus::Active), totalFines(0.0) {}

int User::getUserId() const { return userId; }
std::string User::getUsername() const { return username; }
UserRole User::getRole() const { return role; }
UserStatus User::getStatus() const { return status; }
double User::getTotalFines() const { return totalFines; }
const std::vector<LoanRecord>& User::getLoanHistory() const { return loanHistory; }

void User::setStatus(UserStatus newStatus) { status = newStatus; }
void User::addLoanRecord(const LoanRecord& record) { loanHistory.push_back(record); }
void User::addFine(double amount) { totalFines += amount; }
void User::payFine(double amount) { totalFines = std::max(0.0, totalFines - amount); }

bool User::authenticate(const std::string& password) const {
    // For demonstration, just compare the password directly (insecure, replace with hash in production)
    return password == this->password;
}

// RegularUser Implementation
RegularUser::RegularUser(int userId, const std::string& username, const std::string& password)
    : User(userId, username, password, UserRole::Regular) {}

bool RegularUser::canBorrow() const { return status == UserStatus::Active && totalFines < 50.0; }
int RegularUser::getBorrowLimit() const { return 5; }
int RegularUser::getLoanPeriod() const { return 14; } // days
bool RegularUser::canReserve() const { return status == UserStatus::Active; }
bool RegularUser::canManageUsers() const { return false; }
bool RegularUser::canManageBooks() const { return false; }
bool RegularUser::canHandleFines() const { return false; }
bool RegularUser::canViewLogs() const { return false; }
std::string RegularUser::getType() const { return "RegularUser"; }

// Librarian Implementation
Librarian::Librarian(int userId, const std::string& username, const std::string& password)
    : User(userId, username, password, UserRole::Librarian) {}

bool Librarian::canBorrow() const { return true; }
int Librarian::getBorrowLimit() const { return 100; } // Arbitrary high limit
int Librarian::getLoanPeriod() const { return 60; } // days
bool Librarian::canReserve() const { return true; }
bool Librarian::canManageUsers() const { return true; }
bool Librarian::canManageBooks() const { return true; }
bool Librarian::canHandleFines() const { return true; }
bool Librarian::canViewLogs() const { return true; }
std::string Librarian::getType() const { return "Librarian"; }
