CXX = g++
LIBS = -lgurobi_c++ -lgurobi75 -lm -D_GLIBCXX_USE_CXX11_ABI=0 -m64 -w
CFLAGS = -I . -I ./headers -I /usr/local/include/ -I /opt/gurobi751/linux64/include/ -g -O3 -std=c++11
LINK_FLAGS = -g -L ./ -L /usr/local/lib/ -L /opt/gurobi751/linux64/lib/

OBJS = ./headers/propagate_intervals.o ./headers/network_computation.o ./headers/gurobi_interface.o ./headers/configuration.o


run_file: main.o $(OBJS)
	g++ -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)


%.o: %.cc
	$(CXX) -O3 -c $(CFLAGS) $(LIBS) -o $@ $<
%.o: %.cpp
	$(CXX) -O3 -c $(CFLAGS) $(LIBS) -o $@ $<
%.o: %.c
	$(CXX) -O3 -c $(CFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f ./headers/*.o *.o
