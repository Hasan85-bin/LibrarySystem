#include "LoanManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>

// LoanTransaction constructor implementation
LoanTransaction::LoanTransaction(int transId, int uId, int bId, const std::string& borrow, const std::string& due)
    : transactionId(transId), userId(uId), bookId(bId), borrowDate(borrow), dueDate(due),
      returnDate(""), fine(0.0), isReturned(false) {}

// Reservation constructor implementation
Reservation::Reservation(int uId, int bId, const std::string& date, const std::string& expiry)
    : userId(uId), bookId(bId), reservationDate(date), expiryDate(expiry), isActive(true) {}

// StandardFineCalculator implementation
double StandardFineCalculator::calculateFine(const std::string& dueDate, const std::string& returnDate) {
    // Simple date comparison - in production, use proper date parsing
    if (returnDate <= dueDate) return 0.0;
    
    // Calculate days overdue (simplified)
    int daysOverdue = 1; // Simplified calculation
    return daysOverdue * dailyRate;
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
        calculateDueDate(14) // 14 days loan period
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
    std::queue<Reservation> tempQueue = bookReservations;
    bool hasReservation = false;
    
    while (!tempQueue.empty()) {
        const Reservation& res = tempQueue.front();
        if (res.userId == user->getUserId() && res.isActive) {
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
        calculateDueDate(7) // 7 days reservation period
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

void LoanManager::printUserReservations(int userId) const {
    std::cout << "\n=== Current Reservations ===" << std::endl;
    bool found = false;
    
    for (const auto& [bookId, queue] : reservations) {
        auto it = std::find_if(queue.front(), queue.back(),
            [userId](const Reservation& res) { return res.userId == userId && res.isActive; });
            
        if (it != queue.end()) {
            std::cout << "Book ID: " << bookId
                     << ", Reserved on: " << it->reservationDate
                     << ", Expires: " << it->expiryDate << std::endl;
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "No current reservations." << std::endl;
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

void LoanManager::printAllReservations() const {
    std::cout << "\n=== All Current Reservations ===" << std::endl;
    bool found = false;
    
    for (const auto& [bookId, queue] : reservations) {
        if (!queue.empty()) {
            std::cout << "Book ID: " << bookId << " Reservations:" << std::endl;
            // If 'queue' is std::queue, copy to a std::vector for iteration
            std::queue<Reservation> tempQueue = queue;
            while (!tempQueue.empty()) {
                const Reservation& reservation = tempQueue.front();
                if (reservation.isActive) {
                    std::cout << "  User ID: " << reservation.userId
                             << ", Reserved: " << reservation.reservationDate
                             << ", Expires: " << reservation.expiryDate << std::endl;
                    found = true;
                }
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

int LoanManager::daysBetween(const std::string& date1, const std::string& date2) const {
    // Simplified implementation - in production use a proper date library
    return date2 > date1 ? 1 : 0;
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

int LoanManager::getCurrentLoansCount(const User* user) const {
    if (!user) return 0;
    
    return std::count_if(transactions.begin(), transactions.end(),
        [user](const auto& transaction) {
            return transaction->userId == user->getUserId() && !transaction->isReturned;
        });
}
