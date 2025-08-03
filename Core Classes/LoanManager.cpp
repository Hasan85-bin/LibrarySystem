#include "LoanManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>
#include "../Utils/ini/GlobalConfiguration.h"

// LoanTransaction constructor implementation
LoanTransaction::LoanTransaction(int transId, int uId, int bId, const std::string& borrow, const std::string& due)
    : transactionId(transId), userId(uId), bookId(bId), borrowDate(borrow), dueDate(due),
      returnDate(""), fine(0.0), isReturned(false) {}

// Reservation constructor implementation
Reservation::Reservation(int uId, int bId, const std::string& date, const std::string& expiry)
    : userId(uId), bookId(bId), reservationDate(date), expiryDate(expiry) {}

// StandardFineCalculator implementation
double StandardFineCalculator::calculateFine(const std::string& dueDate, const std::string& returnDate) {
    // Simple date comparison - in production, use proper date parsing
    if (returnDate <= dueDate) return 0.0;

    auto dueDateTime = parseDate(dueDate);
    auto returnDateTime = parseDate(returnDate);

    // Calculate days overdue (simplified)
    int daysOverdue = (dueDateTime-returnDateTime)/60/60/24; // Simplified calculation
    return daysOverdue * daily_fine_rate; // Use global configuration for fine rate
}

time_t FineCalculator::parseDate(const std::string& dateStr) const{
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date");
    }
    // mktime expects tm_isdst to be set, -1 means mktime will attempt to determine whether DST is in effect
    tm.tm_isdst = -1;
    return std::mktime(&tm);
}

LoanManager::LoanManager() : nextTransactionId(1) {
    fineCalculator = std::make_unique<StandardFineCalculator>();
}

bool LoanManager::borrowBook(User* user, Book* book) {
    if (!user || !book || !canUserBorrowBook(user, book)) {
        return false;
    }

    // Create new transaction
    auto transaction = std::make_unique<LoanTransaction>(
        nextTransactionId++,
        user->getUserId(),
        book->getId(),
        getCurrentDate(),
        calculateDueDate(regular_user_loan_period) // 14 days loan period
    );

    transactions.push_back(std::move(transaction));
    book->setStatus(BookStatus::Borrowed);
    return true;
}

bool LoanManager::returnBook(User* user, Book* book) {
    if (!user || !book) {
        return false;
    }

    // Find the loan transaction
    auto it = std::find_if(transactions.begin(), transactions.end(),
        [user, book](const auto& transaction) {
            return transaction->userId == user->getUserId() &&
                   transaction->bookId == book->getId() &&
                   !transaction->isReturned;
        });

    if (it == transactions.end()) {
        return false;
    }

    // Update transaction
    (*it)->isReturned = true;
    (*it)->returnDate = getCurrentDate();
    
    // Calculate fine if overdue
    if (isDateOverdue((*it)->dueDate)) {
        (*it)->fine = fineCalculator->calculateFine((*it)->dueDate, (*it)->returnDate);
        user->addFine((*it)->fine);
    }

    book->setStatus(BookStatus::Available);

    // Check if there are reservations for this book
    auto& queue = reservations[book->getId()];
    cleanupExpiredReservations(queue);
    if (queue.empty()) {
        auto& reservation = queue.front();
        std::cout << "Book is now available for user " << reservation.userId << " (next in reservation queue)" << std::endl;
    }

    return true;
}

bool LoanManager::reserveBook(User* user, Book* book) {
    if (!user || !book) {
        return false;
    }

    // Can only reserve borrowed books
    if (book->getStatus() != BookStatus::Borrowed) {
        return false;
    }

    // Check if user already has a reservation for this book
    auto& bookReservations = reservations[book->getId()];
    cleanupExpiredReservations(bookReservations);
    std::queue<Reservation> tempQueue = bookReservations;
    bool hasReservation = false;
    
    while (!tempQueue.empty()) {
        const Reservation& res = tempQueue.front();
        if (res.userId == user->getUserId()) {
            hasReservation = true;
            break;
        }
        tempQueue.pop();
    }

    if (hasReservation) {
        return false; // User already has an active reservation
    }

    // Add new reservation
    bookReservations.push(Reservation(
        user->getUserId(),
        book->getId(),
        getCurrentDate(),
        calculateDueDate(reservation_period) // configuration reservation period
    ));

    return true;
}

bool LoanManager::payFine(User* user, double amount) {
    if (!user || amount <= 0) {
        return false;
    }

    double currentFines = user->getTotalFines();
    if (amount > currentFines) {
        amount = currentFines; // Can't pay more than owed
    }

    user->payFine(amount);
    return true;
}

bool LoanManager::isDateOverdue(const std::string& dueDate) const {
    return getCurrentDate() > dueDate;
}

void LoanManager::printOverdueBooks() const {
    std::cout << "\n=== Overdue Books ===" << std::endl;
    bool found = false;
    
    for (const auto& transaction : transactions) {
        if (!transaction->isReturned && isDateOverdue(transaction->dueDate)) {
            std::cout << "Book ID: " << transaction->bookId
                     << ", User ID: " << transaction->userId
                     << ", Due Date: " << transaction->dueDate
                     << ", Fine: $" << transaction->fine << std::endl;
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "No overdue books found." << std::endl;
    }
}

void LoanManager::printUserLoans(int userId) const {
    std::cout << "\n=== Current Loans ===" << std::endl;
    bool found = false;
    
    for (const auto& transaction : transactions) {
        if (transaction->userId == userId && !transaction->isReturned) {
            std::cout << "Book ID: " << transaction->bookId
                     << ", Borrowed: " << transaction->borrowDate
                     << ", Due: " << transaction->dueDate
                     << ", Fine: $" << transaction->fine << std::endl;
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "No current loans." << std::endl;
    }
}

void LoanManager::printUserReservations(int userId) {

    auto it = reservations.find(userId);
    cleanupExpiredReservations(it->second);
    if (it != reservations.end()) {
        std::queue<Reservation> queue = it->second; // Make a copy to iterate
        
        if (!queue.empty()) {
            std::cout << "\nReservations for User ID " << userId << ":\n";
            while (!queue.empty()) {
                const Reservation& res = queue.front();
                std::cout << "Book ID: " << res.bookId
                            << ", Reserved on: " << res.reservationDate
                            << ", Expires: " << res.expiryDate << std::endl;
                queue.pop();
            }
        } else {
            std::cout << "\nNo active reservations found.\n";
        }
    } else {
        std::cout << "\nNo reservations found for this user.\n";
    }
}

void LoanManager::printAllLoans() const {
    std::cout << "\n=== All Current Loans ===" << std::endl;
    bool found = false;
    
    for (const auto& transaction : transactions) {
        if (!transaction->isReturned) {
            std::cout << "User ID: " << transaction->userId
                     << ", Book ID: " << transaction->bookId
                     << ", Borrowed: " << transaction->borrowDate
                     << ", Due: " << transaction->dueDate
                     << ", Fine: $" << transaction->fine << std::endl;
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "No current loans." << std::endl;
    }
}

void LoanManager::printAllReservations() {
    std::cout << "\n=== All Current Reservations ===" << std::endl;
    bool found = false;
    for (auto& [bookId, queue] : reservations) {
        cleanupExpiredReservations(queue);
        if (!queue.empty()) {
            std::cout << "Book ID: " << bookId << " Reservations:" << std::endl;
            // If 'queue' is std::queue, copy to a std::vector for iteration
            std::queue<Reservation> tempQueue = queue;
            while (!tempQueue.empty()) {
                const Reservation& reservation = tempQueue.front();
                
                std::cout << "  User ID: " << reservation.userId
                            << ", Reserved: " << reservation.reservationDate
                            << ", Expires: " << reservation.expiryDate << std::endl;
                found = true;
                tempQueue.pop();
            }
        }
    }
    
    if (!found) {
        std::cout << "No current reservations." << std::endl;
    }
}

int LoanManager::getTotalLoans() const {
    return transactions.size();
}

int LoanManager::getActiveLoans() const {
    return std::count_if(transactions.begin(), transactions.end(),
        [](const auto& transaction) { return !transaction->isReturned; });
}

int LoanManager::getOverdueCount() const {
    return std::count_if(transactions.begin(), transactions.end(),
        [this](const auto& transaction) {
            return !transaction->isReturned && isDateOverdue(transaction->dueDate);
        });
}

double LoanManager::getTotalFines() const {
    double total = 0.0;
    for (const auto& transaction : transactions) {
        total += transaction->fine;
    }
    return total;
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



bool LoanManager::canUserBorrowBook(const User* user, const Book* book) const {
    if (!user || !book) return false;
    
    // Check if user can borrow
    if (!user->canBorrow()) return false;
    
    // Check if book is available
    if (book->getStatus() != BookStatus::Available) return false;
    
    // Check if user has reached borrowing limit
    if (getCurrentLoansCount(user) >= user->getBorrowLimit()) return false;
    
    return true;
}

void LoanManager::cleanupExpiredReservations(std::queue<Reservation>& queue) {
    std::string currentDate = getCurrentDate();
        std::queue<Reservation> activeQueue;
        while (!queue.empty()) {
            Reservation res = queue.front();
            if (currentDate <= res.expiryDate) {
                activeQueue.push(res);
            }
            queue.pop();
        }
        queue = activeQueue;
}

int LoanManager::getCurrentLoansCount(const User* user) const {
    if (!user) return 0;
    
    return std::count_if(transactions.begin(), transactions.end(),
        [user](const auto& transaction) {
            return transaction->userId == user->getUserId() && !transaction->isReturned;
        });
}
