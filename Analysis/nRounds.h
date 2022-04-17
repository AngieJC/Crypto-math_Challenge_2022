#pragma once
#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>
#include <algorithm>
#include <set>
#include <bitset>
#include <time.h>
#include <cmath>
#include <chrono>

#include "gurobi_c++.h"

using namespace std;
using namespace std::chrono;

void analysisnRounds(int r) {
	uint16_t L0 = 0, R0 = 0, Lr = 0, Rr = 0;
	uint32_t c = 0;
	cout << "����pt[0], pt[1](ʮ������)��";
	cin.setf(ios_base::hex, ios_base::basefield);
	cin >> L0 >> R0;
	cout << "��������(ʮ������)��";
	cin >> c;
	// �����Ĳ���Lr��Rr
	memcpy(&Rr, ((uint16_t*)&c), 2);
	memcpy(&Lr, ((uint16_t*)&c + 1), 2);

	try {
		// MILP����
		GRBEnv env = GRBEnv(true);
		env.set("LogFile", "nRounds.log");
		env.set(GRB_DoubleParam_PoolGap, GRB_INFINITY);
		env.start();

		// ������ģ��
		GRBModel model = GRBModel(env);

		/* ��ʼ���� */
		// ��Կ��չ
		// ����r�ּ��ܣ�����Ҫ(int)(r / 2) + r % 2��K
		vector<GRBVar> K(((int)(r / 2) + r % 2) * 16);
		for (int i = 0; i < ((int)(r / 2) + r % 2) * 16; i++) {
			K[i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
		}
		// K_i+4 = ~K_i + i
		for (int j = 4 * 16; j < ((int)(r / 2) + r % 2) * 16; j += 16) {
			bitset<16> i = (j % 16) - 4;
			for (int k = 0; k < 16; k++) {
				model.addConstr(K[j + k] == (1 - K[j - 4 * 16 + k]) + (int)i[k]);
			}
		}

		vector<GRBVar> X(144 * (r + 1));
		// L0, R0��32������������Լ��
		bitset<16> L0_bit = L0, R0_bit = R0;
		for (int i = 15; i >= 0; i--) { // Ϊ���ֽṹһ�£�������룬��ʼ������ǰ7*16��λ�ò�û�����壬ֻ��ռλ
			X[7 * 16 + 15 - i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			model.addConstr(X[7 * 16 + 15 - i] == (int)L0_bit[i]);
			X[8 * 16 + 15 - i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			model.addConstr(X[8 * 16 + 15 - i] == (int)R0_bit[i]);
		}


		/* ����di��eiʱ�漰3�����������ֱ���Ҫr*16���м���� */
		vector<GRBVar> D(16 * r);
		vector<GRBVar> E(16 * r);
		for (int i = 0; i < 16 * r; i++) {
			D[i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			E[i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
		}


		/* ÿ�ֱ��� */
		bitset<16> i_1;
		for (int i = 1; i <= r; i++) {
			i_1 = i - 1;
			// ���ִ���MILP��������ʼ����Ϊi*144
			// ai, bi, ci, di, ei, fi, RK_i-1, Li, Ri��144������
			for (int j = 0; j < 144; j++) {
				X[i * 144 + j] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			}

			for (int j = 0; j < 16; j++) {
				// RK_2i = K_i, RK_2i+1 = ~K_i
				if (i % 2 == 1) { // �����֣�RK_2i = K_i
					model.addConstr(X[i * 144 + 96 + j] == K[((int)((i - 1) / 2)) * 16 + j]);
				}
				else // ż���֣�RK_2i+1 = ~K_i
				{
					model.addConstr(X[i * 144 + 96 + j] == 1 - K[((int)(i / 2) - 1) * 16 + j]);
				}
				// ai = L_i-1 & RK_i-1
				model.addQConstr(X[i * 144 + 0 * 16 + j] == X[7 * 16 + (i - 1) * 144 + j] * X[i * 144 + 96 + j]);
				// bi = R_i-1 + (i - 1)
				model.addConstr(X[i * 144 + 1 * 16 + j] >= X[8 * 16 + (i - 1) * 144 + j] - i_1[j]);
				model.addConstr(X[i * 144 + 1 * 16 + j] >= i_1[j] - X[8 * 16 + (i - 1) * 144 + j]);
				model.addConstr(X[i * 144 + 1 * 16 + j] <= X[8 * 16 + (i - 1) * 144 + j] + i_1[j]);
				model.addConstr(X[i * 144 + 1 * 16 + j] <= 2 - X[8 * 16 + (i - 1) * 144 + j] - (int)i_1[j]);
				// ci = ai + bi
				model.addConstr(X[i * 144 + 2 * 16 + j] >= X[i * 144 + 1 * 16 + j] - X[i * 144 + 0 * 16 + j]);
				model.addConstr(X[i * 144 + 2 * 16 + j] >= X[i * 144 + 0 * 16 + j] - X[i * 144 + 1 * 16 + j]);
				model.addConstr(X[i * 144 + 2 * 16 + j] <= X[i * 144 + 1 * 16 + j] + X[i * 144 + 0 * 16 + j]);
				model.addConstr(X[i * 144 + 2 * 16 + j] <= 2 - X[i * 144 + 1 * 16 + j] - X[i * 144 + 0 * 16 + j]);
				// di = ((ci << 2) & ((ci << 1) + ci
				if (j < 2) {
					model.addConstr(X[i * 144 + 3 * 16 + j] == X[i * 144 + 2 * 16 + j]);
				}
				else {
					model.addQConstr(D[(i - 1) * 16 + j] == X[i * 144 + 2 * 16 + j - 1] * X[i * 144 + 2 * 16 + j - 2]);
					model.addConstr(X[i * 144 + 3 * 16 + j] >= D[(i - 1) * 16 + j] - X[i * 144 + 2 * 16 + j]);
					model.addConstr(X[i * 144 + 3 * 16 + j] >= X[i * 144 + 2 * 16 + j] - D[(i - 1) * 16 + j]);
					model.addConstr(X[i * 144 + 3 * 16 + j] <= D[(i - 1) * 16 + j] + X[i * 144 + 2 * 16 + j]);
					model.addConstr(X[i * 144 + 3 * 16 + j] <= 2 - D[(i - 1) * 16 + j] - X[i * 144 + 2 * 16 + j]);
				}
				// ei = (di <<< 3) + (di <<< 9) + (di <<< 14)
				model.addConstr(E[(i - 1) * 16 + j] >= X[i * 144 + 3 * 16 + ((16 - 3 + j) % 16)] + X[i * 144 + 3 * 16 + ((16 - 9 + j) % 16)]);
				model.addConstr(E[(i - 1) * 16 + j] >= X[i * 144 + 3 * 16 + ((16 - 9 + j) % 16)] - X[i * 144 + 3 * 16 + ((16 - 3 + j) % 16)]);
				model.addConstr(E[(i - 1) * 16 + j] <= X[i * 144 + 3 * 16 + ((16 - 3 + j) % 16)] + X[i * 144 + 3 * 16 + ((16 - 9 + j) % 16)]);
				model.addConstr(E[(i - 1) * 16 + j] <= 2 - X[i * 144 + 3 * 16 + ((16 - 3 + j) % 16)] - X[i * 144 + 3 * 16 + ((16 - 9 + j) % 16)]);
				model.addConstr(X[i * 144 + 4 * 16 + j] >= E[(i - 1) * 16 + j] - X[i * 144 + 3 * 16 + ((16 - 14 + j) % 16)]);
				model.addConstr(X[i * 144 + 4 * 16 + j] >= X[i * 144 + 3 * 16 + ((16 - 14 + j) % 16)] - E[(i - 1) * 16 + j]);
				model.addConstr(X[i * 144 + 4 * 16 + j] <= E[(i - 1) * 16 + j] + X[i * 144 + 3 * 16 + ((16 - 14 + j) % 16)]);
				model.addConstr(X[i * 144 + 4 * 16 + j] <= 2 - E[(i - 1) * 16 + j] - X[i * 144 + 3 * 16 + ((16 - 14 + j) % 16)]);
				// fi = ei & RK_i-1
				model.addQConstr(X[i * 144 + 5 * 16 + j] == X[i * 144 + 4 * 16 + j] * X[i * 144 + 96 + j]);
				// Li = fi + R_i-1
				model.addConstr(X[i * 144 + 7 * 16 + j] >= X[i * 144 + 5 * 16 + j] - X[(i - 1) * 144 + 8 * 16 + j]);
				model.addConstr(X[i * 144 + 7 * 16 + j] >= X[(i - 1) * 144 + 8 * 16 + j] - X[i * 144 + 5 * 16 + j]);
				model.addConstr(X[i * 144 + 7 * 16 + j] <= X[i * 144 + 5 * 16 + j] + X[(i - 1) * 144 + 8 * 16 + j]);
				model.addConstr(X[i * 144 + 7 * 16 + j] <= 2 - X[i * 144 + 5 * 16 + j] - X[(i - 1) * 144 + 8 * 16 + j]);
				// Ri = ei + L_i-1
				model.addConstr(X[i * 144 + 8 * 16 + j] >= X[i * 144 + 4 * 16 + j] - X[(i - 1) * 144 + 7 * 16 + j]);
				model.addConstr(X[i * 144 + 8 * 16 + j] >= X[(i - 1) * 144 + 7 * 16 + j] - X[i * 144 + 4 * 16 + j]);
				model.addConstr(X[i * 144 + 8 * 16 + j] <= X[i * 144 + 4 * 16 + j] + X[(i - 1) * 144 + 7 * 16 + j]);
				model.addConstr(X[i * 144 + 8 * 16 + j] <= 2 - X[i * 144 + 4 * 16 + j] - X[(i - 1) * 144 + 7 * 16 + j]);
			}
		}

		/* ����Լ�� */
		// Lr|Rr = c
		bitset<16> Lr_bit = Lr, Rr_bit = Rr;
		for (int i = 15; i >= 0; i--) {
			model.addConstr(X[144 * r + 7 * 16 + 15 - i] == (int)Lr_bit[i]);
			model.addConstr(X[144 * r + 8 * 16 + 15 - i] == (int)Rr_bit[i]);
		}

		// ���
		model.write("nRounds.lp");
		auto start = high_resolution_clock::now();
		model.optimize();
		auto stop = high_resolution_clock::now();

		int solCount = model.get(GRB_IntAttr_SolCount);
		if (solCount > 0) {
			for(int i = 0; i < solCount; i++)
			model.set(GRB_IntParam_SolutionNumber, i);
			bitset<16> K0, K1, K2, K3;
			for (int j = 0; j < 16; j++) {
				K0[15 - j] = 0;
				K0[16 + 15 - j] = 0;
				K0[16 * 2 + 15 - j] = 0;
				K0[16 * 3 + 15 - j] = 0;
				if (K[j].get(GRB_DoubleAttr_Xn) > 0.5) {
					K0[15 - j] = 1;
				}
				if (r > 2) {
					if (K[16 + j].get(GRB_DoubleAttr_Xn) > 0.5) {
						K0[16 + 15 - j] = 1;
					}
					if (r > 4) {
						if (K[16 * 2 + j].get(GRB_DoubleAttr_Xn) > 0.5) {
							K0[16 * 2 + 15 - j] = 1;
						}
						if (r > 8) {
							if (K[16 * 3 + j].get(GRB_DoubleAttr_Xn) > 0.5) {
								K0[16 * 3 + 15 - j] = 1;
							}
						}
					}
				}
			}
			printf("K0��0X%04X\t", K0);
			printf("K1��0X%04X\t", K1);
			printf("K2��0X%04X\t", K2);
			printf("K3��0X%04X\t\n", K3);
		}

		auto duration = duration_cast<microseconds>(stop - start);
		cout << "Time: " << duration.count() / 1000000 << "s" << endl;
	}
	catch (GRBException e) {
		cout << "Gurobi�����룺" << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "δ֪����" << endl;
	}
	

	return;
}