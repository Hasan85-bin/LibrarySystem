#include "Book.h"
#include <iostream>

// Book Base Class Implementation
Book::Book(int id, const std::string& title, const std::string& author,
           const std::string& category, const std::string& publicationDate,
           int pageCount, BookStatus status)
    : id(id), title(title), author(author), category(category),
      publicationDate(publicationDate), pageCount(pageCount), status(status) {}

int Book::getId() const { return id; }
std::string Book::getTitle() const { return title; }
std::string Book::getAuthor() const { return author; }
std::string Book::getCategory() const { return category; }
std::string Book::getPublicationDate() const { return publicationDate; }
int Book::getPageCount() const { return pageCount; }
BookStatus Book::getStatus() const { return status; }

void Book::setStatus(BookStatus newStatus) { status = newStatus; }
void Book::setTitle(const std::string& newTitle) { title = newTitle; }
void Book::setAuthor(const std::string& newAuthor) { author = newAuthor; }
void Book::setCategory(const std::string& newCategory) { category = newCategory; }
void Book::setPublicationDate(const std::string& newDate) { publicationDate = newDate; }
void Book::setPageCount(int newPageCount) { pageCount = newPageCount; }

void Book::printInfo() const {
    std::cout << "[" << getType() << "] ID: " << id
              << ", Title: " << title
              << ", Author: " << author
              << ", Category: " << category
              << ", Published: " << publicationDate
              << ", Pages: " << pageCount
              << ", Status: ";
    switch (status) {
        case BookStatus::Available: std::cout << "Available"; break;
        case BookStatus::Borrowed: std::cout << "Borrowed"; break;
        case BookStatus::Reserved: std::cout << "Reserved"; break;
        case BookStatus::Lost: std::cout << "Lost"; break;
        case BookStatus::ReferenceOnly: std::cout << "Reference Only"; break;
    }
    std::cout << std::endl;
}

// TextBook Implementation
TextBook::TextBook(int id, const std::string& title, const std::string& author,
                   const std::string& category, const std::string& publicationDate,
                   int pageCount, const std::string& academicLevel, const std::string& field,
                   BookStatus status)
    : Book(id, title, author, category, publicationDate, pageCount, status),
      academicLevel(academicLevel), field(field) {}

std::string TextBook::getAcademicLevel() const { return academicLevel; }
std::string TextBook::getField() const { return field; }
void TextBook::setAcademicLevel(const std::string& level) { academicLevel = level; }
void TextBook::setField(const std::string& f) { field = f; }
std::unique_ptr<Book> TextBook::clone() const { return std::make_unique<TextBook>(*this); }
void TextBook::printInfo() const {
    Book::printInfo();
    std::cout << "  Academic Level: " << academicLevel << ", Field: " << field << std::endl;
}
std::string TextBook::getType() const { return "TextBook"; }

// Magazine Implementation
Magazine::Magazine(int id, const std::string& title, const std::string& author,
                   const std::string& category, const std::string& publicationDate,
                   int pageCount, int issueNumber, BookStatus status)
    : Book(id, title, author, category, publicationDate, pageCount, status),
      issueNumber(issueNumber) {}

int Magazine::getIssueNumber() const { return issueNumber; }
void Magazine::setIssueNumber(int issue) { issueNumber = issue; }
std::unique_ptr<Book> Magazine::clone() const { return std::make_unique<Magazine>(*this); }
void Magazine::printInfo() const {
    Book::printInfo();
    std::cout << "  Issue Number: " << issueNumber << std::endl;
}
std::string Magazine::getType() const { return "Magazine"; }

// ReferenceBook Implementation
ReferenceBook::ReferenceBook(int id, const std::string& title, const std::string& author,
                             const std::string& category, const std::string& publicationDate,
                             int pageCount)
    : Book(id, title, author, category, publicationDate, pageCount, BookStatus::ReferenceOnly) {}

std::unique_ptr<Book> ReferenceBook::clone() const { return std::make_unique<ReferenceBook>(*this); }
void ReferenceBook::printInfo() const {
    Book::printInfo();
    std::cout << "  (Reference Book - Not Borrowable)" << std::endl;
}
std::string ReferenceBook::getType() const { return "ReferenceBook"; } 