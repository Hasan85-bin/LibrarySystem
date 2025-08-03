#ifndef LOAN_MANAGER_H
#define LOAN_MANAGER_H

#include <vector>
#include <memory>
#include <map>
#include <queue>
#include <ctime>
#include "Book.h"
#include "User.h"

// Forward declarations
class Book;
class User;

// Loan transaction record
struct LoanTransaction {
    int transactionId;
    int userId;
    int bookId;
    std::string borrowDate;
    std::string dueDate;
    std::string returnDate;
    double fine;
    bool isReturned;
    
    LoanTransaction(int transId, int uId, int bId, const std::string& borrow, const std::string& due);
};

// Reservation record
struct Reservation {
    int userId;
    int bookId;
    std::string reservationDate;
    std::string expiryDate;
    bool isActive;
    
    Reservation(int uId, int bId, const std::string& date, const std::string& expiry);
};

// Fine calculation strategy (Strategy Pattern)
class FineCalculator {
public:

    virtual ~FineCalculator() = default;
    virtual double calculateFine(const std::string& dueDate, const std::string& returnDate) = 0;
    //helper
    time_t parseDate(const std::string& dateStr) const;
};

class StandardFineCalculator : public FineCalculator {
private:
    double dailyRate;
public:
    StandardFineCalculator(double rate = 1.0) : dailyRate(rate) {}
    double calculateFine(const std::string& dueDate, const std::string& returnDate) override;
};

class BookTypeFineCalculator : public FineCalculator {
private:
    std::map<std::string, double> rates;
public:
    BookTypeFineCalculator();
    double calculateFine(const std::string& dueDate, const std::string& returnDate) override;
    void setRate(const std::string& bookType, double rate);
};

// Main Loan Manager Class
class LoanManager {
private:
    std::vector<std::unique_ptr<LoanTransaction>> transactions;
    std::map<int, std::queue<Reservation>> reservations; // bookId -> queue of reservations
    std::unique_ptr<FineCalculator> fineCalculator;
    int nextTransactionId;
    
    // Helper methods
    std::string getCurrentDate() const;
    std::string calculateDueDate(int loanPeriod) const;
    bool isDateOverdue(const std::string& dueDate) const;
    bool canUserBorrowBook(const User* user, const Book* book) const;
    int getCurrentLoansCount(const User* user) const;
    
public:
    LoanManager();
    ~LoanManager() = default;
    
    // Core borrowing and returning functionality
    bool borrowBook(User* user, Book* book);
    bool returnBook(User* user, Book* book);
    
    // Reservation system
    bool reserveBook(User* user, Book* book);
    bool cancelReservation(User* user, Book* book);
    std::vector<Reservation> getBookReservations(int bookId) const;
    
    // Fine management
    double calculateUserFines(const User* user) const;
    bool payFine(User* user, double amount);
    
    // Transaction history
    std::vector<LoanTransaction*> getUserTransactions(int userId) const;
    std::vector<LoanTransaction*> getBookTransactions(int bookId) const;
    
    // Display and reporting methods
    void printUserLoans(int userId) const;
    void printUserReservations(int userId) const;
    void printAllLoans() const;
    void printAllReservations() const;
    void printOverdueBooks() const;
    
    // Statistics methods
    int getTotalLoans() const;
    int getActiveLoans() const;
    int getOverdueCount() const;
    double getTotalFines() const;
    std::vector<LoanTransaction*> getOverdueTransactions() const;
    
    // Utility methods
    void setFineCalculator(std::unique_ptr<FineCalculator> calculator);
    void printTransactionHistory(int userId) const;
};

#endif // LOAN_MANAGER_H 