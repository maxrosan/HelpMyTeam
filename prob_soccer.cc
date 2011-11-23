
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <string>
#include <map>

#define N 20

uint32_t A, B;

uint8_t lib[4], point[N];
uint64_t count_camp[N], count_lib[N], n_poss = 0;

using namespace std;

void max_arr(uint8_t points[N], uint8_t arr[10], uint8_t n) {
	uint8_t k, l, l_min;
	arr[0] = l_min = 0;
	for (k = 1; k < N; k++) {
		if (points[k] > points[arr[0]]) arr[0] = k;
		if (points[k] < points[l_min]) l_min = k;
	}

	if (n == 1)
		return;

	for (k = 1; k < n; k++) {
		arr[k] = l_min;
		for (l = 0; l < N; l++) {
			if (points[l] > points[arr[k]] && points[l] < points[arr[k-1]]) {
				arr[k] = l;
			}
		}
	}
}

#if 0
void calc(uint8_t points[N], uint8_t matches[][N>>1][2], uint8_t Nrods, uint8_t rod) {
	uint8_t modpoints[N];

	if (rod >= Nrods) {
		max_arr(points, lib, 4);

		count_camp[lib[0]]++;

		count_lib[lib[0]]++;
		count_lib[lib[1]]++;
		count_lib[lib[2]]++;
		count_lib[lib[3]]++;

		n_poss++;

		return;
	}

	for (x = 0; x < (1 << N); x++) {
		for (y = 0; y < (1 << N); y++) {

			if ((x & y) != 0) continue;

			memcpy(modpoints, points, N);

			for (i = 0; i < (N>>1); i++) {
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

			calc(modpoints, matches, Nrods, rod+1);
		}
	}
}
#endif

void calc_lim(uint8_t points[N], uint8_t matches[][N>>1][2], uint8_t Nrods, uint8_t rod, uint8_t lim) {
	uint8_t modpoints[N];
	uint32_t x, y, i;

	if (rod >= Nrods) {
		max_arr(points, lib, 4);

		count_camp[lib[0]]++;

		count_lib[lib[0]]++;
		count_lib[lib[1]]++;
		count_lib[lib[2]]++;
		count_lib[lib[3]]++;

		n_poss++;

		return;
	}

	for (x = 0; x < (1 << lim); x++) {
		for (y = 0; y < (1 << lim); y++) {

			if (x & y) continue;

			memcpy(modpoints, points, N);

			for (i = 0; i < (N>>1); i++) {

				if(matches[rod][i][0] == 0 && matches[rod][i][1] == 0) continue;

				A = x & (1 << i);
				B = y & (1 << i);

				//printf("%d x %d => %d%d\n", matches[rod][i][0], matches[rod][i][1], !!A, !!B);

				if (!A && !B) {
					modpoints[matches[rod][i][0]] += 1;
					modpoints[matches[rod][i][1]] += 1;
				} else if (A) {
					modpoints[matches[rod][i][0]] += 3;
				} else {
					modpoints[matches[rod][i][1]] += 3;
				}
			}

			calc_lim(modpoints, matches, Nrods, rod+1, lim);
		}
	}
}

int main() {

	uint8_t rods;
	char name[100], name1[100], vs;
	map<string, uint8_t> teams;
	string names[N];
	uint8_t lim = 3;
	uint32_t i, x;

	bzero(count_camp, N);
	bzero(count_lib, N);

	printf("Times: \n");

	for (i = 0; i < N; i++) {
		scanf("%s %d", name, &point[i]);
		printf("%s %d\n", name, point[i]);
		teams[string(name)] = i;
		names[i] = string(name);

	}

	scanf("%d", &rods);

	uint8_t match[rods][N>>1][2];

	bzero(match, sizeof match);

	for (i = 4; i < N; i++) {
		if ((point[i] + rods*3) >= point[3]) lim = i;
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

	//FIXME: Ler os times garantidos já para a Libertadores


	calc_lim(point, match, rods, 0, lim);

	printf("\n\n Time | Camp. | Lib. \n");
	printf("NP = %lu\n", n_poss);

	for (i = 0; i <= lim; i++) {
		printf("%s %.2lf %.2lf\n", names[i].c_str(), 100.*((double) count_camp[i])/((double) n_poss), 100.*((double) count_lib[i])/((double) n_poss));
	}

}
