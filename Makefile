
CC = g++ -std=c++17

CXXFLAGS = -Wall

PARALLELFLAGS = -Xpreprocessor -fopenmp

TEST_DIR = ./tests

INCLUDES = -Isrc/include/ -I./

LIBTBB = -ltbb

LIBOMP = -lomp

TARGET = run_test

NUM_OF_THREADS = 1

NUM_OF_ENTRIES = 10000

NUM_OF_RECORDS = 100000

STRING_LENGTH = -1

all: check clean

check: symbol record clean

symbol:
	@echo "\n********** Test Symbol Table **********"
	@$(CC) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_DIR)/symbol_table_test.cpp $(LIBTBB)
	@./$(TARGET)

record:
	@echo "\n********** Test Record Table **********"
	@$(CC) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_DIR)/record_table_test.cpp $(LIBTBB)
	@./$(TARGET)

parallel-symbol:
	@echo "\n********** Test Parallel Symbol Table **********"
	@$(CC) $(CXXFLAGS) $(PARALLELFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_DIR)/symbol_table_parallel_test.cpp $(LIBTBB) $(LIBOMP)
	@./$(TARGET)

parallel-record:
	@echo "\n********** Test Parallel Record Table **********"
	@$(CC) $(CXXFLAGS) $(PARALLELFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_DIR)/record_table_parallel_test.cpp $(LIBTBB) $(LIBOMP)
	@./$(TARGET)

performance-symbol:
	@echo "\n********** Test Performance Symbol Table **********"
	@$(CC) $(CXXFLAGS) $(PARALLELFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_DIR)/symbol_table_performance_test.cpp $(LIBTBB) $(LIBOMP)
	@./$(TARGET) $(NUM_OF_THREADS) $(STRING_LENGTH) $(FILE_PATH)

performance-record:
	@echo "\n********** Test Performance Record Table **********"
	@$(CC) $(CXXFLAGS) $(PARALLELFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_DIR)/record_table_performance_test.cpp $(LIBTBB) $(LIBOMP)
	@./$(TARGET) $(NUM_OF_THREADS) $(NUM_OF_ENTRIES) $(NUM_OF_RECORDS) $(RECORD_LENGTH)

clean:
	rm -f $(TARGET)

.PHONY: clean
