//============================================================================
// Name        : Project Two, Hash Table Method
// Author      : Emily Nagorski
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <sstream>
#include <fstream>
#include <algorithm>
#include <climits>
#include <iostream>
#include <string> // atoi
#include <time.h>
#include <vector>
#include <stdexcept>
#include <cctype>


using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;


// define a structure to hold course information
struct Course {
    string courseId; // unique identifier
    string title;
    vector<string> prereq;
};

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold courses
    struct Node {
        Course course;
        unsigned int key;
        Node *next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a course
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a course and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    vector<Node> nodes;
    unsigned int tableSize = DEFAULT_SIZE;
    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll();
    Course Search(string courseId);
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    nodes.resize(tableSize);
    // Initalize node structure by resizing tableSize
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    this->tableSize = size;
    // resize nodes size
    nodes.resize(size);
}

/**
 * Destructor
 */
HashTable::~HashTable() {
    nodes.erase(nodes.begin());
    // erase nodes beginning
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
    // return key tableSize
    return key % tableSize;
}

/**
 * Insert a course
 *
 * @param course The course to insert
 */
void HashTable::Insert(Course course) {
    // create the key for the given course
    unsigned key = hash(atoi(course.courseId.c_str()));
    // retrieve node using key
    // if no entry found for the key
    Node* oldNode = &(nodes.at(key));
    if (oldNode == nullptr) {
        Node* newNode = new Node(course, key);
        // assign this node to the key position
        nodes.insert(nodes.begin()+key, (*newNode));
        // else if node is not used
    } else {
        // assign old node key to UNIT_MAX, set to key, set old node to course and old node next to null pointer
        if (oldNode->key == UINT_MAX) {
            oldNode->key = key;
            oldNode->course = course;
            oldNode->next = nullptr;
        // else find the next open node
        } else {
            //find the next open node
            while (oldNode->next != nullptr) {
                oldNode = oldNode->next;
            }
            // add new newNode to end
            oldNode->next = new Node(course, key);
        }
    }          
}

/**
 * Print all courses
 */
void HashTable::PrintAll() {
    // for node begin to end iterate
    for (auto it = nodes.begin(); it !=nodes.end(); ++it) {
        //   if key not equal to UINT_MAx
        if (it->key != UINT_MAX) {
            // node is equal to next iter
            Node* node = it->next;
            // while node not equal to nullptr
            while (node != nullptr) {
                // output course, title, for entries w/o prereq
                if (node->course.prereq.size() == 0) {
                    cout << "Course " << ": " << it->course.courseId << " | " << it->course.title << " | ";
                } else {
                    // output course, title, prereq
                    cout << "Course " << ": " << node->course.courseId << " | " << node->course.title << " | ";
                    cout << node->course.prereq.at(0);

                    for (unsigned int i = 1; i < node->course.prereq.size(); ++i) {
                        cout << ", " << node->course.prereq.at(i);
                    }
                }
            cout << endl;
            // node is equal to next node
            node = node->next;
            }     
        }
    }
}


/**
 * Search for the specified courseId
 *
 * @param courseId The course id to search for
 */
Course HashTable::Search(string courseId) {
    Course course;
    unsigned int key = hash(atoi(courseId.c_str()));
    Node *node = &(nodes.at(key));
    // create the key for the given course
    // if entry found for the key
    if (node != nullptr && node->key != UINT_MAX && node->course.courseId.compare(courseId) == 0) {
        //return node course
        return node->course;
    }
    // if no entry found for the key
    if (node == nullptr || node->key == UINT_MAX) {
        // return course
        return course;
    }
    // while node not equal to nullptr
    while (node != nullptr) {
        // if the current node matches, return it
        if (node->key != UINT_MAX && node->course.courseId.compare(courseId) == 0) {
            return node->course;
        }
        //node is equal to next node
        node = node->next;
    }
    return course;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the course information to the console (std::out)
 *
 * @param course struct containing the course info
 */
void displayCourse(Course course) {
    if (course.prereq.size() == 0) {
        //output course, title for course w/o prereq
        cout << "Course " << ": " << course.courseId << " | " << course.title << " | ";
    } else {
        // output course, title, prereq
        cout << "Course " << ": " << course.courseId << " | " << course.title << " | ";
        cout << course.prereq.at(0);

        for (unsigned int i = 1; i < course.prereq.size(); ++i) {
            cout << ", " << course.prereq.at(i);
        }
    }
    cout << endl;
    return;
}


vector<vector<string>> readFile(string csvPath);

// error handling
struct FileParserError : public exception {
    string  msg;
    FileParserError(string err) {
        msg = string("File Parser: ").append(err);
    }
    const char* what() const throw () {
        return msg.c_str();
    }
};

/**
 * Load a CSV file containing courses into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the courses read
 */
void loadCourse(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    vector<string> courseTitles;
    try {
        // read file file
        vector<vector<string>> fileContents = readFile(csvPath);

        // extract course titles
        for (unsigned int i = 0; i < fileContents.size(); ++i) {
            vector<string> row = fileContents.at(i);
            if (row.size() >= 2) {
                courseTitles.push_back(row.at(0));
            }
        }
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < fileContents.size(); i++) {

            // Create a data structure and add to the collection of courses
            vector<string> row = fileContents.at(i);
            if (row.size() >= 2) {
                Course course;

                // course number
                course.courseId = row.at(0);

                // course name
                course.title= row.at(1);

                // add pre-requisite(s)
                for (unsigned int j = 2; j < row.size(); ++j) {
                    if (find(courseTitles.begin(), courseTitles.end(), row.at(j)) != courseTitles.end()) {
                        course.prereq.push_back(row.at(j));
                    }
                }

            // push this course to the end
            hashTable->Insert(course);
            }
        }
    }catch (FileParserError &err) {
        cerr << err.what() << endl;
    }
}

/**
* split string by ',' delimiter
*/
vector<string> parseLine(string line) {
    vector<string> row;
    string delimiter = ",";
    int startIndex = 0;
    int endIndex = line.find(delimiter);

    while (endIndex != -1) {
        row.push_back(line.substr(startIndex, endIndex - startIndex));
        startIndex = endIndex + delimiter.size();
        endIndex = line.find(delimiter, startIndex);
    }
    row.push_back(line.substr(startIndex, endIndex - startIndex));
    return row;
}

/**
* Read and parse CSV file
* 
* @param csvPath path to csv file
* @return file content in a vector
*/
vector<vector<string>> readFile(string csvPath) {
    vector<vector<string>> fileContents;
    fstream csvFile;

    // open file for reading
    csvFile.open(csvPath, ios::in);

    if (csvFile.is_open()) {
        string line;
        // read data from file, line by line
        while (getline(csvFile, line)) {
            if (line != "") {
                // string string by ','
                vector<string> row = parseLine(line);
                fileContents.push_back(row);
            }
        }

        // close file
        csvFile.close();
    }
    else {
        throw FileParserError(string("Failed to open file: ").append(csvPath));
    }

    return fileContents;
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // Define a hash table to hold all the courses
    HashTable* courseTable;

    Course course;
    courseTable = new HashTable();
    
    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Data Structure" << endl;
        cout << "  2. Print Course List" << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        string courseNumber;
        string courseKey;
        string csvPath;

        switch (choice) {

        case 1:
            
            // Complete the method call to load the courses
            cout << "Enter file path: ";            
            cin.ignore();
            getline(cin, csvPath);

            loadCourse(csvPath, courseTable);
            break;

        case 2:
            courseTable->PrintAll();
            break;

        case 3:

            cout << "Enter course number: ";
            cin.ignore();
            getline(cin, courseKey);

            // check if input is empty
            courseNumber = courseKey;

            if (courseNumber == "") {
                cout << "Course number cannot be empty" << endl;
            } else {
                course = courseTable->Search(courseKey);

                if (!course.courseId.empty()) {
                    displayCourse(course);
                } else {
                    cout << "Course Id " << courseKey << " not found." << endl;
                }
            break;
            }
        }
    }
    cout << "Good bye." << endl;

    return 0;
}
