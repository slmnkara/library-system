#include <iostream>
#include <map>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

constexpr int ADMIN_BORROWING_LIMIT = 15;
constexpr int CIVILAN_BORROWING_LIMIT = 10;
constexpr int STUDENT_BORROWING_LIMIT = 5;
enum logLevel { INFO, WARNING, ERROR };
enum memberType { ADMIN, CIVILIAN, STUDENT };

class Logger {
private:
	Logger() {}
	std::string filename = "log.txt";

	void writeToFile(const std::string& message) {
		std::ofstream file(filename, std::ios::app);
		if (file.is_open()) {
			file << message << "\n";
		}
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
		case INFO:
			return "INFO";
			break;
		case WARNING:
			return "WARNING";
			break;
		case ERROR:
			return "ERROR";
			break;
		default:
			return "UNKNOWN";
			break;
		}
	}

	void log(logLevel level, std::string message) {
		std::ostringstream oss;
		oss << "time=" << getTime() << " level=" << levelToString(level) << " msg='" << message << "'";
		writeToFile(oss.str());
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
	size_t getNewBookID() { return newBookID++; }
	size_t getNewMemberID() { return newMemberID++; }
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
			<< "\n" << " Available: " << available;
		if (!available) std::cout << "\n" << " Borrower ID: " << borrowerID << "\n";
	}
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
		case CIVILAN_BORROWING_LIMIT:
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
	void setBorrowedBooks(int borrowedBooks) { this->borrowedBooks = borrowedBooks; }

	void display() {
		std::cout << "Member Details:"
			<< "\n" << " ID: " << ID
			<< "\n" << " Name: " << name
			<< "\n" << " Age: " << age
			<< "\n" << " Borrowed Books: " << borrowedBooks
			<< "\n";
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
			Logger::getInstance().log(INFO, "New admin added.");
			break;
		case CIVILIAN:
			members.emplace(newID, Member(newID, name, age, CIVILAN_BORROWING_LIMIT));
			Logger::getInstance().log(INFO, "New civilian added.");
			break;
		case STUDENT:
			members.emplace(newID, Member(newID, name, age, STUDENT_BORROWING_LIMIT));
			Logger::getInstance().log(INFO, "New student added.");
			break;
		default:
			Logger::getInstance().log(ERROR, "Invalid member type.");
			break;
		}
	}

	void addBook(const std::string& title, const std::string& author,
		const std::string& ISBN, const std::string& year, const std::string& category) {
		size_t newID = IDGenerator::getInstance().getNewBookID();
		books.emplace(newID, Book(newID, title, author, ISBN, year, category));
		Logger::getInstance().log(INFO, "New book added.");
	}

	int searchMemberByName(const std::string& targetName) {
		for (const auto& [key, value] : members) {
			if (value.getName() == targetName) {
				Logger::getInstance().log(INFO, "Member search by name successful.");
				return value.getID();
			}
		}
		Logger::getInstance().log(ERROR, "Member search by name failed.");
		return -1;
	}

	std::string searchMemberByID(int targetID) {
		auto target = members.find(targetID);
		if (target != members.end()) {
			Logger::getInstance().log(INFO, "Member search by ID successful.");
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
				Logger::getInstance().log(INFO, "Book search by title successful.");
				return value.getID();
			}
			else {
				Logger::getInstance().log(ERROR, "Book search by title failed.");
				return -1;
			}
		}
	}

	int searchBookByISBN(const std::string& targetISBN) {
		for (const auto& [key, value] : books) {
			if (value.getISBN() == targetISBN) {
				Logger::getInstance().log(INFO, "Book search by ISBN successful.");
				return value.getID();
			}
			else {
				Logger::getInstance().log(ERROR, "Book search by ISBN failed.");
				return -1;
			}
		}
	}

	std::string searchBookByID(int targetID) {
		auto target = books.find(targetID);
		if (target != books.end()) {
			Logger::getInstance().log(INFO, "Book search by ID successful.");
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
			Logger::getInstance().log(INFO, "Member successfully deleted.");
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
				int targetBorrowedBooks = targetBorrower->second.getBorrowedBooks();
				targetBorrower->second.setBorrowedBooks(--targetBorrowedBooks);
			}
			books.erase(targetID);
			Logger::getInstance().log(INFO, "Book successfully deleted.");
			return;
		}
		else {
			Logger::getInstance().log(ERROR, "Failed to delete the book.");
		}
	}

	void listMembers() {
		if (!members.empty()) {
			std::cout << std::left
				<< std::setw(5) << "ID"
				<< std::setw(30) << "Name"
				<< std::setw(5) << "Age"
				<< std::setw(20) << "Borrowed Books"
				<< "Type\n";
			for (const auto& [key, value] : members) {
				std::cout
					<< std::setw(5) << value.getID()
					<< std::setw(30) << value.getName()
					<< std::setw(5) << value.getAge()
					<< std::setw(20) << value.getBorrowedBooks()
					<< value.getMemberType()
					<< "\n";
			}
			Logger::getInstance().log(INFO, "Members listed.");
		}
		else {
			Logger::getInstance().log(ERROR, "Members map empty.");
		}
	}

	void listBooks() {
		if (!books.empty()) {
			std::cout << std::left
				<< std::setw(5) << "ID"
				<< std::setw(30) << "Title"
				<< std::setw(30) << "Author"
				<< std::setw(30) << "ISBN"
				<< std::setw(10) << "Year"
				<< std::setw(20) << "Category"
				<< "Available\n";
			for (const auto& [key, value] : books) {
				std::cout
					<< std::setw(5) << value.getID()
					<< std::setw(30) << value.getTitle()
					<< std::setw(30) << value.getAuthor()
					<< std::setw(30) << value.getISBN()
					<< std::setw(10) << value.getYear()
					<< std::setw(20) << value.getCategory()
					<< value.getAvailable()
					<< "\n";
			}
			Logger::getInstance().log(INFO, "Books listed.");
		}
		else {
			Logger::getInstance().log(ERROR, "Books map empty.");
		}
	}

	void borrow_book(size_t targetMemberID, size_t targetBookID) {
		if (searchMemberByID(targetMemberID) != "UNKNOWN") {
			if (searchBookByID(targetBookID) != "UNKNOWN") {
				// Init target member and book
				auto targetMember = members.find(targetMemberID);
				auto targetBook = books.find(targetBookID);

				// Borrowing limit control
				if (targetMember->second.getBorrowingLimit() > targetMember->second.getBorrowedBooks()) {
					// Setting number of borrowed books
					int targetBorrowedBooks = targetMember->second.getBorrowedBooks();
					targetMember->second.setBorrowedBooks(++targetBorrowedBooks);

					// Setting availability and borrowerID of book
					targetBook->second.setAvailable(false);
					targetBook->second.setBorrowerID(targetMemberID);
				}
				else Logger::getInstance().log(ERROR, "Reached the limit for borrowing.");
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
			int targetBorrowedBooks = targetBorrower->second.getBorrowedBooks();
			targetBorrower->second.setBorrowedBooks(--targetBorrowedBooks);
			// Make book available
			targetBook->second.setAvailable(true);
			targetBook->second.setBorrowerID(-1);
			Logger::getInstance().log(INFO, "Book successfully returned.");
		}
		else Logger::getInstance().log(ERROR, "Invalid bookID at return function.");
	}
};

int main() {

}