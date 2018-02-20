CXX = g++


include Makefile.locale


GUROBI_INCLUDEDIR=$(strip $(GUROBI_PATH))/$(strip $(HOST_ARCH))/include
GUROBI_LIBDIR=$(strip $(GUROBI_PATH))/$(strip $(HOST_ARCH))/lib


LIBS = -lgurobi_c++ -lgurobi75 -lm -D_GLIBCXX_USE_CXX11_ABI=0 -m64 -w

CXXFLAGS = -MMD -I . -I ./src  -I /usr/local/include/ -I $(GUROBI_INCLUDEDIR) -g -O3 -std=c++11

LINK_FLAGS = -g -L ./ -L /usr/local/lib/ -L$(GUROBI_LIBDIR)

OBJS = ./src/propagate_intervals.o ./src/network_computation.o ./src/gurobi_interface.o ./src/configuration.o
DEPENDS = ${OBJECTS:.o=.d}


all: libs run_file

libs: $(OBJS)
	ar rcs ./src/libsherlock.a $(OBJS)
	ranlib ./src/libsherlock.a
	cp ./src/*.h ./include


run_file: main.o $(OBJS)
	g++ -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)


clean:
	rm -f ./src/*.o *.o ./run_file ./lib/* ./include/configuration.h ./include/gurobi_interface.h ./include/network_computation.h ./include/propagate_intervals.h 	

-include ${DEPENDS}   
