
CC = g++ -std=c++17

CXXFLAGS = -Wall

PARALLELFLAGS = -fopenmp

TEST_DIR = ./tests

INCLUDES = -Isrc/include/ -I./

TARGET = run_test

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

clean:
	rm -f $(TARGET)

.PHONY: clean
