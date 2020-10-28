/************************************************************************
 *
 * @file record_table_performance_test.cpp
 *
 * Tests souffle's record table.
 *
 ***********************************************************************/

#include "souffle/CompiledTuple.h"
#include "souffle/RamTypes.h"
#include "souffle/RecordTable.h"
#include <cmath>
#include <chrono>

std::array<double,2> test(int numOfThreads, int numOfEntries, int numOfRecords,
            std::vector<std::vector<souffle::RamDomain>> *records);

// argv[1]: num of threads
// argv[2]: num of entries
// argv[3]: num of records
// argv[4]: record length
int main(int argc, char** argv) {
    int numOfThreads = 1;
    int numOfEntries = 10000;
    int numOfRecords = 100000; //arity
    int recordLength = 10; //recordLength

    // set parameters
    switch (argc) {
        case 5:
            recordLength = std::stoi(argv[4]);
        case 4:
            numOfRecords = std::stoi(argv[3]);
        case 3:
            numOfEntries = std::stoi(argv[2]);
        case 2:
            numOfThreads = std::stoi(argv[1]);
            if(numOfThreads == 0) {
                std::cerr << "invalid argument! numOfThreads = 0" << std::endl;
                exit(1);
            }
        default:
            break;
    }

    int minRecord = pow(10, recordLength - 1);
    int maxRecord = trunc(log10(std::numeric_limits<souffle::RamDomain>::max())) < recordLength ?
                    std::numeric_limits<souffle::RamDomain>::max() : pow(10, recordLength) - 1;

    // check if possible to generate test records
    if(numOfRecords * numOfEntries > maxRecord - minRecord + 1) {
        std::cout << "invalid arguments: cannot produce sufficient unique records ";
        std::cout << "(possible record range: " << minRecord << " - " << maxRecord << ")" << std::endl;
        exit(1);
    }

    // generate fixed-length random records
    std::vector<std::vector<souffle::RamDomain>> records(numOfRecords * numOfEntries);
    int count = minRecord;
    for(int i = 0 ; i < numOfEntries ; i++) {
        std::vector<souffle::RamDomain> temp(numOfRecords);
        for (int j = 0; j < numOfRecords; ++j) {
            temp[j] = count++;
        }
        records[i] = temp;
    }

    std::cout << "numOfThreads: " << numOfThreads << std::endl;
    std::cout << "numOfEntries: " << numOfEntries << std::endl;
    std::cout << "numOfRecords(arity): " << numOfRecords << std::endl;
    std::cout << "recordLength: " << recordLength << std::endl;
    std::cout << "# of threads\tpack\t\tunpack\n";

    std::array<double,2> durations;
    for (int i = 1; i < numOfThreads; ++i) {
        durations = test(numOfThreads, numOfEntries, numOfRecords, &records);
        std::cout << i << "\t\t" << durations[0] << " s\t" << durations[1] << " s\n";
    }
}

std::array<double,2> test(int numOfThreads, int numOfEntries, int numOfRecords,
            std::vector<std::vector<souffle::RamDomain>> *records) {
    souffle::RecordTable recordTable;
    std::vector<souffle::RamDomain> references(numOfEntries);
    std::array<double,2> durations;

    // start pack test
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
#pragma omp parallel for num_threads(numOfThreads)
    for (int i = 0; i < numOfEntries; i++) {
        references[i] = recordTable.pack(records->at(i).data(), numOfRecords);
    }
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    durations[0] = elapsed_seconds.count();

    // start unpack test
    startTime = std::chrono::system_clock::now();
#pragma omp parallel for num_threads(numOfThreads)
    for (int i = 0; i < numOfEntries; i++) {
        recordTable.unpack(references[i], numOfRecords);
    }
    endTime = std::chrono::system_clock::now();
    elapsed_seconds = endTime - startTime;
    durations[1] = elapsed_seconds.count();

    return durations;
}