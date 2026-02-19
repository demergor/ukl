CXX := clang++
CXXFLAGS := -std=c++23 -Wall -Iinclude -I$(HOME)/googletest/googletest/include

SRC_DIR := src
BUILD_DIR := build
TEST_DIR := tests

SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
TEST_SRCS := $(shell find $(TEST_DIR) -name "*.cpp")

OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))

LIB_OBJS := $(filter-out $(BUILD_DIR)/main.o, $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(shell find $(SRC_DIR) -name "*.cpp")))

TARGET := ukl
TEST_TARGET := run_tests

GTEST_LIBS := $(HOME)/googletest/build/lib/libgtest.a \
							$(HOME)/googletest/build/lib/libgtest_main.a
LDFLAGS := -pthread $(GTEST_LIBS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(TEST_TARGET): $(LIB_OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJS) $(TEST_OBJS) $(LDFLAGS) -o $(TEST_TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean: 
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)

.PHONY: all clean test
