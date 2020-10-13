TARGET = main.out

SRC = ../../actorlib
BLD = ../../../build
LOCBLD = build

LIBSOURCE = $(wildcard $(SRC)/*.cpp)
LIBINCLUDE = $(wildcard $(SRC)/*.hpp)
LIBOBJECT = $(patsubst %,$(BLD)/%, $(notdir $(LIBSOURCE:.cpp=.o)))
LOCSOURCE =  $(wildcard *.cpp)
LOCINCLUDE = $(wildcard *.hpp)
LOCOBJECT = $(patsubst %,$(LOCBLD)/%, $(notdir $(LOCSOURCE:.cpp=.o)))

#CC = g++
CC = mpicc

#IFLAGS = -I/home/amartya/local/include
UTILFLAGS = -I$(SRC)/utils
#LFLAGS = -L/home/amartya/local/lib64
#CFLAGS = -lGPI2 -pthread

LFLAGS = -w -O0 -pipe -g $(GPI_2_INC)
CFLAGS = -lGPI2 -lpthread -libverbs

$(TARGET) :  $(LOCOBJECT) $(LIBOBJECT)
	#$(CC) $(IFLAGS) $(UTILFLAGS) $(LFLAGS) -o $@ $^ $(CFLAGS)
	$(CC) $(UTILFLAGS) $(LFLAGS) -o $@ $^ $(CFLAGS)

$(LOCOBJECT): $(LOCBLD)/%.o: %.cpp $(LOCINCLUDE)
	mkdir -p $(dir $@)
	#$(CC) $(IFLAGS) -I$(SRC) $(UTILFLAGS) $(LFLAGS) -c $< -o $@ $(CFLAGS)
	$(CC) -I$(SRC) $(UTILFLAGS) $(LFLAGS) -c $< -o $@ $(CFLAGS)

$(LIBOBJECT): $(LIBSOURCE) $(LIBINCLUDE)
	cd $(SRC) && $(MAKE)

.PHONY: help clean

clean :
	cd $(SRC) && $(MAKE) clean
	$(RM) -r $(LOCBLD)
	$(RM) $(TARGET)

help :
	@echo "src: $(LIBSOURCE)"
	@echo "obj: $(LIBOBJECT)"
	@echo "inc: $(LIBINCLUDE)"
	@echo "src: $(LOCSOURCE)"
	@echo "obj: $(LOCOBJECT)"
	@echo "inc: $(LOCINCLUDE)"
	