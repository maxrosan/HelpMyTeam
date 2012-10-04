
NTHREADS = 4
FLAGS = -DNTHREADS=4 -ggdb -fopenmp -O2

all: demoted

demoted: prob_reb.o
	g++ $? $(FLAGS) -lgmp -lgmpxx -lgomp -o $@

%.o: %.cc
	g++ $(FLAGS) -c $^ -o $@

clean:
	rm *.o
