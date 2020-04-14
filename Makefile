CXX = g++


include Makefile.locale
GUROBI_INCLUDEDIR=$(strip $(GUROBI_PATH))/$(strip $(HOST_ARCH))/include/
GUROBI_LIBDIR=$(strip $(GUROBI_PATH))/$(strip $(HOST_ARCH))/lib/

#-D_GLIBCXX_USE_CXX11_ABI=0
LIBS = -lgurobi_c++ -lgurobi80 -lflowstar -lmpfr -lgmp -lgsl -lgslcblas -lm -lglpk -lmpfi \
 -m64 -lprotobuf -w -pthread  -std=c++17

CXXFLAGS = -MMD -I . -I ./src/ -I ./src/neural_rule_analysis -I ./eigen_file/  -I /usr/local/include/ \
-I $(GUROBI_INCLUDEDIR) -I ./flowstar-release/ -g -O3 -std=c++11

LINK_FLAGS = -g -L ./ -L /usr/local/lib/ -L $(GUROBI_LIBDIR) -L ./flowstar-release/

OBJS = ./src/onnx.pb.o ./src/sherlock_message.pb.o ./src/sherlock.o ./src/network_computation.o \
./src/configuration.o ./src/nodes.o ./src/computation_graph.o ./src/region_constraints.o \
./src/generate_constraints.o ./src/network_signatures.o ./src/gurobi_interface.o  \
./src/parsing_onnx.o ./src/selective_binarization.o ./src/image_handler.o \
./src/neural_rule_analysis/AffineArithmeticExpression.o ./src/neural_rule_analysis/AffineArithmeticNoiseSymbols.o \
./src/neural_rule_analysis/Box.o ./src/neural_rule_analysis/Monomial.o \
./src/neural_rule_analysis/mpfiWrapper.o ./src/neural_rule_analysis/neuralRuleAnalysisInterfaceMain.o \
./src/neural_rule_analysis/Polynomial.o ./src/neural_rule_analysis/PolynomialApproximator.o \
./src/neural_rule_analysis/Tiling.o ./src/neural_rule_analysis/RangeToVariables.o \
./src/compute_flowpipes.o ./src/sherlock_poly.o ./src/polynomial_computations.o

DEPENDS = ${OBJECTS:.o=.d}


all: libs run_file
interface_python: libs python_interface
flocking : libs drone
dynamical_systems: libs ./ODE_reach_tubes/Ex_1
test_rule : libs ./ODE_reach_tubes/test_rule
test_1: libs ./tests/test_1
test_2: libs ./tests/test_2
test_3: libs ./tests/test_3
test_3_poly: libs ./tests/test_3_poly
test_4: libs ./tests/test_4
test_5: libs ./tests/test_5
test_6: libs ./tests/test_6
test_7: libs ./tests/test_7
test_8: libs ./tests/test_8

# protoc -I=./src/ --cpp_out=./src/ ./src/onnx.proto

libs: $(OBJS)
	ar rcs ./src/libsherlock.a $(OBJS)
	ranlib ./src/libsherlock.a
	cp ./src/*.h ./include
	cp python_interface ./sherlock_python_interface
	cp ./src/sherlock_message_pb2.py ./sherlock_python_interface

run_file: main.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

python_interface: ./src/python_interface.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

drone: \
	./systems_with_networks/flocking_controller/drone.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

./tests/test_1: ./tests/test_1.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

./tests/test_2: ./tests/test_2.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

./tests/test_3: ./tests/test_3.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

./tests/test_3_poly: ./tests/test_3_poly.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

./tests/test_4: ./tests/test_4.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

./tests/test_5: ./tests/test_5.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

./tests/test_6: ./tests/test_6.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

./tests/test_7: ./tests/test_7.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

./tests/test_8: ./tests/test_8.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)


./ODE_reach_tubes/Ex_1: ./ODE_reach_tubes/Ex_1.o $(OBJS)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

./ODE_reach_tubes/test_rule: ./src/neural_rule_analysis/main.o $(OBJS_1)
	$(CXX) -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

%.o: %.cc
	$(CXX) -O3 -c $(CXXFLAGS) -o $@ $< $(LIBS)
%.o: %.cpp
	$(CXX) -O3 -c $(CXXFLAGS) -o $@ $< $(LIBS)
%.o: %.c
	$(CXX) -O3 -c $(CXXFLAGS) -o $@ $< $(LIBS)
%.pb.o: %.proto
	protoc -I=./src/ --cpp_out=./src/ ./src/onnx.proto
	protoc -I=./src/ --cpp_out=./src/ --python_out=./src/ ./src/sherlock_message.proto
	$(CXX) -O3 -c $(CXXFLAGS) -o ./src/onnx.pb.o ./src/onnx.pb.cc $(LIBS)
	$(CXX) -O3 -c $(CXXFLAGS) -o ./src/sherlock_message.pb.o ./src/sherlock_message.pb.cc $(LIBS)

clean:
	rm -f ./src/*.o ./src/neural_rule_analysis/*.o *.o ./run_file ./lib/* ./include/*.h \
	./ODE_reach_tubes/*.o ./tests/*.o ./tests/test_1 ./tests/test_2 ./tests/test_3 \
	./tests/test_3_poly ./tests/test_4 ./tests/test_5 ./tests/test_6 ./tests/test_7 \
	./src/*.pb.* ./tests/test_8

-include ${DEPENDS}
