#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <memory>
#include <vector>
#include <ctime>
using namespace std;

// Enum for Book Status
enum class BookStatus {
    Available,
    Borrowed,
    Reserved,
    Lost,
    ReferenceOnly
};

// Base Book class
class Book {
protected:
    int id;
    string title;
    string author;
    string category;
    string publicationDate;
    int pageCount;
    BookStatus status;

public:
    Book(int id, const string& title, const string& author,
         const string& category, const string& publicationDate,
         int pageCount, BookStatus status = BookStatus::Available);
    virtual ~Book() = default;

    // Getters
    int getId() const;
    string getTitle() const;
    string getAuthor() const;
    string getCategory() const;
    string getPublicationDate() const;
    int getPageCount() const;
    BookStatus getStatus() const;

    // Setters
    void setStatus(BookStatus newStatus);
    void setTitle(const string& newTitle);
    void setAuthor(const string& newAuthor);
    void setCategory(const string& newCategory);
    void setPublicationDate(const string& newDate);
    void setPageCount(int newPageCount);

    // Polymorphic clone
    virtual std::unique_ptr<Book> clone() const = 0;

    // For displaying book info
    virtual void printInfo() const;

    // For type identification
    virtual string getType() const = 0;
};

// TextBook derived class
class TextBook : public Book {
    string academicLevel;
    string field;
public:
    TextBook(int id, const string& title, const string& author,
             const string& category, const string& publicationDate,
             int pageCount, const string& academicLevel, const string& field,
             BookStatus status = BookStatus::Available);
    string getAcademicLevel() const;
    string getField() const;
    void setAcademicLevel(const string& level);
    void setField(const string& field);
    std::unique_ptr<Book> clone() const override;
    void printInfo() const override;
    string getType() const override;
};

// Magazine derived class
class Magazine : public Book {
    int issueNumber;
public:
    Magazine(int id, const string& title, const string& author,
             const string& category, const string& publicationDate,
             int pageCount, int issueNumber,
             BookStatus status = BookStatus::Available);
    int getIssueNumber() const;
    void setIssueNumber(int issue);
    std::unique_ptr<Book> clone() const override;
    void printInfo() const override;
    string getType() const override;
};

// ReferenceBook derived class (non-borrowable)
class ReferenceBook : public Book {
public:
    ReferenceBook(int id, const string& title, const string& author,
                  const string& category, const string& publicationDate,
                  int pageCount);
    std::unique_ptr<Book> clone() const override;
    void printInfo() const override;
    string getType() const override;
};

#endif // BOOK_H 