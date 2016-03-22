
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <string>
#include <map>
#include <algorithm>
#include <gmp.h>
#include <omp.h>
#include <assert.h>

using namespace std;

#define N 20

uint32_t A, B;

uint8_t lib[N], point[N];
mpz_t count_reb[N], n_poss;
omp_lock_t lock_poss;
string names[N];

void calc_lim(uint8_t points[N], uint8_t matches[][N>>1][2], uint8_t Nrods, uint8_t rod, uint8_t lim, bool thread_div);

void init() {
	uint8_t i;

	mpz_init(n_poss);

	for (i = 0; i < N; i++) {
		mpz_init(count_reb[i]);
	}

	omp_init_lock(&lock_poss);
}

void min_arr(uint8_t points[N], uint8_t arr[N]) {
	uint8_t k, l, l_min;
	uint32_t num_used = 0;

	arr[0] = l_min = 0;
	
	assert(N < 32);

	for (k = 1; k < N; k++) {
		if (points[k] <= points[arr[0]]) arr[0] = k;
		if (points[k] >= points[l_min]) l_min = k;
	}

	num_used |= (1 << arr[0]);

	for (k = 1; k < N; k++) {

		arr[k] = l_min;

		for (l = 0; l < N; l++) {
			if (points[l] <= points[arr[k]] && !( (1 << l) & num_used )) {
				arr[k] = l;
			}
		}

		num_used |= (1 << arr[k]);
	}
}

inline static void loop(int i, int x, uint8_t matches[][N>>1][2], uint8_t Nrods, uint8_t rod, uint8_t lim, uint8_t modpoints[N], uint8_t points[N]) {

	uint16_t y;

	for (y = 0; y < (1 << lim); y++) {

		/*
		 A | B | Result
		 0 | 0 |  draw
		 0 | 1 | B wins
		 1 | 0 | A wins
		 1 | 1 | not valid result
		*/
		if (x & y) continue;

		memcpy(modpoints, points, N);

		for (i = 0; i < (N>>1); i++) {

			if(matches[rod][i][0] == 0 && matches[rod][i][1] == 0) continue;

			A = x & (1 << i);
			B = y & (1 << i);

			if (!A && !B) {
				modpoints[matches[rod][i][0]] += 1;
				modpoints[matches[rod][i][1]] += 1;
			} else if (A) {
				modpoints[matches[rod][i][0]] += 3;
			} else {
				modpoints[matches[rod][i][1]] += 3;
			}
		}

		calc_lim(modpoints, matches, Nrods, rod+1, lim, false);
	}

}

void calc_lim(uint8_t points[N], uint8_t matches[][N>>1][2], uint8_t Nrods, uint8_t rod, uint8_t lim, bool thread_div) {
	uint8_t modpoints[N];
	uint32_t x, i;

	if (rod >= Nrods) {

		omp_set_lock(&lock_poss);
		min_arr(points, lib);

		for (i = 0; i < 4; i++) {
			mpz_add_ui(count_reb[lib[i]], count_reb[lib[i]], 1);
		}

		for (i = 4; (i < N) && (points[lib[i]] == points[lib[3]]); i++) {
			mpz_add_ui(count_reb[lib[i]], count_reb[lib[i]], 1);
		}

		mpz_add_ui(n_poss, n_poss, 1);
		omp_unset_lock(&lock_poss);

	} else {

		if (thread_div) {
			#pragma omp parallel for private(x,i,modpoints) num_threads(NTHREADS)
			for (x = 0; x < (1 << lim); x++) {
				loop(i, x, matches, Nrods, rod, lim, modpoints, points);
			}

		} else {
			for (x = 0; x < (1 << lim); x++)
				loop(i, x, matches, Nrods, rod, lim, modpoints, points);
		}

	}
}

uint8_t match[N*N*2][N>>1][2];
int sort_team[N];

static int compare(const void *a, const void *b) {
	int aI = *((int*) a);
	int bI = *((int*) b);
	return mpz_cmp(count_reb[aI], count_reb[bI]);
}

int main() {

	uint8_t rods;
	char name[100], name1[100], vs;
	map<string, uint8_t> teams;
	uint8_t lim = 3;
	uint32_t x;
	uint32_t i;

	init();
	//bzero(count_reb, N);

	printf("Times: \n");

	for (i = 0; i < N ; i++) {
		scanf("%s %u", name, &point[i]);
		printf("%s %u\n", name, point[i]);
		teams[string(name)] = i;
		names[i] = string(name);
	}

	scanf("%d", &rods);

	bzero(match, sizeof match);

	reverse(point, point+N);

	// It avoids unnecessary calculation. 
	for (i = 4; i < N; i++) {
		if ((point[3] + rods*3) >= point[i]) {
			lim = i;
		}
	}

	for (i = 0; i < rods; i++) {
		for (x = 0; x < (N >> 1); x++) {
			scanf("%s %c %s", name, &vs, name1);
			
			if (teams[string(name)] > lim && teams[string(name1)] > lim) continue;

			match[i][x][0] = teams[string(name)];
			match[i][x][1] = teams[string(name1)];

			printf("%s[%d] vs. %s[%d]\n", name, teams[string(name)], name1, teams[string(name1)]);
		}

		printf("####\n");
	}

	//n_poss = 1;
	mpz_set_ui(n_poss, 1);

	calc_lim(point, match, rods, 0, lim, true);

	printf("\n\n Team | Chance of being relegated \n");
	
	printf("NP = ");
	mpz_out_str(stdout, 10, n_poss);
	printf("\n");

	for (i = 0; i < N; i++) {
		sort_team[i] = i;
	}

	qsort(sort_team, N, sizeof(int), compare);

	for (i = 0; i < N; i++) {
		int J = sort_team[i];
		mpz_mul_ui(count_reb[J], count_reb[J], 100);
		mpz_cdiv_q(count_reb[J], count_reb[J], n_poss);

		printf("%s ", names[N-J-1].c_str());
		mpz_out_str(stdout, 10, count_reb[J]);
		printf("% \n");
		//printf("%s %.2lf\n", names[N-i-1].c_str(), 100.*((double) count_reb[i])/((double) n_poss));
	}

}
