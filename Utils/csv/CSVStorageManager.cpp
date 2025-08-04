#include "CSVStorageManager.h"
#include <fstream>
#include <sstream>
bool CSVStorageManager::saveReservations(const std::vector<Reservation>& reservations, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    // Header
    file << "UserId,BookId,ReservationDate,ExpiryDate\n";
    for (const auto& r : reservations) {
        file << r.userId << ","
             << r.bookId << ","
             << r.reservationDate << ","
             << r.expiryDate << "\n";
    }
    file.close();
    return true;
}

std::vector<Reservation> CSVStorageManager::loadReservations(const std::string& filename) {
    std::vector<Reservation> reservations;
    std::ifstream file(filename);
    if (!file.is_open()) return reservations;

    std::string line;
    std::getline(file, line); // header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> fields;
        while (std::getline(ss, item, ',')) {
            fields.push_back(item);
        }
        if (fields.size() < 4) continue;

        int userId = std::stoi(fields[0]);
        int bookId = std::stoi(fields[1]);
        std::string reservationDate = fields[2];
        std::string expiryDate = fields[3];

        reservations.emplace_back(userId, bookId, reservationDate, expiryDate);
    }
    file.close();
    return reservations;
}
#include "CSVStorageManager.h"
#include "../../Core Classes/LoanManager.h"
#include <fstream>
#include <sstream>
bool CSVStorageManager::saveLoanTransactions(const std::vector<std::unique_ptr<LoanTransaction>>& transactions, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    // Header
    file << "TransactionId,UserId,BookId,BorrowDate,DueDate,ReturnDate,Fine,IsReturned\n";
    for (const auto& t : transactions) {
        file << t->transactionId << ","
             << t->userId << ","
             << t->bookId << ","
             << t->borrowDate << ","
             << t->dueDate << ","
             << t->returnDate << ","
             << t->fine << ","
             << (t->isReturned ? "1" : "0") << "\n";
    }
    file.close();
    return true;
}

std::vector<std::unique_ptr<LoanTransaction>> CSVStorageManager::loadLoanTransactions(const std::string& filename) {
    std::vector<std::unique_ptr<LoanTransaction>> transactions;
    std::ifstream file(filename);
    if (!file.is_open()) return transactions;

    std::string line;
    std::getline(file, line); // header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> fields;
        while (std::getline(ss, item, ',')) {
            fields.push_back(item);
        }
        if (fields.size() < 8) continue;

        int transactionId = std::stoi(fields[0]);
        int userId = std::stoi(fields[1]);
        int bookId = std::stoi(fields[2]);
        std::string borrowDate = fields[3];
        std::string dueDate = fields[4];
        std::string returnDate = fields[5];
        double fine = std::stod(fields[6]);
        bool isReturned = (fields[7] == "1");

        auto transaction = std::make_unique<LoanTransaction>(transactionId, userId, bookId, borrowDate, dueDate);
        transaction->returnDate = returnDate;
        transaction->fine = fine;
        transaction->isReturned = isReturned;
        transactions.push_back(std::move(transaction));
    }
    file.close();
    return transactions;
}
#include "CSVStorageManager.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../../Core Classes/Book.h"
bool CSVStorageManager::saveBooks(const std::vector<std::unique_ptr<Book>>& books, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    // Header
    file << "Id,Title,Author,Category,PublicationDate,PageCount,Status,Type,AcademicLevel,Field,IssueNumber\n";
    for (const auto& book : books) {
        file << book->getId() << ","
             << book->getTitle() << ","
             << book->getAuthor() << ","
             << book->getCategory() << ","
             << book->getPublicationDate() << ","
             << book->getPageCount() << ","
             << static_cast<int>(book->getStatus()) << ","
             << book->getType() << ",";

        if (book->getType() == "TextBook") {
            const TextBook* tb = dynamic_cast<const TextBook*>(book.get());
            file << tb->getAcademicLevel() << "," << tb->getField() << ",";
            file << "" << "\n"; // IssueNumber خالی
        } else if (book->getType() == "Magazine") {
            const Magazine* mg = dynamic_cast<const Magazine*>(book.get());
            file << "" << "," << "" << "," << mg->getIssueNumber() << "\n";
        } else {
            // ReferenceBook یا Book معمولی
            file << "" << "," << "" << "," << "" << "\n";
        }
    }
    file.close();
    return true;
}

std::vector<std::unique_ptr<Book>> CSVStorageManager::loadBooks(const std::string& filename) {
    std::vector<std::unique_ptr<Book>> books;
    std::ifstream file(filename);
    if (!file.is_open()) return books;

    std::string line;
    std::getline(file, line); // header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> fields;
        while (std::getline(ss, item, ',')) {
            fields.push_back(item);
        }
        if (fields.size() < 8) continue;

        int id = std::stoi(fields[0]);
        std::string title = fields[1];
        std::string author = fields[2];
        std::string category = fields[3];
        std::string publicationDate = fields[4];
        int pageCount = std::stoi(fields[5]);
        BookStatus status = static_cast<BookStatus>(std::stoi(fields[6]));
        std::string type = fields[7];

        if (type == "TextBook" && fields.size() >= 10) {
            std::string academicLevel = fields[8];
            std::string field = fields[9];
            books.push_back(std::make_unique<TextBook>(id, title, author, category, publicationDate, pageCount, academicLevel, field, status));
        } else if (type == "Magazine" && fields.size() >= 11) {
            int issueNumber = fields[10].empty() ? 0 : std::stoi(fields[10]);
            books.push_back(std::make_unique<Magazine>(id, title, author, category, publicationDate, pageCount, issueNumber, status));
        } else if (type == "ReferenceBook") {
            books.push_back(std::make_unique<ReferenceBook>(id, title, author, category, publicationDate, pageCount));
        } else {
            // اگر نوع ناشناخته بود، کتاب را نادیده بگیر
            continue;
        }
    }
    file.close();
    return books;
}

bool CSVStorageManager::saveUsers(const std::vector<std::unique_ptr<User>>& users, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    // Header
    file << "UserId,Username,Password,Type,Status,TotalFines,BorrowLimit,LoanPeriod,CanManageUsers,CanManageBooks,CanHandleFines,CanViewLogs\n";
    for (const auto& user : users) {
        file << user->getUserId() << ","
             << user->getUsername() << ","
             << user->getPassword() << ","
             << user->getType() << ","
             << (user->getStatus() == UserStatus::Active ? "Active" : "Suspended") << ","
             << user->getTotalFines() << ","
             << user->getBorrowLimit() << ","
             << user->getLoanPeriod() << ","
             << (user->canManageUsers() ? "true" : "false") << ","
             << (user->canManageBooks() ? "true" : "false") << ","
             << (user->canHandleFines() ? "true" : "false") << ","
             << (user->canViewLogs() ? "true" : "false") << "\n";
    }
    file.close();
    return true;
}

std::vector<std::unique_ptr<User>> CSVStorageManager::loadUsers(const std::string& filename) {
    std::vector<std::unique_ptr<User>> users;
    std::ifstream file(filename);
    if (!file.is_open()) return users;

    std::string line;
    // خواندن هدر
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> fields;
        while (std::getline(ss, item, ',')) {
            fields.push_back(item);
        }
        if (fields.size() < 13 && fields.size() < 12) continue; // تعداد ستون‌ها

        int userId = std::stoi(fields[0]);
        std::string username = fields[1];
        std::string password = fields[2];
        std::string type = fields[3];
        // سایر فیلدها در صورت نیاز قابل استفاده‌اند

        if (type == "RegularUser") {
            users.push_back(std::make_unique<RegularUser>(userId, username, password));
        } else if (type == "Librarian") {
            users.push_back(std::make_unique<Librarian>(userId, username, password));
        }
        // اگر نوع دیگری اضافه شد، اینجا هندل شود
    }
    file.close();
    return users;
}

void CSVStorageManager::checkOrCreateCSVFile(const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "UserId,Username,Password,Type,Status,TotalFines,BorrowLimit,LoanPeriod,CanManageUsers,CanManageBooks,CanHandleFines,CanViewLogs\n";
            file.close();
        }
    }
}
