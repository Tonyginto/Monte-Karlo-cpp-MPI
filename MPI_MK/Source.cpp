#include <stdio.h>
#include "mpi.h"
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <cmath>
#include <vector>

using namespace std;

int allRandom(unsigned int &seed);
float RndKoeff(vector<vector<float>> ar, int a, int b, int an, int bn, unsigned int &seed);
float g(int a, int b);

int main(int argc, char **argv)
{
	int an = 10, bn = 10;
	long int N = 1000000;
	int in = 5, jn = 5;

	vector<vector<float>> ar(an, vector <float>(bn));
	for (int i = 0; i < an; i++) {
		for (int j = 0; j < bn; j++) {
			if (i != 0 && i != an - 1 && j != 0 && j != bn - 1) {
				ar[i][j] = 0;
			}
			else {
				ar[i][j] = g(i, j);
			}
		}
	}

	float rnk = 0, ret = 0;

	int rank, commSize;

	double beg = clock();

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	MPI_Status status;

	int fr = rank * N / commSize;
	int to = (rank + 1) * N / commSize;

	unsigned int seed = rank + 1563;
	for (int k = fr; k < to; k++) {
		rnk += RndKoeff(ar, in, jn, an, bn, seed);
	}
	rnk = rnk / N;

	MPI_Reduce(&rnk, &ret, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Finalize();

	if (rank == 0) {
		cout << "time: " << (clock() - beg) / 1000.0 << "\n\n";
		cout << ret << "\n";
	}

	/*cout << "time: " << (clock() - beg) / 1000.0 << "\n\n";
	cout << "solution: " << ret << "\n";*/
}

int allRandom(unsigned int &seed) {
	seed = (8253729 * seed + 2396403);

	// Берем стартовое число и возвращаем значение в диапазоне от 0 до 3
	return (seed * 32768) / 32767 % 4;

}

float RndKoeff(vector<vector<float>> ar, int a, int b, int an, int bn, unsigned int &seed) {
	while (a != 0 && a != an - 1 && b != 0 && b != bn - 1) {
		int rnd = allRandom(seed);
		if (rnd == 0) {
			a--;
		}
		if (rnd == 1) {
			a++;
		}
		if (rnd == 2) {
			b--;
		}
		if (rnd == 3) {
			b++;
		}
	}
	return ar[a][b];
}

float g(int a, int b) {
	return sin(a + 3.1415 / 2) * sin(3 * b + 3.1415 / 2);
}