#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <ctime>
#include <stdexcept>

using namespace std;

enum class BookStatus
{
    AVAILABLE,
    ISSUED,
    RESERVED,
    LOST
};

enum class MemberType
{
    STUDENT,
    FACULTY
};

class Book
{
private:
    string isbn;
    string title;
    string author;
    string publisher;
    string category;

public:
    Book(const string &isbn,
         const string &title,
         const string &author,
         const string &publisher,
         const string &category)
        : isbn(isbn),
          title(title),
          author(author),
          publisher(publisher),
          category(category) {}

    string getISBN() const
    {
        return isbn;
    }

    string getTitle() const
    {
        return title;
    }

    string getAuthor() const
    {
        return author;
    }

    string getCategory() const
    {
        return category;
    }
};

class BookItem
{
private:
    string barcode;
    string rackNumber;
    Book *book;
    BookStatus status;

public:
    BookItem(const string &barcode,
             const string &rackNumber,
             Book *book)
        : barcode(barcode),
          rackNumber(rackNumber),
          book(book),
          status(BookStatus::AVAILABLE) {}

    string getBarcode() const
    {
        return barcode;
    }

    string getRackNumber() const
    {
        return rackNumber;
    }

    Book *getBook() const
    {
        return book;
    }

    BookStatus getStatus() const
    {
        return status;
    }

    void setStatus(BookStatus status)
    {
        this->status = status;
    }
};

class Member
{
protected:
    string memberId;
    string name;
    MemberType type;
    int maxBooksAllowed;
    int borrowedBooksCount;

public:
    Member(const string &memberId,
           const string &name,
           MemberType type,
           int maxBooksAllowed)
        : memberId(memberId),
          name(name),
          type(type),
          maxBooksAllowed(maxBooksAllowed),
          borrowedBooksCount(0) {}

    virtual ~Member() {}

    string getMemberId() const
    {
        return memberId;
    }

    string getName() const
    {
        return name;
    }

    int getBorrowedBooksCount() const
    {
        return borrowedBooksCount;
    }

    int getMaxBooksAllowed() const
    {
        return maxBooksAllowed;
    }

    bool canBorrowMoreBooks() const
    {
        return borrowedBooksCount < maxBooksAllowed;
    }

    void incrementBorrowedBooks()
    {
        borrowedBooksCount++;
    }

    void decrementBorrowedBooks()
    {
        if (borrowedBooksCount > 0)
        {
            borrowedBooksCount--;
        }
    }
};

class StudentMember : public Member
{
public:
    StudentMember(const string &id,
                  const string &name)
        : Member(id,
                 name,
                 MemberType::STUDENT,
                 3) {}
};

class FacultyMember : public Member
{
public:
    FacultyMember(const string &id,
                  const string &name)
        : Member(id,
                 name,
                 MemberType::FACULTY,
                 5) {}
};

class MemberFactory
{
public:
    static Member *createStudent(
        const string &id,
        const string &name)
    {

        return new StudentMember(id, name);
    }

    static Member *createFaculty(
        const string &id,
        const string &name)
    {

        return new FacultyMember(id, name);
    }
};

class Loan
{
private:
    string loanId;
    BookItem *bookItem;
    Member *member;

    time_t issueDate;
    time_t dueDate;
    time_t returnDate;

public:
    Loan(const string &loanId,
         BookItem *bookItem,
         Member *member)
        : loanId(loanId),
          bookItem(bookItem),
          member(member),
          returnDate(0)
    {

        issueDate = time(nullptr);

        dueDate =
            issueDate +
            (14 * 24 * 60 * 60);
    }

    string getLoanId() const
    {
        return loanId;
    }

    BookItem *getBookItem() const
    {
        return bookItem;
    }

    Member *getMember() const
    {
        return member;
    }

    time_t getIssueDate() const
    {
        return issueDate;
    }

    time_t getDueDate() const
    {
        return dueDate;
    }

    time_t getReturnDate() const
    {
        return returnDate;
    }

    void markReturned()
    {
        returnDate = time(nullptr);
    }

    void simulateLateReturn(int daysLate)
    {
        returnDate =
            dueDate +
            (daysLate * 24 * 60 * 60);
    }
};

class FineStrategy
{
public:
    virtual double calculateFine(
        const Loan &loan) = 0;

    virtual ~FineStrategy() {}
};

class DailyFineStrategy
    : public FineStrategy
{

private:
    double finePerDay;

public:
    DailyFineStrategy(
        double finePerDay = 10)
        : finePerDay(finePerDay) {}

    double calculateFine(
        const Loan &loan) override
    {

        if (loan.getReturnDate() == 0)
            return 0;

        if (loan.getReturnDate() <= loan.getDueDate())
            return 0;

        double lateDays =
            difftime(
                loan.getReturnDate(),
                loan.getDueDate()) /
            (24 * 60 * 60);

        return lateDays * finePerDay;
    }
};

class Library
{

private:
    unordered_map<string, Book *> books;

    unordered_map<
        string,
        BookItem *>
        bookItems;

    unordered_map<
        string,
        Member *>
        members;

    unordered_map<
        string,
        Loan *>
        activeLoans;

    FineStrategy *fineStrategy;

    int loanCounter;

public:
    Library(FineStrategy *strategy)
        : fineStrategy(strategy),
          loanCounter(1) {}

    void addBook(Book *book)
    {

        books[book->getISBN()] = book;
    }

    void addBookItem(
        BookItem *item)
    {

        bookItems[item->getBarcode()] = item;
    }

    void registerMember(
        Member *member)
    {

        members[member->getMemberId()] = member;
    }

    void searchBookByTitle(
        const string &title)
    {

        bool found = false;

        for (auto &entry : books)
        {

            Book *book =
                entry.second;

            if (book->getTitle() == title)
            {

                cout
                    << "Found Book: "
                    << book->getTitle()
                    << endl;

                found = true;
            }
        }

        if (!found)
        {

            cout
                << "Book not found"
                << endl;
        }
    }

    Loan *issueBook(
        const string &memberId,
        const string &barcode)
    {

        if (!members.count(memberId))
            throw runtime_error(
                "Invalid Member");

        if (!bookItems.count(barcode))
            throw runtime_error(
                "Invalid Book Copy");

        Member *member =
            members[memberId];

        if (!member
                 ->canBorrowMoreBooks())
            throw runtime_error(
                "Borrow Limit Reached");

        BookItem *item =
            bookItems[barcode];

        if (item->getStatus() != BookStatus::AVAILABLE)
            throw runtime_error(
                "Book Not Available");

        item->setStatus(
            BookStatus::ISSUED);

        string loanId =
            "L" +
            to_string(
                loanCounter++);

        Loan *loan =
            new Loan(
                loanId,
                item,
                member);

        activeLoans[barcode] = loan;

        member
            ->incrementBorrowedBooks();

        return loan;
    }

    double returnBook(
        const string &barcode,
        bool simulateLate = false,
        int lateDays = 0)
    {

        if (!activeLoans.count(barcode))
            throw runtime_error(
                "Loan Not Found");

        Loan *loan =
            activeLoans[barcode];

        if (simulateLate)
            loan->simulateLateReturn(
                lateDays);
        else
            loan->markReturned();

        loan->getBookItem()
            ->setStatus(
                BookStatus::AVAILABLE);

        loan->getMember()
            ->decrementBorrowedBooks();

        double fine =
            fineStrategy
                ->calculateFine(
                    *loan);

        activeLoans.erase(barcode);

        delete loan;

        return fine;
    }

    void displayBooks()
    {

        cout
            << "\nBooks In Library\n";

        for (auto &entry : books)
        {

            cout
                << entry.second
                       ->getTitle()
                << endl;
        }
    }

    ~Library()
    {

        for (auto &entry : books)
            delete entry.second;

        for (auto &entry : bookItems)
            delete entry.second;

        for (auto &entry : members)
            delete entry.second;

        for (auto &entry : activeLoans)
            delete entry.second;

        delete fineStrategy;
    }
};

int main()
{

    Library library(
        new DailyFineStrategy(10));

    Book *cleanCode =
        new Book(
            "ISBN001",
            "Clean Code",
            "Robert Martin",
            "Prentice Hall",
            "Programming");

    Book *designPatterns =
        new Book(
            "ISBN002",
            "Design Patterns",
            "GoF",
            "Pearson",
            "Programming");

    library.addBook(cleanCode);
    library.addBook(designPatterns);

    BookItem *copy1 =
        new BookItem(
            "BC001",
            "R1-S1",
            cleanCode);

    BookItem *copy2 =
        new BookItem(
            "BC002",
            "R1-S2",
            cleanCode);

    BookItem *copy3 =
        new BookItem(
            "BC003",
            "R2-S1",
            designPatterns);

    library.addBookItem(copy1);
    library.addBookItem(copy2);
    library.addBookItem(copy3);

    Member *student =
        MemberFactory ::createStudent(
            "M001",
            "Yogesh");

    Member *faculty =
        MemberFactory ::createFaculty(
            "M002",
            "John");

    library.registerMember(student);
    library.registerMember(faculty);

    library.displayBooks();

    library.searchBookByTitle(
        "Clean Code");

    cout
        << "\nIssuing Book...\n";

    Loan *loan =
        library.issueBook(
            "M001",
            "BC001");

    cout
        << "Loan ID: "
        << loan->getLoanId()
        << endl;

    cout
        << "\nReturning Book "
        << "(5 days late)...\n";

    double fine =
        library.returnBook(
            "BC001",
            true,
            5);

    cout
        << "Fine = "
        << fine
        << endl;

    return 0;
}