/************************************************************************
 *
 * @file symbol_table_performance_test.cpp
 *
 * Tests souffle's symbol table.
 *
 ***********************************************************************/


#include "tests/test.h"

#include "souffle/SymbolTable.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace souffle::test {

#define MIN_STRING_LENGTH 6
#define MAX_STRING_LENGTH 20
#define NUM_OF_THREADS 4
#define FILE_PATH "randomstr.txt"

std::vector<std::string> getRandomStrings();

// input new strings via lookup
TEST(SymbolTable, Insertion) {
    std::vector<std::string> randomStrings = getRandomStrings();

    SymbolTable table;
    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    for(std::vector<std::string>::size_type i = 0; i < randomStrings.size(); i++) {
        table.lookup(randomStrings[i]);
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << elapsed_seconds.count() << " s\n";

    //check insertion
    for(std::vector<std::string>::size_type i = 0; i < randomStrings.size(); i++) {
        EXPECT_STREQ(randomStrings[i], table.resolve(table.lookup(randomStrings[i])));
    }
}

#ifdef _OPENMP
// input new strings via lookup
TEST(SymbolTable, ParallelInsertion) {
    std::vector<std::string> randomStrings = getRandomStrings();

    SymbolTable table;
    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
#pragma omp parallel for num_threads(4)
    for(std::vector<std::string>::size_type i = 0; i < randomStrings.size(); i++) {
        table.lookup(randomStrings[i]);
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << elapsed_seconds.count() << " s\n";

}
#endif

TEST(SymbolTable, Lookup) {
    std::vector<std::string> randomStrings = getRandomStrings();

    SymbolTable table;
    //input new strings
    for(std::vector<std::string>::size_type i = 0; i < randomStrings.size(); i++) {
        table.lookup(randomStrings[i]);
    }

    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    for(std::vector<std::string>::size_type i = 0; i < randomStrings.size(); i++) {
        table.lookup(randomStrings[i]);
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << elapsed_seconds.count() << " s\n";

}

#ifdef _OPENMP
TEST(SymbolTable, ParallelLookup) {
    std::vector<std::string> randomStrings = getRandomStrings();

    SymbolTable table;
    //input new strings
    for(std::vector<std::string>::size_type i = 0; i < randomStrings.size(); i++) {
        table.lookup(randomStrings[i]);
    }

    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
#pragma omp parallel for num_threads(4)
    for(std::vector<std::string>::size_type i = 0; i < randomStrings.size(); i++) {
        table.lookup(randomStrings[i]);
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << elapsed_seconds.count() << " s\n";

}
#endif

TEST(SymbolTable, Resolve) {
    std::vector<std::string> randomStrings = getRandomStrings();

    SymbolTable table;
    std::vector<size_t> indices;

    //input new strings
    for(std::vector<std::string>::size_type i = 0; i < randomStrings.size(); i++) {
        indices.push_back(table.lookup(randomStrings[i]));
    }

    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    for(std::vector<size_t>::size_type i = 0; i < indices.size(); i++) {
        table.resolve(indices[i]);
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << elapsed_seconds.count() << " s\n";

}

#ifdef _OPENMP
TEST(SymbolTable, ParallelResolve) {
    std::vector<std::string> randomStrings = getRandomStrings();

    SymbolTable table;
    std::vector<size_t> indices;

    //input new strings
    for(std::vector<std::string>::size_type i = 0; i < randomStrings.size(); i++) {
        indices.push_back(table.lookup(randomStrings[i]));
    }

    //start
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
#pragma omp parallel for num_threads(4)
    for(std::vector<size_t>::size_type i = 0; i < indices.size(); i++) {
        table.resolve(indices[i]);
    }
    //end
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << elapsed_seconds.count() << " s\n";

}
#endif

std::vector<std::string> getRandomStrings(){
    srand((unsigned int)time(NULL));

    std::string filePath = FILE_PATH;
    std::ifstream openFile(filePath.data());

    std::vector<std::string> randomStrings;

    if(openFile.is_open()){
        std::string line;
        while(getline(openFile, line)){
            int randomLength = rand() % (MAX_STRING_LENGTH-1) + MIN_STRING_LENGTH;
            line.resize(randomLength);
            randomStrings.push_back(line);
        }
        openFile.close();
    }
    return randomStrings;
}

} // namespace souffle::test