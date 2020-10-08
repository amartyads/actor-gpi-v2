SRC = .
BLD = ../../build

SOURCE = $(wildcard $(SRC)/*.cpp)
INCLUDE = $(wildcard $(SRC)/*.hpp)
OBJECT = $(patsubst %,$(BLD)/%, $(notdir $(SOURCE:.cpp=.o)))

CC = g++

IFLAGS = -I/home/amartya/local/include
UTILFLAGS = -I./utils
LFLAGS = -L/home/amartya/local/lib64
CFLAGS = -lGPI2 -pthread

all: $(OBJECT)

$(BLD)/%.o: $(SRC)/%.cpp $(SRC)/%.hpp
	mkdir -p $(dir $@)
	$(CC) $(IFLAGS) $(UTILFLAGS) $(LFLAGS) -c $< -o $@ $(CFLAGS)

.PHONY: clean

clean:
	$(RM) -r $(BLD)