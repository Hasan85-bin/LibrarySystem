#include <iostream>
#include <memory>
#include <vector>
#include "Core Classes/Book.h"
#include "Core Classes/User.h"
#include "Core Classes/LoanManager.h"

void printSeparator(const std::string& title) {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << " " << title << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

void testBookCreation() {
    printSeparator("TESTING BOOK CREATION");
    
    // Create different types of books
    auto textbook1 = std::make_unique<TextBook>(1, "Advanced C++ Programming", "John Doe", 
                                               "Programming", "2023-01-15", 450, "Graduate", "Computer Science");
    
    auto magazine1 = std::make_unique<Magazine>(2, "Tech Weekly", "Tech Publications", 
                                               "Technology", "2024-03-01", 50, 15);
    
    auto reference1 = std::make_unique<ReferenceBook>(3, "Library Management Handbook", "Jane Smith", 
                                                     "Reference", "2022-06-10", 300);
    
    auto textbook2 = std::make_unique<TextBook>(4, "Data Structures", "Alice Johnson", 
                                               "Computer Science", "2023-08-20", 380, "Undergraduate", "Computer Science");
    
    // Print book information
    std::cout << "Created books:" << std::endl;
    textbook1->printInfo();
    magazine1->printInfo();
    reference1->printInfo();
    textbook2->printInfo();
    
    // Test book status changes
    std::cout << "\nTesting book status changes:" << std::endl;
    std::cout << "Original status: ";
    textbook1->printInfo();
    
    textbook1->setStatus(BookStatus::Borrowed);
    std::cout << "After borrowing: ";
    textbook1->printInfo();
    
    textbook1->setStatus(BookStatus::Available);
    std::cout << "After returning: ";
    textbook1->printInfo();
}

void testUserCreation() {
    printSeparator("TESTING USER CREATION");
    
    // Create users
    auto regularUser1 = std::make_unique<RegularUser>(1, "john_doe", "password123");
    auto regularUser2 = std::make_unique<RegularUser>(2, "jane_smith", "password456");
    auto librarian1 = std::make_unique<Librarian>(3, "admin_lib", "admin123");
    
    // Print user information
    std::cout << "Created users:" << std::endl;
    std::cout << "User 1: " << regularUser1->getUsername() << " (" << regularUser1->getType() << ")" << std::endl;
    std::cout << "User 2: " << regularUser2->getUsername() << " (" << regularUser2->getType() << ")" << std::endl;
    std::cout << "Librarian: " << librarian1->getUsername() << " (" << librarian1->getType() << ")" << std::endl;
    
    // Test permissions
    std::cout << "\nTesting user permissions:" << std::endl;
    std::cout << "Regular User 1 can borrow: " << (regularUser1->canBorrow() ? "Yes" : "No") << std::endl;
    std::cout << "Regular User 1 borrow limit: " << regularUser1->getBorrowLimit() << std::endl;
    std::cout << "Regular User 1 loan period: " << regularUser1->getLoanPeriod() << " days" << std::endl;
    std::cout << "Regular User 1 can manage books: " << (regularUser1->canManageBooks() ? "Yes" : "No") << std::endl;
    
    std::cout << "Librarian can borrow: " << (librarian1->canBorrow() ? "Yes" : "No") << std::endl;
    std::cout << "Librarian borrow limit: " << librarian1->getBorrowLimit() << std::endl;
    std::cout << "Librarian can manage books: " << (librarian1->canManageBooks() ? "Yes" : "No") << std::endl;
    std::cout << "Librarian can view logs: " << (librarian1->canViewLogs() ? "Yes" : "No") << std::endl;
    
    // Test authentication
    std::cout << "\nTesting authentication:" << std::endl;
    std::cout << "User 1 correct password: " << (regularUser1->authenticate("password123") ? "Success" : "Failed") << std::endl;
    std::cout << "User 1 wrong password: " << (regularUser1->authenticate("wrongpass") ? "Success" : "Failed") << std::endl;
}

void testBorrowingAndReturning() {
    printSeparator("TESTING BORROWING AND RETURNING");
    
    // Create loan manager
    auto loanManager = std::make_unique<LoanManager>();
    
    // Create books and users
    auto textbook1 = std::make_unique<TextBook>(1, "Advanced C++ Programming", "John Doe", 
                                               "Programming", "2023-01-15", 450, "Graduate", "Computer Science");
    auto magazine1 = std::make_unique<Magazine>(2, "Tech Weekly", "Tech Publications", 
                                               "Technology", "2024-03-01", 50, 15);
    auto reference1 = std::make_unique<ReferenceBook>(3, "Library Management Handbook", "Jane Smith", 
                                                     "Reference", "2022-06-10", 300);
    
    auto regularUser1 = std::make_unique<RegularUser>(1, "john_doe", "password123");
    auto regularUser2 = std::make_unique<RegularUser>(2, "jane_smith", "password456");
    
    // Test borrowing
    std::cout << "Testing book borrowing:" << std::endl;
    loanManager->borrowBook(regularUser1.get(), textbook1.get());
    loanManager->borrowBook(regularUser1.get(), magazine1.get());
    
    // Try to borrow reference book (should fail)
    std::cout << "\nTrying to borrow reference book:" << std::endl;
    loanManager->borrowBook(regularUser1.get(), reference1.get());
    
    // Try to borrow more than limit
    auto textbook2 = std::make_unique<TextBook>(4, "Data Structures", "Alice Johnson", 
                                               "Computer Science", "2023-08-20", 380, "Undergraduate", "Computer Science");
    auto textbook3 = std::make_unique<TextBook>(5, "Algorithms", "Bob Wilson", 
                                               "Computer Science", "2023-09-15", 400, "Undergraduate", "Computer Science");
    auto textbook4 = std::make_unique<TextBook>(6, "Database Systems", "Carol Brown", 
                                               "Computer Science", "2023-10-01", 350, "Undergraduate", "Computer Science");
    auto textbook5 = std::make_unique<TextBook>(7, "Software Engineering", "David Lee", 
                                               "Computer Science", "2023-11-01", 420, "Undergraduate", "Computer Science");
    
    std::cout << "\nTesting borrowing limits:" << std::endl;
    loanManager->borrowBook(regularUser1.get(), textbook2.get());
    loanManager->borrowBook(regularUser1.get(), textbook3.get());
    loanManager->borrowBook(regularUser1.get(), textbook4.get());
    loanManager->borrowBook(regularUser1.get(), textbook5.get()); // Should fail (limit reached)
    
    // Test returning
    std::cout << "\nTesting book returning:" << std::endl;
    loanManager->returnBook(regularUser1.get(), textbook1.get());
    loanManager->returnBook(regularUser1.get(), magazine1.get());
    
    // Show transaction history
    loanManager->printTransactionHistory(1);
}

void testReservations() {
    printSeparator("TESTING RESERVATION SYSTEM");
    
    auto loanManager = std::make_unique<LoanManager>();
    
    // Create books and users
    auto textbook1 = std::make_unique<TextBook>(1, "Advanced C++ Programming", "John Doe", 
                                               "Programming", "2023-01-15", 450, "Graduate", "Computer Science");
    auto regularUser1 = std::make_unique<RegularUser>(1, "john_doe", "password123");
    auto regularUser2 = std::make_unique<RegularUser>(2, "jane_smith", "password456");
    auto regularUser3 = std::make_unique<RegularUser>(3, "bob_wilson", "password789");
    
    // Borrow the book first
    loanManager->borrowBook(regularUser1.get(), textbook1.get());
    
    // Test reservations
    std::cout << "Testing book reservations:" << std::endl;
    loanManager->reserveBook(regularUser2.get(), textbook1.get());
    loanManager->reserveBook(regularUser3.get(), textbook1.get());
    
    // Try to reserve again (should fail)
    std::cout << "\nTrying to reserve the same book again:" << std::endl;
    loanManager->reserveBook(regularUser2.get(), textbook1.get());
    
    // Try to reserve available book (should fail)
    auto textbook2 = std::make_unique<TextBook>(2, "Data Structures", "Alice Johnson", 
                                               "Computer Science", "2023-08-20", 380, "Undergraduate", "Computer Science");
    std::cout << "\nTrying to reserve available book:" << std::endl;
    loanManager->reserveBook(regularUser2.get(), textbook2.get());
    
    // Cancel reservation
    std::cout << "\nCancelling reservation:" << std::endl;
    loanManager->cancelReservation(regularUser2.get(), textbook1.get());
    
    // Return book and see reservation queue
    std::cout << "\nReturning book to test reservation queue:" << std::endl;
    loanManager->returnBook(regularUser1.get(), textbook1.get());
}

void testFineCalculation() {
    printSeparator("TESTING FINE CALCULATION");
    
    auto loanManager = std::make_unique<LoanManager>();
    
    // Create book and user
    auto textbook1 = std::make_unique<TextBook>(1, "Advanced C++ Programming", "John Doe", 
                                               "Programming", "2023-01-15", 450, "Graduate", "Computer Science");
    auto regularUser1 = std::make_unique<RegularUser>(1, "john_doe", "password123");
    
    // Borrow book
    loanManager->borrowBook(regularUser1.get(), textbook1.get());
    
    // Show current fines
    std::cout << "Current user fines: $" << regularUser1->getTotalFines() << std::endl;
    
    // Return book (should be on time)
    std::cout << "\nReturning book on time:" << std::endl;
    loanManager->returnBook(regularUser1.get(), textbook1.get());
    
    std::cout << "Fines after on-time return: $" << regularUser1->getTotalFines() << std::endl;
    
    // Test fine payment
    std::cout << "\nTesting fine payment:" << std::endl;
    regularUser1->addFine(25.0); // Add some fines manually for testing
    std::cout << "Fines after adding: $" << regularUser1->getTotalFines() << std::endl;
    
    loanManager->payFine(regularUser1.get(), 10.0);
    std::cout << "Fines after partial payment: $" << regularUser1->getTotalFines() << std::endl;
    
    loanManager->payFine(regularUser1.get(), 20.0); // Pay more than owed
    std::cout << "Fines after full payment: $" << regularUser1->getTotalFines() << std::endl;
}

void testStatistics() {
    printSeparator("TESTING STATISTICS AND REPORTING");
    
    auto loanManager = std::make_unique<LoanManager>();
    
    // Create multiple books and users
    auto textbook1 = std::make_unique<TextBook>(1, "Advanced C++ Programming", "John Doe", 
                                               "Programming", "2023-01-15", 450, "Graduate", "Computer Science");
    auto textbook2 = std::make_unique<TextBook>(2, "Data Structures", "Alice Johnson", 
                                               "Computer Science", "2023-08-20", 380, "Undergraduate", "Computer Science");
    auto magazine1 = std::make_unique<Magazine>(3, "Tech Weekly", "Tech Publications", 
                                               "Technology", "2024-03-01", 50, 15);
    
    auto regularUser1 = std::make_unique<RegularUser>(1, "john_doe", "password123");
    auto regularUser2 = std::make_unique<RegularUser>(2, "jane_smith", "password456");
    
    // Create some loan activity
    loanManager->borrowBook(regularUser1.get(), textbook1.get());
    loanManager->borrowBook(regularUser2.get(), textbook2.get());
    loanManager->borrowBook(regularUser1.get(), magazine1.get());
    
    // Return some books
    loanManager->returnBook(regularUser1.get(), textbook1.get());
    loanManager->returnBook(regularUser2.get(), textbook2.get());
    
    // Print statistics
    std::cout << "Library Statistics:" << std::endl;
    std::cout << "Total loans: " << loanManager->getTotalLoans() << std::endl;
    std::cout << "Active loans: " << loanManager->getActiveLoans() << std::endl;
    std::cout << "Overdue books: " << loanManager->getOverdueCount() << std::endl;
    std::cout << "Total fines: $" << loanManager->getTotalFines() << std::endl;
    
    // Print overdue books
    loanManager->printOverdueBooks();
    
    // Print transaction history for users
    loanManager->printTransactionHistory(1);
    loanManager->printTransactionHistory(2);
}

int main() {
    std::cout << "LIBRARY MANAGEMENT SYSTEM - COMPREHENSIVE TEST" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    try {
        // Test all major components
        testBookCreation();
        testUserCreation();
        testBorrowingAndReturning();
        testReservations();
        testFineCalculation();
        testStatistics();
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << " ALL TESTS COMPLETED SUCCESSFULLY!" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during testing: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
