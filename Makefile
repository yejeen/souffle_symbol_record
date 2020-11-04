
CC = g++ -std=c++17

CXXFLAGS = -Wall

PARALLELFLAGS = -fopenmp

TEST_DIR = ./tests

INCLUDES = -Isrc/include/ -I./

TARGET = run_test

NUM_OF_THREADS = 1

NUM_OF_ENTRIES = 10000

NUM_OF_RECORDS = 100000

STRING_LENGTH = -1

all: check clean

check: symbol record clean

symbol:
	@echo "\n********** Test Symbol Table **********"
	@$(CC) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_DIR)/symbol_table_test.cpp
	@./$(TARGET)

record:
	@echo "\n********** Test Record Table **********"
	@$(CC) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_DIR)/record_table_test.cpp
	@./$(TARGET)

parallel-symbol:
	@echo "\n********** Test Parallel Symbol Table **********"
	@$(CC) $(CXXFLAGS) $(PARALLELFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_DIR)/symbol_table_parallel_test.cpp
	@./$(TARGET)

performance-symbol:
	@echo "\n********** Test Performance Symbol Table **********"
	@$(CC) $(CXXFLAGS) $(PARALLELFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_DIR)/symbol_table_performance_test.cpp
	@./$(TARGET) $(NUM_OF_THREADS) $(FILE_PATH) $(STRING_LENGTH)

performance-record:
	@echo "\n********** Test Performance Record Table **********"
	@$(CC) $(CXXFLAGS) $(PARALLELFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_DIR)/record_table_performance_test.cpp
	@./$(TARGET) $(NUM_OF_THREADS) $(NUM_OF_ENTRIES) $(RECORD_LENGTH) $(NUM_OF_RECORDS)

clean:
	rm -f $(TARGET)

.PHONY: clean
