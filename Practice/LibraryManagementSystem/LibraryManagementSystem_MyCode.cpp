#include<iostream>
#include<unordered_map>

using namespace std;

enum class BookStatus{
    AVAILABLE,
    ISSUED,
    RESERVED,
    LOST
};

enum class MemberType{
    STUDENT,
    FACULTY
};

class Book{
    private:
        string isbn;
        string title;
        string author;
        string publisher;
        string category;
    public:
        Book(string isbn, string title, string author, string publisher, string category){
            this->isbn = isbn;
            this->title = title;
            this->author = author;
            this->publisher = publisher;
            this->category = category;
        }
        string getISBN(){
            return this->isbn;
        }
        string getTitle(){
            return this->title;
        }
        string getAuthor(){
            return this->author;
        }
        string getPublisher(){
            return this->publisher;
        }
        string getCategory(){
            return this->category;
        }
};

class BookItem{
    private:
        string barcode;
        string rackNumber;
        Book* book;
        BookStatus status;
    public:
        BookItem(string barcode,string rackNumber,Book* book){
            this->barcode = barcode;
            this->rackNumber= rackNumber;
            this->book = book;
            this->status = BookStatus::AVAILABLE;
        }
        string getBarcode(){
            return this->barcode;
        }
        string getRackNumber(){
            return this->rackNumber;
        }
        Book* getBook(){
            return this->book;
        }
        BookStatus getStatus(){
            return this->status;
        }
        void setStatus(BookStatus status){
            this->status = status;
        }
};

class Member{
    protected:
        string memberId;
        string name;
        MemberType type;
        int maxBookLimit;
        int borrowedBooks;
    public:
        Member(string memberId, string name, MemberType type, int maxLimit){
            this->memberId = memberId;
            this->name = name;
            this->type = type;
            this->maxBookLimit = maxLimit;
            this->borrowedBooks = 0;
        }
        string getMemberId(){
            return this->memberId;
        }
        string getName(){
            return this->name;
        }
        MemberType getType(){
            return this->type;
        }
        int getBorrowedBooks(){
            return this->borrowedBooks;
        }
        bool canBorrowBook(){
            return borrowedBooks<maxBookLimit;
        }
        void incrementBorrowedBooks(){
            borrowedBooks++;
        }
        void decrementBorrowedBooks(){
            if(borrowedBooks>0) borrowedBooks--;
        }
};

class StudentMember:public Member{
    public:
        StudentMember(string memberId, string name):Member(memberId,name,MemberType::STUDENT,3){}
};

class FacultyMember:public Member{
    public:
        FacultyMember(string memberId, string name):Member(memberId,name,MemberType::FACULTY,5){}
};

class MemberFactory{
    public:
        static Member* createStudent(string memberId, string name){
            return new StudentMember(memberId,name);
        }
        static Member* createFaculty(string memberId, string name){
            return new FacultyMember(memberId,name);
        }
};

class Loan{
    private:
        string loanId;
        BookItem* bookItem;
        Member* member;

        time_t issueDate;
        time_t dueDate;
        time_t returnDate;
    public:
        Loan(string id, BookItem* item, Member* member){
            this->loanId=id;
            this->bookItem=item;
            this->member=member;
            this->issueDate=time(nullptr);
            this->dueDate=issueDate+(14*24*60*60);
            this->returnDate=0;
        }

        string getLoanId(){
            return this->loanId;
        }
        BookItem* getBookItem(){
            return this->bookItem;
        }
        Member* getMember(){
            return this->member;
        }
        time_t getIssueDate(){
            return this->issueDate;
        }
        time_t getDueDate(){
            return this->dueDate;
        }
        time_t getReturnDate(){
            return this->returnDate;
        }
        void makrReturned(){
            this->returnDate=time(nullptr);
        }
        void simulateLateReturn(int days){
            this->returnDate = dueDate + (days*24*60*60);
        }
};

class FineStrategy{
    public:
        virtual double calculateFine(Loan* loan) = 0;
};

class DailyFineStrategy:public FineStrategy{
    private:
        double dailyRate;
    public:
        DailyFineStrategy(double rate){
            this->dailyRate=rate;
        }
        double calculateFine(Loan* loan) override{
            if(loan->getReturnDate()==0) return 0;
            if(loan->getReturnDate()<=loan->getDueDate()) return 0;

            double fine = dailyRate*(difftime(loan->getReturnDate(),loan->getDueDate())/(24*60*60));
            return fine;
        };
};

class Library{
    private:
        unordered_map<string,Book*> books;
        unordered_map<string,BookItem*> bookItems;
        unordered_map<string,Member*> members;
        unordered_map<string,Loan*> activeLoans;
        FineStrategy* fineStrategy;
        int loanCounter;
    public:
        Library(FineStrategy* strategy){
            this->fineStrategy = strategy;
            this->loanCounter=0;
        }

        void addBook(Book* book){
            books[book->getISBN()]=book;
        }
        void addBookItem(BookItem* bookItem){
            // cout << bookItem->getBarcode() <<endl;
            bookItems[bookItem->getBarcode()]=bookItem;
        }
        void registerMember(Member* member){
            members[member->getMemberId()]=member;
        }
        void searchBookByTitle(string title){
            bool found = false;

            for(auto entry:books){
                auto book = entry.second;
                if(book->getTitle()==title){
                    found=true;
                    cout << "Found Book: "<<title<<endl;
                    break;
                }
            }
            if(!found){
                cout << "Book not found!" <<endl;
            }
        }

        Loan* issueBook(string memberId, string barcode){
            if(!members.count(memberId)){
                throw runtime_error("Invalid member!");
            }
            // for(auto item:bookItems){
            //     cout << item.first << endl;
            // }
            if(!bookItems.count(barcode)){
                throw runtime_error("Invalid barcode!");
            }

            auto member = members[memberId];

            if(!member->canBorrowBook()){
                throw runtime_error("Borrowing limit reached!");
            }
            auto bookItem = bookItems[barcode];

            if (bookItem->getStatus() != BookStatus::AVAILABLE){
                throw runtime_error("Book Not Available");
            }

            string loanId = "L"+to_string(++loanCounter);

            Loan* loan = new Loan(loanId,bookItem,member);

            member->incrementBorrowedBooks();
            bookItem->setStatus(BookStatus::ISSUED);
            activeLoans[barcode]=loan;
            return loan;
        }
        double returnBook(string barcode,bool simulateLate = false, int lateDays = 0){
            if(!activeLoans.count(barcode)){
                throw runtime_error("Loan not found!");
            }
            Loan* loan = activeLoans[barcode];
            auto bookItem = loan->getBookItem();
            if(simulateLate){
                loan->simulateLateReturn(lateDays);
            }else{
                loan->makrReturned();
            }
            double fine = fineStrategy->calculateFine(loan);

            loan->getBookItem()->setStatus(BookStatus::AVAILABLE);
            loan->getMember()->decrementBorrowedBooks();
            activeLoans.erase(barcode);

            delete loan;

            return fine;
        }
        void displayBooks(){

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
        ~Library(){

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