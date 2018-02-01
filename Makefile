CFLAG_1 := -w
CLFAG_2 := -lgurobi_c++
CFLAG_3 := -lgurobi75
CFLAG_4 := -std=c++11
CFLAG_5 := -lm
CFLAG_6 := -D_GLIBCXX_USE_CXX11_ABI=0
CFLAG_7 := -lglpk

LFLAG_1 := -L /usr/local/lib/
LFLAG_2 := -L ./
LFLAG_3 := -L /opt/gurobi751/linux64/lib/

IFLAG_1 := -I ./headers
IFLAG_2 := -I /usr/local/include/
IFLAG_3 := -I /opt/gurobi751/linux64/include/

headers_path := ./headers/

run_file: main.o propagate_intervals.o network_computation.o \
  gurobi_interface.o
	g++  $(headers_path)propagate_intervals.o $(headers_path)network_computation.o \
  $(headers_path)gurobi_interface.o $(headers_path)main.o\
	 -o run_file $(LFLAG_3) -lgurobi_c++ $(CFLAG_3) $(CFLAG_4) \
	 $(CFLAG_5) $(CFLAG_6)

main.o: main.cpp
	g++ -o $(headers_path)main.o -c main.cpp $(IFLAG_2) $(IFLAG_3) $(CFLAG_4)

propagate_intervals.o: $(headers_path)propagate_intervals.cpp $(headers_path)propagate_intervals.h
	g++ -o $(headers_path)propagate_intervals.o -c $(headers_path)propagate_intervals.cpp $(IFLAG_2) $(IFLAG_3) $(CFLAG_4)

network_computation.o: $(headers_path)network_computation.cpp $(headers_path)network_computation.h
	g++ -o $(headers_path)network_computation.o -c $(headers_path)network_computation.cpp $(IFLAG_3) $(CFLAG_4)

gurobi_interface.o: $(headers_path)gurobi_interface.cpp $(headers_path)gurobi_interface.h
	g++ -m64 -g -o $(headers_path)gurobi_interface.o -c $(headers_path)gurobi_interface.cpp \
	$(IFLAG_3) $(CFLAG_6)

clean:
	rm -f run_file ./headers/main.o ./headers/propagate_intervals.o ./headers/network_computation.o ./headers/gurobi_interface.o
