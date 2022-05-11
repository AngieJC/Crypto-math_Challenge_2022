#include "CryptographicAlgorithm.h"
#include <mpi.h>
#include <iostream>
#include <map>
#include <cmath>
#include <cstring>

#define diffInL		0b0000000000000000
#define diffInR		0b1000000000000000
#define L0			0b0000000100100011
#define R0			0b0100010101100111

using namespace std;

int map2File(string fname, map<uint64_t, uint32_t>* diffs);

int main(int argc, char** argv) {
	if (argc == 1) {
		cout << "��Ҫ������MPI�����£�" << endl;
		return 0;
	}
	else if (argc != 2) {
		cout << "���з�ʽ��mpiexec -n [������] ./a.out [����]" << endl;
		return 0;
	}

	// ��ȡ��Ҫ����������
	int r = atoi(argv[1]);

	// ��ʼ�����ĶԺ���Կ
	uint16_t p[2] = { L0, R0 }, p_[2] = {p[0] ^ diffInL, p[1] ^ diffInR}, c[2] = {0}, c_[2] = {0};
	uint16_t seedkey[4] = { 0 };
	uint64_t SeedKey = 0;

	// ��ʼ��MPI
	int world_size, world_rank;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	// ����ÿ��������Ҫ��������Կ�ռ�
	uint64_t v = pow(2, ((int)((r + 1) / 2 ) < 4 ? (int)((r + 1) / 2) : 4) * 16);
	// map<uint64_t, uint32_t> diffs; // <��Կ, ������>
	char filename[32] = { 0 };
	sprintf(filename, "r%d_th%d.csv", r, world_rank);
	FILE* fp = fopen(filename, "w");
	fprintf(fp, "k[3..0], ������\n");
	uint32_t diff = 0;
	for (SeedKey = world_rank * (v / world_size); SeedKey < (world_rank + 1) * (v / world_size); SeedKey++) {
		memcpy((uint16_t*) & seedkey[0], ((uint16_t*) & SeedKey) + 0, 2);
		memcpy((uint16_t*) & seedkey[1], ((uint16_t*) & SeedKey) + 1, 2);
		memcpy((uint16_t*) & seedkey[2], ((uint16_t*) & SeedKey) + 2, 2);
		memcpy((uint16_t*) & seedkey[3], ((uint16_t*) & SeedKey) + 3, 2);
		// ����
		Enc(p, c, seedkey, r);
		Enc(p_, c_, seedkey, r);
		// diffs[SeedKey] = pow(2, 16) * (c[0] ^ c_[0]) + (c[1] ^ c_[1]);
		diff = pow(2, 16) * (c[0] ^ c_[0]) + (c[1] ^ c_[1]);
		fprintf(fp, "%016X, %08X\n", SeedKey, diff);
	}
	fclose(fp);

	// д���ı�
	/*char filename[32] = { 0 };
	sprintf(filename, "r%d_th%d.csv", r, world_rank);
	map2File(filename, &diffs);*/

	// ����ͬ��
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	// cout << "����" << world_rank << "�˳�" << endl;

	return 0;
}

int map2File(string fname, map<uint64_t, uint32_t>* diffs) {
	if (diffs->empty()) {
		return 0;
	}

	FILE* fp = fopen(fname.c_str(), "w");
	if (!fp) {
		return -errno;
	}
	fprintf(fp, "k[3..0], ������\n");
	for (map<uint64_t, uint32_t>::iterator it = diffs->begin(); it != diffs->end(); it++) {
		fprintf(fp, "%016X, %08X\n", it->first, it->second);
	}
	fclose(fp);

	return 0;
}