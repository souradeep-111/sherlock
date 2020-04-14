CXX = g++
include makefile.local
# -D_GLIBCXX_USE_CXX11_ABI=0
LIBS = -lmpfr -lgmp -lgsl -lgslcblas -lm -lglpk
CFLAGS = -I . -I $(GMP_HOME) -g -O3 -std=c++11
LINK_FLAGS = -g -L $(GMP_LIB_HOME)
OBJS = Interval.o Matrix.o Polynomial.o Monomial.o Constraints.o Geometry.o TaylorModel.o Continuous.o Hybrid.o expression.o

all: flowstar lib

flowstar: $(OBJS) lex.yy.o modelParser.tab.o modelParser.o
	g++ -O3 -w $(LINK_FLAGS) -o $@ $^ $(LIBS)

lib: $(OBJS) lex.yy.o modelParser.tab.o
	ar rcs libflowstar.a $^

# -D_GLIBCXX_USE_CXX11_ABI=0

%.o: %.cc
	$(CXX) -O3 -c $(CFLAGS) -o $@ $<
%.o: %.cpp
	$(CXX) -O3 -c $(CFLAGS) -o $@ $<
%.o: %.c
	$(CXX) -O3 -c $(CFLAGS) -o $@ $<

modelParser.tab.c: modelParser.y
	bison -d -v modelParser.y

lex.yy.c: modelLexer.l modelParser.tab.c
	flex modelLexer.l

clean:
	rm -f flowstar *.o libflowstar.a *~ modelParser.tab.c modelParser.tab.h modelParser.output lex.yy.c
