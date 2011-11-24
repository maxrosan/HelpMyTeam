
FLAGS = -fopenmp -O2

all: demoted

demoted: prob_reb.o
	g++ $? $(FLAGS) -lgomp -o $@

%.o: %.cc
	g++ $(FLAGS) -c $^ -o $@

clean:
	rm *.o
