
NTHREADS = 3
FLAGS = -DNTHREADS=$(NTHREADS) -ggdb -fopenmp -O3

all: calculate_chance

calculate_chance: prob_reb.o
	g++ $? $(FLAGS) -lgmp -lgmpxx -lgomp -o $@

%.o: %.cc
	g++ $(FLAGS) -c $^ -o $@

clean:
	rm *.o
