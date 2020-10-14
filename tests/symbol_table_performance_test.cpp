/************************************************************************
 *
 * @file symbol_table_performance_test.cpp
 *
 * Tests souffle's symbol table.
 *
 ***********************************************************************/


#include "souffle/SymbolTable.h"
#include "souffle/utility/MiscUtil.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

void printDuration(std::chrono::system_clock::time_point startTime,
                   std::chrono::system_clock::time_point endTime,
                   std::string title);

std::vector<std::string> getRandomStrings(std::string filePath){
    int minStringLength = 6;
    int maxStringLength = 20;

    srand((unsigned int)time(NULL));

    std::ifstream openFile(filePath.data());

    std::vector<std::string> randomStrings;

    if(openFile.is_open()){
        std::string line;
        while(getline(openFile, line)){
            int randomLength = rand() % (maxStringLength-1) + minStringLength;
            line.resize(randomLength);
            randomStrings.push_back(line);
        }
        openFile.close();
    }
    return randomStrings;
}

void insert(std::vector<std::string> *randomStrings) {

    souffle::SymbolTable table;
    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    for(std::vector<std::string>::size_type i = 0; i < randomStrings->size(); i++) {
        table.lookup(randomStrings->at(i));
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    printDuration(startTime, endTime, "insert");
}

void insertInParallel(int numOfThreads, std::vector<std::string> *randomStrings){

    souffle::SymbolTable table;
    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
#pragma omp parallel for num_threads(numOfThreads)
    for(std::vector<std::string>::size_type i = 0; i < randomStrings->size(); i++) {
        table.lookup(randomStrings->at(i));
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    printDuration(startTime, endTime, "parallelInsert");
}

void lookup(std::vector<std::string> *randomStrings) {

    souffle::SymbolTable table;
    //input new strings
    for(std::vector<std::string>::size_type i = 0; i < randomStrings->size(); i++) {
        table.lookup(randomStrings->at(i));
    }

    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    for(std::vector<std::string>::size_type i = 0; i < randomStrings->size(); i++) {
        table.lookup(randomStrings->at(i));
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    printDuration(startTime, endTime, "lookup");
}

void lookupInParallel(int numOfThreads, std::vector<std::string> *randomStrings) {

    souffle::SymbolTable table;
    //input new strings
    for(std::vector<std::string>::size_type i = 0; i < randomStrings->size(); i++) {
        table.lookup(randomStrings->at(i));
    }

    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
#pragma omp parallel for num_threads(numOfThreads)
    for(std::vector<std::string>::size_type i = 0; i < randomStrings->size(); i++) {
        table.lookup(randomStrings->at(i));
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    printDuration(startTime, endTime, "lookupParallel");
}

void resolve(std::vector<std::string> *randomStrings) {

    souffle::SymbolTable table;
    std::vector<size_t> indices;

    //input new strings
    for(std::vector<std::string>::size_type i = 0; i < randomStrings->size(); i++) {
        indices.push_back(table.lookup(randomStrings->at(i)));
    }

    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    for(std::vector<size_t>::size_type i = 0; i < indices.size(); i++) {
        table.resolve(indices[i]);
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    printDuration(startTime, endTime, "resolve");
}

void resolveInParallel(int numOfThreads, std::vector<std::string> *randomStrings) {

    souffle::SymbolTable table;
    std::vector<size_t> indices;

    //input new strings
    for(std::vector<std::string>::size_type i = 0; i < randomStrings->size(); i++) {
        indices.push_back(table.lookup(randomStrings->at(i)));
    }

    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
#pragma omp parallel for num_threads(numOfThreads)
    for(std::vector<size_t>::size_type i = 0; i < indices.size(); i++) {
        table.resolve(indices[i]);
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    printDuration(startTime, endTime, "resolveParallel");
}

// argv[1]: num of threads
// argv[2]: file path of random strings
int main(int argc, char** argv) {
    int numOfThreads = 1;
    std::string filePath = "randomstr.txt";

    if(argc > 1) {
        numOfThreads = std::stoi(argv[1]);
    }
    std::cout << "numOfThreads: " + std::to_string(numOfThreads) << std::endl;

    if(argc > 2) {
        filePath = argv[2];
    }
    std::vector<std::string> randomStrings = getRandomStrings(filePath);
    std::cout << "numOfStrings: " + std::to_string(randomStrings.size()) << std::endl;

    insert(&randomStrings);
    insertInParallel(numOfThreads, &randomStrings);
    lookup(&randomStrings);
    lookupInParallel(numOfThreads, &randomStrings);
    resolve(&randomStrings);
    resolveInParallel(numOfThreads, &randomStrings);
}

void printDuration(std::chrono::system_clock::time_point startTime,
                   std::chrono::system_clock::time_point endTime,
                   std::string title) {
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << title + ": " + std::to_string(elapsed_seconds.count()) << " s\n";
}