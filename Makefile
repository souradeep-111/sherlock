CXX = g++


include Makefile.locale


GUROBI_INCLUDEDIR = $(GUROBI_PATH)/$(HOST_ARCH)/include
GUROBI_LIBDIR = $(GUROBI_PATH)/$(HOST_ARCH)/lib


LIBS = -lgurobi_c++ -lgurobi75 -lm -D_GLIBCXX_USE_CXX11_ABI=0 -m64 -w

CFLAGS = -I . -I ./src  -I /usr/local/include/ -I$(GUROBI_INCLUDEDIR) -g -O3 -std=c++11

LINK_FLAGS = -g -L ./ -L /usr/local/lib/ -L$(GUROBI_LIBDIR)

OBJS = ./src/propagate_intervals.o ./src/network_computation.o ./src/gurobi_interface.o ./src/configuration.o


run_file: main.o $(OBJS)
	g++ -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)


%.o: %.cc
	$(CXX) -O3 -c $(CFLAGS) $(LIBS) -o $@ $<
%.o: %.cpp
	$(CXX) -O3 -c $(CFLAGS) $(LIBS) -o $@ $<
%.o: %.c
	$(CXX) -O3 -c $(CFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f ./src/*.o *.o ./run_file
