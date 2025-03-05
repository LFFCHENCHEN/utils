CXX = g++
CXXFLAGS = -std=c++11 -Wall -DELPP_NO_DEFAULT_LOG_FILE -DELPP_THREAD_SAFE
LDFLAGS = -lpthread

TARGET = main
SRCS = main.cpp easylogging++.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET) xx.log

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
