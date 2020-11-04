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

void printDuration(int numOfThreads, double insertTime, double lookupTime, double resolveTime);

std::vector<std::string> getRandomStrings(std::string filePath, int stringLength){
    int minStringLength = 6;
    int maxStringLength = 20;

    srand((unsigned int)time(NULL));

    std::ifstream openFile(filePath.data());
    std::vector<std::string> randomStrings;

    if(openFile.is_open()){
        std::string line;
        while(getline(openFile, line)){
            int randomLength = stringLength;
            if(stringLength == -1) {
                randomLength = rand() % (maxStringLength-1) + minStringLength;
            }
            line.resize(randomLength);
            randomStrings.push_back(line);
        }
        openFile.close();
    }
    return randomStrings;
}

double insert(std::vector<std::string> *randomStrings) {

    souffle::SymbolTable table;
    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    for(std::vector<std::string>::size_type i = 0; i < randomStrings->size(); i++) {
        table.lookup(randomStrings->at(i));
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    return elapsed_seconds.count();
}

double insertInParallel(int numOfThreads, std::vector<std::string> *randomStrings){

    souffle::SymbolTable table;
    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
#pragma omp parallel for num_threads(numOfThreads)
    for(std::vector<std::string>::size_type i = 0; i < randomStrings->size(); i++) {
        table.lookup(randomStrings->at(i));
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    return elapsed_seconds.count();
}

double lookup(std::vector<std::string> *randomStrings) {

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
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    return elapsed_seconds.count();
}

double lookupInParallel(int numOfThreads, std::vector<std::string> *randomStrings) {

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
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    return elapsed_seconds.count();
}

double resolve(std::vector<std::string> *randomStrings) {

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
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    return elapsed_seconds.count();
}

double resolveInParallel(int numOfThreads, std::vector<std::string> *randomStrings) {

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
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    return elapsed_seconds.count();
}

// argv[1]: num of threads. no use multi-threading if 0
// argv[2]: length of symbol string (6 <= length <= 20) if not exist or -1, length is random
// argv[3]: file path of random strings
int main(int argc, char** argv) {
    int maxNumOfThreads = 1;
    std::string filePath = "randomStrings.txt";
    int stringLength = -1;

    if(argc > 1) {
        maxNumOfThreads = std::stoi(argv[1]);
        if (maxNumOfThreads == 0) {
            std::cout << "invalid argument! numOfThreads = 0" << std::endl;
            exit(1);
        }
    }
    std::cout << "maxNumOfThreads: " + std::to_string(maxNumOfThreads) << std::endl;

    if(argc > 2) {
        stringLength = std::stoi(argv[2]);
        if ((stringLength != -1 && stringLength < 6) || stringLength > 20 ) {
            std::cout << "invalid argument! stringLength == -1 or 6 <= stringLength <= 20 " << std::endl;
            exit(1);
        }
    }

    if(argc > 3) {
        filePath = argv[3];
    }

    std::vector<std::string> randomStrings = getRandomStrings(filePath, stringLength);
    std::cout << "numOfStrings: " + std::to_string(randomStrings.size()) << std::endl;


    std::cout << "# of threads\tinsert\t\tlookup\t\tresolve" << std::endl;
    double insertTime;
    double lookupTime;
    double resolveTime;
    for (int numOfThreads = 1; numOfThreads <= maxNumOfThreads; ++numOfThreads) {
        if(numOfThreads > 1) {
            insertTime = insertInParallel(numOfThreads, &randomStrings);
            lookupTime = lookupInParallel(numOfThreads, &randomStrings);
            resolveTime = resolveInParallel(numOfThreads, &randomStrings);
        } else {
            insertTime = insert(&randomStrings);
            lookupTime = lookup(&randomStrings);
            resolveTime = resolve(&randomStrings);
        }
        printDuration(numOfThreads, insertTime, lookupTime, resolveTime);
    }
}

void printDuration(int numOfThreads, double insertTime, double lookupTime, double resolveTime) {
    std::cout << numOfThreads << "\t\t"
        << std::to_string(insertTime) << " s\t"
        << std::to_string(lookupTime) << " s\t"
        << std::to_string(resolveTime) << " s\n";
}