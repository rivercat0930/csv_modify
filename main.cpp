/*
* Program Brief Introduction:
*	This C++ program first checks if the target CSV file exists before proceeding to the next step.
*	If needed, you can set the CREATE_DEFAULT_DATA part to true.
*	Next, it will scan the CSV data and then output it, modify a value, and then output the modified file.
*
* Notes:
*	The IDE used is Visual Studio 2022, adopting the ISO C++20 Standard, on a Windows 10 environment, version => 10.0.19045 Build 19045.
*
* Usage Rights:
*	This program is shared for classroom homework use.
*	You may modify the code as you wish, but the source must be credited => https://github.com/rivercat0930
*/

#include <filesystem>
#include <format>
#include <iostream>
#include <locale>
#include <string>
#include <vector>
#include <Windows.h>

/* CSV2 */
/* https://github.com/p-ranav/csv2?tab=readme-ov-file#generating-single-header */
#include "./csv2/csv2.hpp"

/* If you need default data then change me */
#define CREATE_DEFAULT_DATA true
/* Give the path to the target csv file */
#define CSV_FILE_NAME_OR_PATH "./score.csv"

class Student {
public:
	Student(std::string& name, int chinese, int english, int math)
		: name(name), chineseScore(chinese), englishScore(english), mathScore(math) {}

	const std::string getName() { 
		return name;
	}
	int getChineseScore() {
		return chineseScore;
	}
	int getEnglishScore() {
		return englishScore;
	}
	int getMathScore() {
		return mathScore;
	}

	void setChineseScore(int score) {
		chineseScore = score;
	}
	void setEnglishScore(int score) {
		englishScore = score;
	}
	void setMathScore(int score) {
		mathScore = score; 
	}

private:
	std::string name;
	int chineseScore;
	int englishScore;
	int mathScore;
};

/*
* Write default data
*/
void createDefaultData() {
	std::ofstream csv(CSV_FILE_NAME_OR_PATH);
	csv2::Writer<csv2::delimiter<','>> writer(csv);

	std::vector<std::vector<std::string>> rows =
	{
		{"姓名", "國文成績", "英文成績", "數學成績"},
		{"Charlie_Chiang", "100", "100", "100"},
		{"Lily_Wu", "90", "80", "70"},
		{"Franky_Chen", "0", "0", "0"},
		{"Wesley_Lu", "0", "8", "7"}
	};

	writer.write_rows(rows);
	csv.close();
}

/*
* Read the csv file,
* then return a vector of student class
*/
std::vector<Student> readData(bool printTitle) {
	std::vector<Student> students;
	csv2::Reader<
		csv2::delimiter<','>,
		csv2::quote_character<'"'>,
		csv2::first_row_is_header<true>
	> csv;

	if (csv.mmap(CSV_FILE_NAME_OR_PATH)) {
		const auto header = csv.header();

		if (printTitle) {
			// Read header
			std::vector<std::string> fields;

			for (const auto& field : header) {
				std::string val;
				field.read_value(val);

				// Remove potential carriage return from the end of the string
				if (!val.empty() && val.back() == '\r')
					val.pop_back();

				fields.push_back(val);
			}

			std::cout << std::format("{:<20}\t{:<10}\t{:<10}\t{:<10}\n", fields.at(0), fields.at(1), fields.at(2), fields.at(3));
		}

		for (const auto& row : csv) {
			std::vector<std::string> temp;

			for (const auto& cell : row) {
				std::string val;
				cell.read_value(val);

				// Remove potential carriage return from the end of the string
				if (!val.empty() && val.back() == '\r')
					val.pop_back();

				temp.push_back(val);
			}

			if (!temp.empty()) {
				Student student(temp.at(0), std::stoi(temp.at(1)), std::stoi(temp.at(2)), std::stoi(temp.at(3)));
				students.push_back(student);
			}
		}
	}
	else {
		std::cout << "Failed to read csv data.\n";
		std::exit(1);
	}

	return students;
}

/*
* For demo it is really modify the target csv file,
* I will read the csv file again and again in this showData(),
* althought it will cost a lot of IO,
* but just for class demo so don't care it right now.
*/
void showData() {
	// read csv data
	std::vector<Student> students = readData(true);

	// print it to console
	for (Student student : students)
		std::cout << std::format("{:<20}\t{:<10}\t{:<10}\t{:<10}\n", student.getName(), student.getChineseScore(), student.getEnglishScore(), student.getMathScore());
}

/*
* 
*/
void writeData(std::vector<Student> students) {
	std::ofstream csv(CSV_FILE_NAME_OR_PATH);
	csv2::Writer<csv2::delimiter<','>> writer(csv);
	std::string title = "姓名,國文成績,英文成績,數學成績\n";
	
	csv << title;

	for (auto student : students) {
		std::vector<std::string> temp;
		temp.push_back(student.getName());
		temp.push_back(std::to_string(student.getChineseScore()));
		temp.push_back(std::to_string(student.getEnglishScore()));
		temp.push_back(std::to_string(student.getMathScore()));

		writer.write_row(temp);
	}

	csv.close();
}

int main() {
	std::locale::global(std::locale("zh_TW.UTF-8"));
	SetConsoleOutputCP(CP_UTF8);

	// Error setting check, if something error then terminal program
	std::string fileName = CSV_FILE_NAME_OR_PATH;
	if (fileName.empty())
		return 1;

	// Pass error check then check file is exist or not then follow the user setting
	if (!std::filesystem::exists(CSV_FILE_NAME_OR_PATH)) {
		if (CREATE_DEFAULT_DATA) {
			createDefaultData();
			std::cout << "File does not exist but already created.\n\n";
		}
		else {
			std::cout << "Target file does not exist and CREATE_DEFAULT_DATA is false.\n";
			return 1;
		}
	}

	// read data from csv
	std::vector<Student> students = readData(false);

	// Show old data
	showData();

	// Pick up one field then modify it
	students.at(0).setChineseScore(0);
	writeData(students);

	// Show new data
	std::cout << "\n\n==========New Data==========\n";
	showData();
}
