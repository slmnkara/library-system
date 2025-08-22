#include <iostream>
#include <map>
#include <fstream> // for file operations
#include <chrono> // for time
#include <iomanip> // for time format
#include <sstream> // for oss

constexpr int ADMIN_BORROWING_LIMIT = 15;
constexpr int CIVILIAN_BORROWING_LIMIT = 10;
constexpr int STUDENT_BORROWING_LIMIT = 5;
enum logLevel { INFO, WARNING, ERROR };
enum memberType { ADMIN, CIVILIAN, STUDENT };

class Logger {
private:
	Logger() {}
	std::string filename = "log.txt";

	void writeToFile(const std::string& message) {
		std::ofstream file(filename, std::ios::app);
		if (file.is_open())
			 file << message << "\n";
		else
			std::cout << "File could not opened.\n";
	}
public:
	Logger(const Logger&) = delete;
	Logger operator=(const Logger&) = delete;

	static Logger& getInstance() {
		static Logger instance;
		return instance;
	}

	std::string getTime() {
		time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::ostringstream oss;
		oss << std::put_time(localtime(&t), "%Y-%m-%d %H:%M:%S");
		return oss.str();
	}

	std::string levelToString(logLevel level) {
		switch (level) {
		case INFO: return "INFO";
		case WARNING: return "WARNING";
		case ERROR: return "ERROR";
		default: return "UNKNOWN";
		}
	}

	void log(logLevel level, const std::string& message, int memberID = -1, int bookID = -1) {
		std::ostringstream oss;
		oss << "time=" << getTime()
			<< " level=" << levelToString(level)
			<< " msg=" << "'" << message << "'";
		if (memberID != -1) oss << " memberID=" << memberID;
		if (bookID != -1) oss << " bookID=" << bookID;
		writeToFile(oss.str());
		// std::cout << oss.str() << "\n";
	}
};

class IDGenerator {
private:
	size_t newBookID;
	size_t newMemberID;
	IDGenerator() : newBookID(0), newMemberID(0) {}
public:
	// Prohibits copying and assignment
	IDGenerator(const IDGenerator&) = delete;
	IDGenerator operator=(const IDGenerator&) = delete;

	static IDGenerator& getInstance() {
		static IDGenerator instance;
		return instance;
	}

	// Getters
	size_t getNewBookID() { return ++newBookID; }
	size_t getNewMemberID() { return ++newMemberID; }
};

class Member {
private:
	size_t ID;
	std::string name;
	size_t age;
	int borrowedBooks = 0;
	size_t borrowingLimit;
public:
	Member(size_t ID, std::string name, size_t age, size_t borrowing_limit) {
		this->ID = ID;
		this->name = name;
		this->age = age;
		this->borrowingLimit = borrowing_limit;
	}

	// Getters
	size_t getID() const { return ID; }
	std::string getName() const { return name; }
	size_t getAge() const { return age; }
	int getBorrowedBooks() const { return borrowedBooks; }
	size_t getBorrowingLimit() const { return borrowingLimit; }
	std::string getMemberType() const {
		switch (borrowingLimit) {
		case ADMIN_BORROWING_LIMIT:
			return "Admin";
			break;
		case CIVILIAN_BORROWING_LIMIT:
			return "Civilian";
			break;
		case STUDENT_BORROWING_LIMIT:
			return "Student";
			break;
		default:
			return "Unknown";
			break;
		}
	}

	// Setters
	//void setBorrowedBooks(int borrowedBooks) { this->borrowedBooks = borrowedBooks; }
	void incrementBorrowedBooks() { this->borrowedBooks++; }
	void decrementBorrowedBooks() { this->borrowedBooks--; }

	void display() {
		std::cout << "Member Details:"
			<< "\n" << " ID: " << ID
			<< "\n" << " Name: " << name
			<< "\n" << " Age: " << age
			<< "\n" << " Borrowed Books: " << borrowedBooks
			<< "\n" << " Type: " << getMemberType()
			<< "\n";
	}
};

class Book {
private:
	size_t ID;
	std::string title;
	std::string author;
	std::string ISBN;
	std::string year;
	std::string category;
	bool available = true;
	int borrowerID = -1;
public:
	Book(size_t ID, std::string title, std::string author, std::string ISBN, std::string year, std::string category) {
		this->ID = ID;
		this->title = title;
		this->author = author;
		this->ISBN = ISBN;
		this->year = year;
		this->category = category;
	}

	// Getters
	size_t getID() const { return ID; }
	std::string getTitle() const { return title; }
	std::string getAuthor() const { return author; }
	std::string getISBN() const { return ISBN; }
	std::string getYear() const { return year; }
	std::string getCategory() const { return category; }
	bool getAvailable() const { return available; }
	int getBorrowerID() const { return borrowerID; }

	// Setters
	void setAvailable(bool available) { this->available = available; }
	void setBorrowerID(int borrowerID) { this->borrowerID = borrowerID; }

	void display() {
		std::cout << "Book Details:"
			<< "\n" << " ID: " << ID
			<< "\n" << " Title: " << title
			<< "\n" << " Author: " << author
			<< "\n" << " ISBN: " << ISBN
			<< "\n" << " Year: " << year
			<< "\n" << " Available: " << ((available) ? "Yes" : "No");
		if (!available) std::cout << "\n" << " Borrower ID: " << borrowerID << "\n";
	}
};

class Library {
private:
	std::map<size_t, Member> members;
	std::map<size_t, Book> books;
public:
	void addMember(memberType type, const std::string& name, size_t age) {
		size_t newID = IDGenerator::getInstance().getNewMemberID();
		switch (type) {
		case ADMIN:
			members.emplace(newID, Member(newID, name, age, ADMIN_BORROWING_LIMIT));
			Logger::getInstance().log(INFO, "New admin added.", newID);
			break;
		case CIVILIAN:
			members.emplace(newID, Member(newID, name, age, CIVILIAN_BORROWING_LIMIT));
			Logger::getInstance().log(INFO, "New civilian added.", newID);
			break;
		case STUDENT:
			members.emplace(newID, Member(newID, name, age, STUDENT_BORROWING_LIMIT));
			Logger::getInstance().log(INFO, "New student added.", newID);
			break;
		default:
			Logger::getInstance().log(ERROR, "Invalid member type.", newID);
			break;
		}
	}

	void addBook(const std::string& title, const std::string& author,
		const std::string& ISBN, const std::string& year, const std::string& category) {
		size_t newID = IDGenerator::getInstance().getNewBookID();
		books.emplace(newID, Book(newID, title, author, ISBN, year, category));
		Logger::getInstance().log(INFO, "New book added.", -1, newID);
	}

	int searchMemberByName(const std::string& targetName) {
		for (const auto& [key, value] : members) {
			if (value.getName() == targetName) {
				Logger::getInstance().log(INFO, "Member search by name successful.", value.getID());
				return value.getID();
			}
		}
		Logger::getInstance().log(ERROR, "Member search by name failed.");
		return -1;
	}

	std::string searchMemberByID(int targetID) {
		auto target = members.find(targetID);
		if (target != members.end()) {
			Logger::getInstance().log(INFO, "Member search by ID successful.", targetID);
			return target->second.getName();
		}
		else {
			Logger::getInstance().log(ERROR, "Member search by ID failed.");
			return "UNKNOWN";
		}
	}

	int searchBookByTitle(const std::string& targetTitle) {
		for (const auto& [key, value] : books) {
			if (value.getTitle() == targetTitle) {
				Logger::getInstance().log(INFO, "Book search by title successful.", -1, value.getID());
				return value.getID();
			}
		}
		Logger::getInstance().log(ERROR, "Book search by title failed.");
		return -1;
	}

	int searchBookByISBN(const std::string& targetISBN) {
		for (const auto& [key, value] : books) {
			if (value.getISBN() == targetISBN) {
				Logger::getInstance().log(INFO, "Book search by ISBN successful.", -1, value.getID());
				return value.getID();
			}
		}
		Logger::getInstance().log(ERROR, "Book search by ISBN failed.");
		return -1;
	}

	std::string searchBookByID(int targetID) {
		auto target = books.find(targetID);
		if (target != books.end()) {
			Logger::getInstance().log(INFO, "Book search by ID successful.", -1, targetID);
			return target->second.getTitle() + ", " + target->second.getAuthor();
		}
		else {
			Logger::getInstance().log(ERROR, "Book search by ID failed.");
			return "UNKNOWN";
		}
	}

	void deleteMember(int targetID) {
		if (searchMemberByID(targetID) != "UNKNOWN") {
			members.erase(targetID);
			// return the books, if there any
			for (auto& [key, value] : books) {
				if (value.getBorrowerID() == targetID) {
					value.setAvailable(true);
					value.setBorrowerID(-1);
				}
			}
			Logger::getInstance().log(INFO, "Member successfully deleted.", targetID);
			return;
		}
		else {
			Logger::getInstance().log(ERROR, "Failed to delete the member.");
		}
	}

	void deleteBook(int targetID) {
		if (searchBookByID(targetID) != "UNKNOWN") {
			auto targetBook = books.find(targetID);
			// if there is a borrower
			if (!targetBook->second.getAvailable()) {
				int targetBorrowerID = targetBook->second.getBorrowerID();
				auto targetBorrower = members.find(targetBorrowerID);

				// decrese borrowed books
				targetBorrower->second.decrementBorrowedBooks();
			}
			books.erase(targetID);
			Logger::getInstance().log(INFO, "Book successfully deleted.", -1, targetID);
			return;
		}
		else {
			Logger::getInstance().log(ERROR, "Failed to delete the book.");
		}
	}

	void listAllMembers() {
		if (!members.empty()) {
			// Init width values
			const size_t IDWidth = 5, nameWidth = 25, ageWidth = 5, borrowedWidth = 16, typeWidth = 10;
			// Title line
			std::cout << "\nMember list:\n"
				<< std::left
				<< std::setw(IDWidth) << "ID"
				<< std::setw(nameWidth) << "NAME"
				<< std::setw(ageWidth) << "AGE"
				<< std::setw(borrowedWidth) << "BORROWED BOOKS"
				<< std::setw(typeWidth) << "TYPE"
				<< "\n"
				// Seperator line
				<< std::setw(IDWidth) << std::string(2, '-')
				<< std::setw(nameWidth) << std::string(4, '-')
				<< std::setw(ageWidth) << std::string(3, '-')
				<< std::setw(borrowedWidth) << std::string(14, '-')
				<< std::setw(typeWidth) << std::string(4, '-')
				<< "\n";

			// List lines
			for (const auto& [key, value] : members) {
				std::cout
					<< std::setw(IDWidth) << value.getID()
					<< std::setw(nameWidth) << value.getName()
					<< std::setw(ageWidth) << value.getAge()
					<< std::setw(borrowedWidth) << value.getBorrowedBooks()
					<< std::setw(typeWidth) << value.getMemberType()
					<< "\n";
			}
			Logger::getInstance().log(INFO, "Members listed.");
		}
		else {
			Logger::getInstance().log(ERROR, "Members map empty.");
		}
	}

	void listAllBooks() {
		// Init width values
		const size_t IDWidth = 5, titleWidth = 25, authorWidth = 25,
			ISBNWidth = 12, yearWidth = 6, categoryWidth = 20, availableWidth = 10;
		// Title line
		if (!books.empty()) {
			std::cout << "\nBook list:\n"
				<< std::left
				<< std::setw(IDWidth) << "ID"
				<< std::setw(titleWidth) << "Title"
				<< std::setw(authorWidth) << "Author"
				<< std::setw(ISBNWidth) << "ISBN"
				<< std::setw(yearWidth) << "Year"
				<< std::setw(categoryWidth) << "Category"
				<< std::setw(availableWidth) << "Available"
				<< "\n"
				// Seperator line
				<< std::setw(IDWidth) << std::string(2, '-')
				<< std::setw(titleWidth) << std::string(5, '-')
				<< std::setw(authorWidth) << std::string(6, '-')
				<< std::setw(ISBNWidth) << std::string(4, '-')
				<< std::setw(yearWidth) << std::string(4, '-')
				<< std::setw(categoryWidth) << std::string(8, '-')
				<< std::setw(availableWidth) << std::string(9, '-')
				<< "\n";
			// List lines
			for (const auto& [key, value] : books) {
				std::cout
					<< std::setw(IDWidth) << value.getID()
					<< std::setw(titleWidth) << value.getTitle()
					<< std::setw(authorWidth) << value.getAuthor()
					<< std::setw(ISBNWidth) << value.getISBN()
					<< std::setw(yearWidth) << value.getYear()
					<< std::setw(categoryWidth) << value.getCategory()
					<< std::setw(availableWidth) << ((value.getAvailable())?"Yes":"No")
					<< "\n";
			}
			Logger::getInstance().log(INFO, "Books listed.");
		}
		else {
			Logger::getInstance().log(ERROR, "Books map empty.");
		}
	}

	void borrow_book(size_t targetBorrowerID, size_t targetBookID) {
		if (searchMemberByID(targetBorrowerID) != "UNKNOWN") {
			if (searchBookByID(targetBookID) != "UNKNOWN") {
				// Init target member and book
				auto targetMember = members.find(targetBorrowerID);
				auto targetBook = books.find(targetBookID);

				// Borrowing limit control
				if (targetMember->second.getBorrowingLimit() > targetMember->second.getBorrowedBooks()) {
					// Increasing number of borrowed books
					targetMember->second.incrementBorrowedBooks();

					// Setting availability and borrowerID of book
					targetBook->second.setAvailable(false);
					targetBook->second.setBorrowerID(targetBorrowerID);
					// Logging
					Logger::getInstance().log(INFO, "Book successfully borrowed.", targetBorrowerID, targetBookID);
				}
				else Logger::getInstance().log(ERROR, "Reached the limit for borrowing.", targetBorrowerID, targetBookID);
			}
			else Logger::getInstance().log(ERROR, "Invalid bookID at borrow function.");
		}
		else Logger::getInstance().log(ERROR, "Invalid memberID at borrow function.");
	}

	void return_book(size_t targetBookID) {
		if (searchBookByID(targetBookID) != "UNKNOWN") {
			auto targetBook = books.find(targetBookID);
			int targetBorrowerID = targetBook->second.getBorrowerID();
			auto targetBorrower = members.find(targetBorrowerID);
			// Decrease the number of borrowed books
			targetBorrower->second.decrementBorrowedBooks();
			// Make book available
			targetBook->second.setAvailable(true);
			targetBook->second.setBorrowerID(-1);
			Logger::getInstance().log(INFO, "Book successfully returned.", targetBorrowerID, targetBookID);
		}
		else Logger::getInstance().log(ERROR, "Invalid bookID at return function.");
	}
};

int main() {
	Library lib;

	// Add members
	lib.addMember(ADMIN, "Alice", 35);
	lib.addMember(CIVILIAN, "Bob", 28);
	lib.addMember(STUDENT, "Charlie", 20);

	// Add books
	lib.addBook("C++ Basics", "Bjarne Stroustrup", "1234567890", "1985", "Programming");
	lib.addBook("Data Structures", "Mark Allen Weiss", "0987654321", "2013", "Computer Science");

	// List members
	lib.listAllMembers();

	// List books
	lib.listAllBooks();

	// Borrow a book
	int bookID = lib.searchBookByTitle("C++ Basics");
	int memberID = lib.searchMemberByName("Bob");
	lib.borrow_book(memberID, bookID);

	// Display the list again
	lib.listAllBooks();
	lib.listAllMembers();

	// Return the book
	lib.return_book(bookID);

	// Display the list again
	lib.listAllBooks();
	lib.listAllMembers();

	// Delete a book and a member
	lib.deleteBook(bookID);
	lib.deleteMember(memberID);

	// Display the list again
	lib.listAllBooks();
	lib.listAllMembers();

	return 0;
}