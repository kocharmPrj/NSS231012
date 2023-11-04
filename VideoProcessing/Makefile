CXX = g++
CXXFLAGS = -std=c++11
OPENCV_LIBS = `pkg-config --cflags --libs opencv4`

TARGET = AutoParking

SRCS = main.cpp LaneDetector.cpp
HEADERS = LaneDetector.h OpencvInit.h

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(OPENCV_LIBS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(OPENCV_LIBS)

clean:
	rm -f $(OBJS) $(TARGET)
