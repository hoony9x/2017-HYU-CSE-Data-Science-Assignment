CXX = g++
OBJS = main.o data_read.o learning.o testing.o
TARGET = dt.exe
CXXFLAGS = -O2 --std=c++11
 
.SUFFIXES : .cpp .o
 
all : $(TARGET)
 
$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS)
 
clean :
	rm -f $(OBJS) $(TARGET)
