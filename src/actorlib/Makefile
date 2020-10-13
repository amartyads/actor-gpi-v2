SRC = .
BLD = ../../build

SOURCE = $(wildcard $(SRC)/*.cpp)
INCLUDE = $(wildcard $(SRC)/*.hpp)
OBJECT = $(patsubst %,$(BLD)/%, $(notdir $(SOURCE:.cpp=.o)))

#CC = g++
CC = mpicc

#IFLAGS = -I/home/amartya/local/include
UTILFLAGS = -I./utils
#LFLAGS = -L/home/amartya/local/lib64
#CFLAGS = -lGPI2 -pthread

LFLAGS = -w -O0 -pipe -g $(GPI_2_INC)
CFLAGS = -lGPI2 -lpthread -libverbs

all: $(OBJECT)

$(BLD)/%.o: $(SRC)/%.cpp $(SRC)/%.hpp
	mkdir -p $(dir $@)
	#$(CC) $(IFLAGS) $(UTILFLAGS) $(LFLAGS) -c $< -o $@ $(CFLAGS)
	$(CC) $(UTILFLAGS) $(LFLAGS) -c $< -o $@ $(CFLAGS)

.PHONY: clean

clean:
	$(RM) -r $(BLD)
