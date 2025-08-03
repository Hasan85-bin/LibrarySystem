#include <iostream>
#include <memory>
#include <vector>
#include <limits>
#include <iomanip>
#include <algorithm>
#include "Core Classes/Book.h"
#include "Core Classes/User.h"
#include "Core Classes/LoanManager.h"
#include "Utils/ini/GlobalConfiguration.h"
#include "Utils/ini/ConfigManager.h"

class LibrarySystem {
private:
    std::vector<std::unique_ptr<Book>> books;
    std::vector<std::unique_ptr<User>> users;
    std::unique_ptr<LoanManager> loanManager;
    User* currentUser;

    // Helper functions
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void waitForKey() {
        std::cout << "\nPress Enter to continue...";
        //causing two enters need for backing to menu
        // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }

    void displayHeader(const std::string& title) {
        clearScreen();
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << std::setw(25 + title.length()/2) << title << std::endl;
        std::cout << std::string(50, '=') << std::endl;
    }

    // Menu functions
    void showMainMenu() {
        while (true) {
            displayHeader("Library Management System");
            std::cout << "\n1. Login"
                     << "\n2. Register New User"
                     << "\n3. Exit"
                     << "\n\nChoice: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            switch (choice) {
                case 1: handleLogin(); break;
                case 2: handleRegistration(); break;
                case 3: return;
                default: std::cout << "Invalid choice. Please try again.\n";
            }
        }
    }

    void showUserMenu() {
        while (true) {
            displayHeader("User Menu - Welcome " + currentUser->getUsername());
            std::cout << "\n1. Search Books"
                     << "\n2. View Available Books"
                     << "\n3. Borrow Book"
                     << "\n4. Return Book"
                     << "\n5. Reserve Book"
                     << "\n6. View My Loans"
                     << "\n7. View My Reservations"
                     << "\n8. View My Fines"
                     << "\n9. Logout"
                     << "\n\nChoice: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            switch (choice) {
                case 1: searchBooks(); break;
                case 2: viewAvailableBooks(); break;
                case 3: borrowBook(); break;
                case 4: returnBook(); break;
                case 5: reserveBook(); break;
                case 6: viewMyLoans(); break;
                case 7: viewMyReservations(); break;
                case 8: viewMyFines(); break;
                case 9: return;
                default: std::cout << "Invalid choice. Please try again.\n";
            }
            waitForKey();
        }
    }

    void showLibrarianMenu() {
        while (true) {
            displayHeader("Librarian Menu - Welcome " + currentUser->getUsername());
            std::cout << "\n1. Add New Book"
                     << "\n2. Edit Book"
                     << "\n3. Remove Book"
                     << "\n4. View All Books"
                     << "\n5. Manage Users"
                     << "\n6. View All Loans"
                     << "\n7. View All Reservations"
                     << "\n8. Manage Fines"
                     << "\n9. Generate Reports"
                     << "\n10. System Settings"
                     << "\n11. Logout"
                     << "\n\nChoice: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            switch (choice) {
                case 1: addBook(); break;
                case 2: editBook(); break;
                case 3: removeBook(); break;
                case 4: viewAllBooks(); break;
                case 5: manageUsers(); break;
                case 6: viewAllLoans(); break;
                case 7: viewAllReservations(); break;
                case 8: manageFines(); break;
                case 9: generateReports(); break;
                case 10: systemSettings(); break;
                case 11: return;
                default: std::cout << "Invalid choice. Please try again.\n";
            }
            waitForKey();
        }
    }

    // Authentication functions
    void handleLogin() {
        std::string username, password;
        displayHeader("Login");
        
        std::cout << "\nUsername: ";
        std::getline(std::cin, username);
        std::cout << "Password: ";
        std::getline(std::cin, password);

        for (const auto& user : users) {
            if (user->getUsername() == username && user->authenticate(password)) {
                currentUser = user.get();
                if (dynamic_cast<Librarian*>(currentUser)) {
                    showLibrarianMenu();
                } else {
                    showUserMenu();
                }
                currentUser = nullptr;
                return;
            }
        }
        std::cout << "\nInvalid username or password.\n";
        waitForKey();
    }

    void handleRegistration() {
        std::string username, password;
        displayHeader("New User Registration");
        
        std::cout << "\nUsername: ";
        std::getline(std::cin, username);
        
        // Check if username already exists
        for (const auto& user : users) {
            if (user->getUsername() == username) {
                std::cout << "\nUsername already exists. Please choose another.\n";
                waitForKey();
                return;
            }
        }
        
        std::cout << "Password: ";
        std::getline(std::cin, password);

        int newId = users.size() + 1;
        users.push_back(std::make_unique<RegularUser>(newId, username, password));
        std::cout << "\nUser registered successfully!\n";
        waitForKey();
    }

    // Book management functions
    void addBook() {
        displayHeader("Add New Book");
        std::cout << "\nSelect book type:"
                 << "\n1. TextBook"
                 << "\n2. Magazine"
                 << "\n3. Reference Book"
                 << "\n\nChoice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        // Common book properties
        std::string title, author, category, publicationDate;
        int pageCount;

        std::cout << "\nEnter book details:\n";
        std::cout << "Title: ";
        std::getline(std::cin, title);
        std::cout << "Author: ";
        std::getline(std::cin, author);
        std::cout << "Category: ";
        std::getline(std::cin, category);
        std::cout << "Publication Date (YYYY-MM-DD): ";
        std::getline(std::cin, publicationDate);
        std::cout << "Page Count: ";
        std::cin >> pageCount;
        std::cin.ignore();

        int id = books.size() + 1; // Simple ID generation

        switch (choice) {
            case 1: {
                std::string academicLevel, field;
                std::cout << "Academic Level: ";
                std::getline(std::cin, academicLevel);
                std::cout << "Field: ";
                std::getline(std::cin, field);
                books.push_back(std::make_unique<TextBook>(id, title, author, category, 
                    publicationDate, pageCount, academicLevel, field));
                break;
            }
            case 2: {
                int issueNumber;
                std::cout << "Issue Number: ";
                std::cin >> issueNumber;
                books.push_back(std::make_unique<Magazine>(id, title, author, category, 
                    publicationDate, pageCount, issueNumber));
                break;
            }
            case 3: {
                books.push_back(std::make_unique<ReferenceBook>(id, title, author, category, 
                    publicationDate, pageCount));
                break;
            }
            default:
                std::cout << "Invalid book type selected.\n";
                return;
        }

        std::cout << "\nBook added successfully!\n";
    }

    void viewAllBooks() {
        displayHeader("All Books");
        if (books.empty()) {
            std::cout << "\nNo books in the library.\n";
            return;
        }

        for (const auto& book : books) {
            book->printInfo();
            std::cout << std::string(50, '-') << std::endl;
        }
    }

    void viewAvailableBooks() {
        displayHeader("Available Books");
        bool found = false;
        
        for (const auto& book : books) {
            if (book->getStatus() == BookStatus::Available) {
                book->printInfo();
                std::cout << std::string(50, '-') << std::endl;
                found = true;
            }
        }

        if (!found) {
            std::cout << "\nNo available books found.\n";
        }
    }

    void searchBooks() {
        displayHeader("Search Books");
        std::cout << "\n1. Search by Title"
                 << "\n2. Search by Author"
                 << "\n3. Search by Category"
                 << "\n\nChoice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        std::string searchTerm;
        std::cout << "Enter search term: ";
        std::getline(std::cin, searchTerm);

        bool found = false;
        for (const auto& book : books) {
            bool match = false;
            switch (choice) {
                case 1: match = (book->getTitle().find(searchTerm) != std::string::npos); break;
                case 2: match = (book->getAuthor().find(searchTerm) != std::string::npos); break;
                case 3: match = (book->getCategory().find(searchTerm) != std::string::npos); break;
            }
            if (match) {
                book->printInfo();
                std::cout << std::string(50, '-') << std::endl;
                found = true;
            }
        }

        if (!found) {
            std::cout << "\nNo matching books found.\n";
        }
    }

    void editBook() {
        displayHeader("Edit Book");
        viewAllBooks();
        
        int id;
        std::cout << "\nEnter book ID to edit (0 to cancel): ";
        std::cin >> id;
        std::cin.ignore();

        if (id == 0) return;

        auto it = std::find_if(books.begin(), books.end(),
            [id](const auto& book) { return book->getId() == id; });

        if (it == books.end()) {
            std::cout << "Book not found.\n";
            return;
        }

        std::cout << "\nCurrent book details:\n";
        (*it)->printInfo();

        std::cout << "\nWhat would you like to edit?"
                 << "\n1. Title"
                 << "\n2. Author"
                 << "\n3. Category"
                 << "\n4. Publication Date"
                 << "\n5. Page Count"
                 << "\n6. Status"
                 << "\n0. Cancel"
                 << "\n\nChoice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        std::string newValue;
        switch (choice) {
            case 1:
                std::cout << "New title: ";
                std::getline(std::cin, newValue);
                (*it)->setTitle(newValue);
                break;
            case 2:
                std::cout << "New author: ";
                std::getline(std::cin, newValue);
                (*it)->setAuthor(newValue);
                break;
            case 3:
                std::cout << "New category: ";
                std::getline(std::cin, newValue);
                (*it)->setCategory(newValue);
                break;
            case 4:
                std::cout << "New publication date (YYYY-MM-DD): ";
                std::getline(std::cin, newValue);
                (*it)->setPublicationDate(newValue);
                break;
            case 5:
                int newPages;
                std::cout << "New page count: ";
                std::cin >> newPages;
                (*it)->setPageCount(newPages);
                break;
            case 6:
                std::cout << "New status:"
                         << "\n1. Available"
                         << "\n2. Borrowed"
                         << "\n3. Reserved"
                         << "\n4. Lost"
                         << "\nChoice: ";
                int statusChoice;
                std::cin >> statusChoice;
                switch (statusChoice) {
                    case 1: (*it)->setStatus(BookStatus::Available); break;
                    case 2: (*it)->setStatus(BookStatus::Borrowed); break;
                    case 3: (*it)->setStatus(BookStatus::Reserved); break;
                    case 4: (*it)->setStatus(BookStatus::Lost); break;
                    default: std::cout << "Invalid status choice.\n"; return;
                }
                break;
            case 0:
                return;
            default:
                std::cout << "Invalid choice.\n";
                return;
        }

        std::cout << "\nBook updated successfully!\n";
    }

    void removeBook() {
        displayHeader("Remove Book");
        viewAllBooks();
        
        int id;
        std::cout << "\nEnter book ID to remove (0 to cancel): ";
        std::cin >> id;
        std::cin.ignore();

        if (id == 0) return;

        auto it = std::find_if(books.begin(), books.end(),
            [id](const auto& book) { return book->getId() == id; });

        if (it == books.end()) {
            std::cout << "Book not found.\n";
            return;
        }

        books.erase(it);
        std::cout << "\nBook removed successfully!\n";
    }

    void borrowBook() {
        displayHeader("Borrow Book");
        viewAvailableBooks();

        int id;
        std::cout << "\nEnter book ID to borrow (0 to cancel): ";
        std::cin >> id;
        std::cin.ignore();

        if (id == 0) return;

        auto it = std::find_if(books.begin(), books.end(),
            [id](const auto& book) { return book->getId() == id; });

        if (it == books.end()) {
            std::cout << "Book not found.\n";
            return;
        }

        if (loanManager->borrowBook(currentUser, it->get())) {
            std::cout << "\nBook borrowed successfully!\n";
        } else {
            std::cout << "\nCould not borrow book. Please check your borrowing limits or book availability.\n";
        }
    }

    void returnBook() {
        displayHeader("Return Book");
        
        // Show user's borrowed books
        bool found = false;
        for (const auto& book : books) {
            if (book->getStatus() == BookStatus::Borrowed) {
                book->printInfo();
                std::cout << std::string(50, '-') << std::endl;
                found = true;
            }
        }

        if (!found) {
            std::cout << "\nYou have no books to return.\n";
            return;
        }

        int id;
        std::cout << "\nEnter book ID to return (0 to cancel): ";
        std::cin >> id;
        std::cin.ignore();

        if (id == 0) return;

        auto it = std::find_if(books.begin(), books.end(),
            [id](const auto& book) { return book->getId() == id; });

        if (it == books.end()) {
            std::cout << "Book not found.\n";
            return;
        }

        if (loanManager->returnBook(currentUser, it->get())) {
            std::cout << "\nBook returned successfully!\n";
        } else {
            std::cout << "\nCould not return book. Please check if you actually borrowed this book.\n";
        }
    }

    void reserveBook() {
        displayHeader("Reserve Book");
        // Show books that can be reserved (currently borrowed)
        bool found = false;
        for (const auto& book : books) {
            if (book->getStatus() == BookStatus::Borrowed) {
                book->printInfo();
                std::cout << std::string(50, '-') << std::endl;
                found = true;
            }
        }

        if (!found) {
            std::cout << "\nNo books available for reservation.\n";
            return;
        }

        int id;
        std::cout << "\nEnter book ID to reserve (0 to cancel): ";
        std::cin >> id;
        std::cin.ignore();

        if (id == 0) return;

        auto it = std::find_if(books.begin(), books.end(),
            [id](const auto& book) { return book->getId() == id; });

        if (it == books.end()) {
            std::cout << "Book not found.\n";
            return;
        }

        if (loanManager->reserveBook(currentUser, it->get())) {
            std::cout << "\nBook reserved successfully!\n";
        } else {
            std::cout << "\nCould not reserve book. The book might not be available for reservation.\n";
        }
    }

    void viewMyLoans() {
        displayHeader("My Loans");
        loanManager->printUserLoans(currentUser->getUserId());
    }

    void viewMyReservations() {
        displayHeader("My Reservations");
        loanManager->printUserReservations(currentUser->getUserId());
    }

    void viewMyFines() {
        displayHeader("My Fines");
        std::cout << "Your current fines: $" << currentUser->getTotalFines() << std::endl;
    }

    void manageUsers() {
        displayHeader("Manage Users");
        for (const auto& user : users) {
            std::cout << "ID: " << user->getUserId()
                     << ", Username: " << user->getUsername()
                     << ", Type: " << user->getType() << std::endl;
        }
    }

    void viewAllLoans() {
        displayHeader("All Loans");
        loanManager->printAllLoans();
    }

    void viewAllReservations() {
        displayHeader("All Reservations");
        loanManager->printAllReservations();
    }

    void manageFines() {
        displayHeader("Manage Fines");
        manageUsers();
        
        int userId;
        std::cout << "\nEnter user ID to manage fines (0 to cancel): ";
        std::cin >> userId;
        std::cin.ignore();

        if (userId == 0) return;

        auto it = std::find_if(users.begin(), users.end(),
            [userId](const auto& user) { return user->getUserId() == userId; });

        if (it == users.end()) {
            std::cout << "User not found.\n";
            return;
        }

        std::cout << "Current fines: $" << (*it)->getTotalFines() << std::endl;
        
        double amount;
        std::cout << "Enter amount to waive (0 to cancel): $";
        std::cin >> amount;

        if (amount <= 0) return;

        if (loanManager->payFine(it->get(), amount)) {
            std::cout << "\nFines waived successfully!\n";
        } else {
            std::cout << "\nError waiving fines.\n";
        }
    }

    void generateReports() {
        displayHeader("Generate Reports");
        
        std::cout << "\nLibrary Statistics:"
                 << "\n-------------------"
                 << "\nTotal books: " << books.size()
                 << "\nTotal users: " << users.size()
                 << "\nTotal loans: " << loanManager->getTotalLoans()
                 << "\nActive loans: " << loanManager->getActiveLoans()
                 << "\nOverdue books: " << loanManager->getOverdueCount()
                 << "\nTotal fines: $" << loanManager->getTotalFines()
                 << std::endl;

        std::cout << "\nOverdue Books:\n";
        loanManager->printOverdueBooks();

        std::cout << "\nMost Popular Books:\n";
        // TODO: Implement most popular books report
    }

    void systemSettings() {
        displayHeader("System Settings");
        std::cout << "\n1. Fine Rate Settings"
                 << "\n2. Loan Period Settings"
                 << "\n3. Reservation Settings"
                 << "\n4. Backup Settings"
                 << "\n5. Librarian Settings"
                 << "\n6. Return to Main Menu"
                 << "\n\nChoice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1: {
                std::cout << "Current fine rate per day: $" << daily_fine_rate << std::endl;
                std::cout << "Enter new fine rate per day: $";
                double newRate;
                std::cin >> newRate;
                std::cin.ignore();
                if (newRate > 0) {
                    daily_fine_rate = newRate;
                    ConfigManager::saveConfig();
                    std::cout << "Fine rate updated successfully.\n";
                } else {
                    std::cout << "Invalid value. No changes made.\n";
                }
                break;
            }
            case 2: {
                std::cout << "Current loan period (days): " << regular_user_loan_period << std::endl;
                std::cout << "Enter new loan period (days): ";
                int newPeriod;
                std::cin >> newPeriod;
                std::cin.ignore();
                if (newPeriod > 0) {
                    regular_user_loan_period = newPeriod;
                    ConfigManager::saveConfig();
                    std::cout << "Loan period updated successfully.\n";
                } else {
                    std::cout << "Invalid value. No changes made.\n";
                }
                break;
            }
            case 3: {
                std::cout << "Current max reservations per user: " << regular_user_borrow_limit << std::endl;
                std::cout << "Enter new max reservations per user: ";
                int newMax;
                std::cin >> newMax;
                std::cin.ignore();
                if (newMax > 0) {
                    regular_user_borrow_limit = newMax;
                    ConfigManager::saveConfig();
                    std::cout << "Reservation limit updated successfully.\n";
                } else {
                    std::cout << "Invalid value. No changes made.\n";
                }
                break;
            }
            case 4:
                // TODO: Implement backup settings
                std::cout << "Feature not yet implemented.\n";
                break;
            case 5: {
                std::cout << "Current librarian borrow limit: " << librarian_borrow_limit << std::endl;
                std::cout << "Enter new librarian borrow limit: ";
                int newLimit;
                std::cin >> newLimit;
                std::cin.ignore();
                if (newLimit > 0) {
                    librarian_borrow_limit = newLimit;
                    ConfigManager::saveConfig();
                    std::cout << "Librarian borrow limit updated successfully.\n";
                } else {
                    std::cout << "Invalid value. No changes made.\n";
                }

                std::cout << "Current librarian loan period (days): " << librarian_loan_period << std::endl;
                std::cout << "Enter new librarian loan period (days): ";
                int newPeriod;
                std::cin >> newPeriod;
                std::cin.ignore();
                if (newPeriod > 0) {
                    librarian_loan_period = newPeriod;
                    ConfigManager::saveConfig();
                    std::cout << "Librarian loan period updated successfully.\n";
                } else {
                    std::cout << "Invalid value. No changes made.\n";
                }
                break;
            }
            case 6:
                return;
            default:
                std::cout << "Invalid choice.\n";
                break;
        }
    }

public:
    LibrarySystem() : loanManager(std::make_unique<LoanManager>()), currentUser(nullptr) {
        // Add a default librarian account
        users.push_back(std::make_unique<Librarian>(1, "admin", "admin123"));
    }

    void run() {
        showMainMenu();
    }
};

int main() {
    try {
        LibrarySystem library;
        library.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
