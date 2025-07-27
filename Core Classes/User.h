#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include "Book.h"

// Enum for user roles
enum class UserRole {
    Regular,
    Librarian
};

// Enum for user status
enum class UserStatus {
    Active,
    Suspended
};

// Forward declaration for LoanRecord
struct LoanRecord {
    int bookId;
    std::string borrowDate;
    std::string dueDate;
    std::string returnDate;
    double fine;
};

// Base User class
class User {
protected:
    int userId;
    std::string username;
    std::string password;
    UserRole role;
    UserStatus status;
    std::vector<LoanRecord> loanHistory;
    double totalFines;

public:
    User(int userId, const std::string& username, const std::string& password, UserRole role);
    virtual ~User() = default;

    int getUserId() const;
    std::string getUsername() const;
    UserRole getRole() const;
    UserStatus getStatus() const;
    double getTotalFines() const;
    const std::vector<LoanRecord>& getLoanHistory() const;

    void setStatus(UserStatus newStatus);
    void addLoanRecord(const LoanRecord& record);
    void addFine(double amount);
    void payFine(double amount);

    virtual bool canBorrow() const = 0;
    virtual int getBorrowLimit() const = 0;
    virtual int getLoanPeriod() const = 0;
    virtual bool canReserve() const = 0;
    virtual bool canManageUsers() const = 0;
    virtual bool canManageBooks() const = 0;
    virtual bool canHandleFines() const = 0;
    virtual bool canViewLogs() const = 0;

    // Authentication (simple, for demonstration)
    bool authenticate(const std::string& password) const;

    // For type identification
    virtual std::string getType() const = 0;
};

// RegularUser class
class RegularUser : public User {
public:
    RegularUser(int userId, const std::string& username, const std::string& password);
    bool canBorrow() const override;
    int getBorrowLimit() const override;
    int getLoanPeriod() const override;
    bool canReserve() const override;
    bool canManageUsers() const override;
    bool canManageBooks() const override;
    bool canHandleFines() const override;
    bool canViewLogs() const override;
    std::string getType() const override;
};

// Librarian class
class Librarian : public User {
public:
    Librarian(int userId, const std::string& username, const std::string& password);
    bool canBorrow() const override;
    int getBorrowLimit() const override;
    int getLoanPeriod() const override;
    bool canReserve() const override;
    bool canManageUsers() const override;
    bool canManageBooks() const override;
    bool canHandleFines() const override;
    bool canViewLogs() const override;
    std::string getType() const override;
};

#endif // USER_H
