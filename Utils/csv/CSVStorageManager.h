#pragma once
#include <string>
#include <vector>
#include "../../Core Classes/User.h"
#include "../../Core Classes/Book.h"
#include "../../Core Classes/LoanManager.h"

class CSVStorageManager {
public:
    // ذخیره کاربران در فایل CSV
    static bool saveUsers(const std::vector<std::unique_ptr<User>>& users, const std::string& filename);

    // خواندن کاربران از فایل CSV
    static std::vector<std::unique_ptr<User>> loadUsers(const std::string& filename);

    // بررسی وجود فایل و ساخت آن در صورت عدم وجود
    static void checkOrCreateCSVFile(const std::string& filename);

    // ذخیره کتاب‌ها در فایل CSV
    static bool saveBooks(const std::vector<std::unique_ptr<Book>>& books, const std::string& filename);

    // خواندن کتاب‌ها از فایل CSV
    static std::vector<std::unique_ptr<Book>> loadBooks(const std::string& filename);

    // ذخیره تراکنش‌های امانت در فایل CSV
    static bool saveLoanTransactions(const std::vector<std::unique_ptr<LoanTransaction>>& transactions, const std::string& filename);

    // خواندن تراکنش‌های امانت از فایل CSV
    static std::vector<std::unique_ptr<LoanTransaction>> loadLoanTransactions(const std::string& filename);
    
    // ذخیره رزروها در فایل CSV
    static bool saveReservations(const std::vector<Reservation>& reservations, const std::string& filename);

    // خواندن رزروها از فایل CSV
    static std::vector<Reservation> loadReservations(const std::string& filename);
};
