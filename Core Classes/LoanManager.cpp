#include "LoanManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>

// LoanTransaction Implementation
LoanTransaction::LoanTransaction(int transId, int uId, int bId, const std::string& borrow, const std::string& due)
    : transactionId(transId), userId(uId), bookId(bId), borrowDate(borrow), dueDate(due), 
      returnDate(""), fine(0.0), isReturned(false) {}

// Reservation Implementation
Reservation::Reservation(int uId, int bId, const std::string& date, const std::string& expiry)
    : userId(uId), bookId(bId), reservationDate(date), expiryDate(expiry), isActive(true) {}

// StandardFineCalculator Implementation
double StandardFineCalculator::calculateFine(const std::string& dueDate, const std::string& returnDate) {
    // Simple date comparison - in production, use proper date parsing
    if (returnDate <= dueDate) return 0.0;
    
    // Calculate days overdue (simplified)
    int daysOverdue = 1; // Simplified calculation
    return daysOverdue * dailyRate;
}

// BookTypeFineCalculator Implementation
BookTypeFineCalculator::BookTypeFineCalculator() {
    rates["TextBook"] = 2.0;
    rates["Magazine"] = 1.5;
    rates["ReferenceBook"] = 5.0;
    rates["Book"] = 1.0;
}

double BookTypeFineCalculator::calculateFine(const std::string& dueDate, const std::string& returnDate) {
    if (returnDate <= dueDate) return 0.0;
    
    int daysOverdue = 1; // Simplified calculation
    return daysOverdue * rates["Book"]; // Default rate
}

void BookTypeFineCalculator::setRate(const std::string& bookType, double rate) {
    rates[bookType] = rate;
}

// LoanManager Implementation
LoanManager::LoanManager() : nextTransactionId(1) {
    fineCalculator = std::make_unique<StandardFineCalculator>();
}

std::string LoanManager::getCurrentDate() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d");
    return ss.str();
}

std::string LoanManager::calculateDueDate(int loanPeriod) const {
    auto now = std::chrono::system_clock::now();
    auto due = now + std::chrono::hours(24 * loanPeriod);
    auto time_t = std::chrono::system_clock::to_time_t(due);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d");
    return ss.str();
}

bool LoanManager::isDateOverdue(const std::string& dueDate) const {
    return getCurrentDate() > dueDate;
}

int LoanManager::daysBetween(const std::string& date1, const std::string& date2) const {
    // Simplified calculation - in production, use proper date parsing
    return 1; // Placeholder
}

bool LoanManager::canUserBorrowBook(const User* user, const Book* book) const {
    if (!user || !book) return false;
    
    // Check if user can borrow
    if (!user->canBorrow()) return false;
    
    // Check if book is available
    if (book->getStatus() != BookStatus::Available) return false;
    
    // Check if user has reached borrowing limit
    if (getCurrentLoansCount(user) >= user->getBorrowLimit()) return false;
    
    // Check if book is borrowable (not reference only)
    if (book->getStatus() == BookStatus::ReferenceOnly) return false;
    
    return true;
}

int LoanManager::getCurrentLoansCount(const User* user) const {
    int count = 0;
    for (const auto& transaction : transactions) {
        if (transaction->userId == user->getUserId() && !transaction->isReturned) {
            count++;
        }
    }
    return count;
}

bool LoanManager::borrowBook(User* user, Book* book) {
    if (!canUserBorrowBook(user, book)) {
        std::cout << "Cannot borrow book. Check user status, book availability, or borrowing limits." << std::endl;
        return false;
    }
    
    std::string currentDate = getCurrentDate();
    std::string dueDate = calculateDueDate(user->getLoanPeriod());
    
    auto transaction = std::make_unique<LoanTransaction>(
        nextTransactionId++, user->getUserId(), book->getId(), currentDate, dueDate
    );
    
    transactions.push_back(std::move(transaction));
    book->setStatus(BookStatus::Borrowed);
    
    std::cout << "Book '" << book->getTitle() << "' borrowed successfully. Due: " << dueDate << std::endl;
    return true;
}

bool LoanManager::returnBook(User* user, Book* book) {
    // Find the active transaction for this user and book
    for (auto& transaction : transactions) {
        if (transaction->userId == user->getUserId() && 
            transaction->bookId == book->getId() && 
            !transaction->isReturned) {
            
            std::string returnDate = getCurrentDate();
            transaction->returnDate = returnDate;
            transaction->isReturned = true;
            
            // Calculate fine if overdue
            if (isDateOverdue(transaction->dueDate)) {
                transaction->fine = fineCalculator->calculateFine(transaction->dueDate, returnDate);
                user->addFine(transaction->fine);
                std::cout << "Book returned late. Fine: $" << transaction->fine << std::endl;
            } else {
                std::cout << "Book returned on time. No fine." << std::endl;
            }
            
            book->setStatus(BookStatus::Available);
            
            // Check if there are reservations for this book
            if (!reservations[book->getId()].empty()) {
                auto& reservation = reservations[book->getId()].front();
                std::cout << "Book is now available for user " << reservation.userId << " (next in reservation queue)" << std::endl;
            }
            
            return true;
        }
    }
    
    std::cout << "No active loan found for this user and book." << std::endl;
    return false;
}

bool LoanManager::reserveBook(User* user, Book* book) {
    if (!user->canReserve()) {
        std::cout << "User cannot reserve books." << std::endl;
        return false;
    }
    
    if (book->getStatus() == BookStatus::Available) {
        std::cout << "Book is available. No need to reserve." << std::endl;
        return false;
    }
    
    // Check if user already has a reservation for this book
    auto& bookReservations = reservations[book->getId()];
    std::queue<Reservation> tempQueue = bookReservations;
    bool hasReservation = false;
    
    while (!tempQueue.empty()) {
        const Reservation& reservation = tempQueue.front();
        if (reservation.userId == user->getUserId() && reservation.isActive) {
            std::cout << "User already has a reservation for this book." << std::endl;
            return false;
        }
        tempQueue.pop();
    }
    
    std::string currentDate = getCurrentDate();
    std::string expiryDate = calculateDueDate(1); // 24 hours
    
    Reservation reservation(user->getUserId(), book->getId(), currentDate, expiryDate);
    bookReservations.push(reservation);
    
    std::cout << "Book '" << book->getTitle() << "' reserved successfully." << std::endl;
    return true;
}

bool LoanManager::cancelReservation(User* user, Book* book) {
    auto& bookReservations = reservations[book->getId()];
    std::queue<Reservation> tempQueue;
    bool found = false;
    
    while (!bookReservations.empty()) {
        Reservation reservation = bookReservations.front();
        bookReservations.pop();
        
        if (reservation.userId == user->getUserId() && reservation.isActive) {
            reservation.isActive = false;
            found = true;
            std::cout << "Reservation cancelled successfully." << std::endl;
        }
        
        tempQueue.push(reservation);
    }
    
    bookReservations = tempQueue;
    return found;
}

std::vector<Reservation> LoanManager::getBookReservations(int bookId) const {
    std::vector<Reservation> result;
    auto it = reservations.find(bookId);
    if (it != reservations.end()) {
        std::queue<Reservation> tempQueue = it->second;
        while (!tempQueue.empty()) {
            result.push_back(tempQueue.front());
            tempQueue.pop();
        }
    }
    return result;
}

double LoanManager::calculateUserFines(const User* user) const {
    double totalFine = 0.0;
    for (const auto& transaction : transactions) {
        if (transaction->userId == user->getUserId() && transaction->isReturned) {
            totalFine += transaction->fine;
        }
    }
    return totalFine;
}

bool LoanManager::payFine(User* user, double amount) {
    if (amount <= 0) return false;
    
    double currentFines = user->getTotalFines();
    if (amount > currentFines) {
        amount = currentFines;
    }
    
    user->payFine(amount);
    std::cout << "Fine payment of $" << amount << " processed." << std::endl;
    return true;
}

std::vector<LoanTransaction*> LoanManager::getUserTransactions(int userId) const {
    std::vector<LoanTransaction*> result;
    for (const auto& transaction : transactions) {
        if (transaction->userId == userId) {
            result.push_back(transaction.get());
        }
    }
    return result;
}

std::vector<LoanTransaction*> LoanManager::getBookTransactions(int bookId) const {
    std::vector<LoanTransaction*> result;
    for (const auto& transaction : transactions) {
        if (transaction->bookId == bookId) {
            result.push_back(transaction.get());
        }
    }
    return result;
}

std::vector<LoanTransaction*> LoanManager::getOverdueTransactions() const {
    std::vector<LoanTransaction*> result;
    std::string currentDate = getCurrentDate();
    
    for (const auto& transaction : transactions) {
        if (!transaction->isReturned && isDateOverdue(transaction->dueDate)) {
            result.push_back(transaction.get());
        }
    }
    return result;
}

int LoanManager::getTotalLoans() const {
    return transactions.size();
}

int LoanManager::getActiveLoans() const {
    int count = 0;
    for (const auto& transaction : transactions) {
        if (!transaction->isReturned) count++;
    }
    return count;
}

int LoanManager::getOverdueCount() const {
    return getOverdueTransactions().size();
}

double LoanManager::getTotalFines() const {
    double total = 0.0;
    for (const auto& transaction : transactions) {
        total += transaction->fine;
    }
    return total;
}

void LoanManager::setFineCalculator(std::unique_ptr<FineCalculator> calculator) {
    fineCalculator = std::move(calculator);
}

void LoanManager::printTransactionHistory(int userId) const {
    std::cout << "\n=== Transaction History for User " << userId << " ===" << std::endl;
    auto userTransactions = getUserTransactions(userId);
    
    if (userTransactions.empty()) {
        std::cout << "No transactions found." << std::endl;
        return;
    }
    
    for (const auto& transaction : userTransactions) {
        std::cout << "Transaction ID: " << transaction->transactionId
                  << ", Book ID: " << transaction->bookId
                  << ", Borrowed: " << transaction->borrowDate
                  << ", Due: " << transaction->dueDate
                  << ", Returned: " << (transaction->isReturned ? transaction->returnDate : "Not returned")
                  << ", Fine: $" << transaction->fine << std::endl;
    }
}

void LoanManager::printOverdueBooks() const {
    std::cout << "\n=== Overdue Books ===" << std::endl;
    auto overdueTransactions = getOverdueTransactions();
    
    if (overdueTransactions.empty()) {
        std::cout << "No overdue books." << std::endl;
        return;
    }
    
    for (const auto& transaction : overdueTransactions) {
        std::cout << "User " << transaction->userId 
                  << " has overdue book " << transaction->bookId
                  << " (Due: " << transaction->dueDate << ")" << std::endl;
    }
} 