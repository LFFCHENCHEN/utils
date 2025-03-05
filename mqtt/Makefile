
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread
LDFLAGS = -lmosquitto

TARGET = main
SRCS = main.cpp MqttClient.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
