#pragma once
#include <iostream>
#include <fstream>
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
	cout << "输入pt[0], pt[1](十六进制)：";
	cin.setf(ios_base::hex, ios_base::basefield);
	cin >> L0 >> R0;
	cout << "输入密文(十六进制)：";
	cin >> c;
	// 将密文拆解成Lr和Rr
	memcpy(&Rr, ((uint16_t*)&c), 2);
	memcpy(&Lr, ((uint16_t*)&c + 1), 2);

	try {
		// MILP环境
		GRBEnv env = GRBEnv(true);
		env.set("LogFile", "nRounds.log");
		env.set(GRB_DoubleParam_PoolGap, GRB_INFINITY);
		env.start();

		// 创建空模型
		GRBModel model = GRBModel(env);

		/* 初始变量 */
		// 密钥扩展
		// 对于r轮加密，共需要(int)(r / 2) + r % 2个K
		vector<GRBVar> K(((int)(r / 2) + r % 2) * 16);
		for (int i = 0; i < ((int)(r / 2) + r % 2) * 16; i++) {
			K[i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
		}
		// K_i+4 = ~K_i + i
		for (int j = 4 * 16; j < ((int)(r / 2) + r % 2) * 16; j += 16) {
			bitset<16> i = (int)(j / 16) - 4;
			for (int k = 0; k < 16; k++) {
				model.addConstr(K[j + k] == (1 - K[j - 4 * 16 + k]) + (int)i[15 - k]);
			}
		}

		vector<GRBVar> X(144 * (r + 1));
		// L0, R0共32个变量，明文约束
		bitset<16> L0_bit = L0, R0_bit = R0;
		for (int i = 0; i < 16; i++) { // 为保持结构一致，方便编码，初始变量中前7*16个位置并没有意义，只是占位
			X[7 * 16 + i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			model.addConstr(X[7 * 16 + i] == (int)L0_bit[15 - i]);
			X[8 * 16 + i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			model.addConstr(X[8 * 16 + i] == (int)R0_bit[15 - i]);
		}


		/* 计算di和ei时涉及3个操作数，分别需要r*16个中间变量 */
		vector<GRBVar> D(16 * r);
		vector<GRBVar> E(16 * r);
		for (int i = 0; i < 16 * r; i++) {
			D[i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			E[i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
		}


		/* 每轮变量 */
		bitset<16> i_1;
		for (int i = 1; i <= r; i++) {
			i_1 = i - 1;
			// 本轮次中MILP变量的起始编码为i*144
			// ai, bi, ci, di, ei, fi, RK_i-1, Li, Ri共144个变量
			for (int j = 0; j < 144; j++) {
				X[i * 144 + j] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			}

			for (int j = 0; j < 16; j++) {
				// RK_2i = K_i, RK_2i+1 = ~K_i
				if (i % 2 == 1) { // 奇数轮，RK_2i = K_i
					model.addConstr(X[i * 144 + 96 + j] == K[((int)((i - 1) / 2)) * 16 + j]);
				}
				else // 偶数轮，RK_2i+1 = ~K_i
				{
					model.addConstr(X[i * 144 + 96 + j] == 1 - K[((int)(i / 2) - 1) * 16 + j]);
				}
				// ai = L_i-1 & RK_i-1
				model.addQConstr(X[i * 144 + 0 * 16 + j] == X[7 * 16 + (i - 1) * 144 + j] * X[i * 144 + 96 + j]);
				// bi = R_i-1 + (i - 1)
				model.addConstr(X[i * 144 + 1 * 16 + j] >= X[8 * 16 + (i - 1) * 144 + j] - i_1[15 - j]);
				model.addConstr(X[i * 144 + 1 * 16 + j] >= i_1[15 - j] - X[8 * 16 + (i - 1) * 144 + j]);
				model.addConstr(X[i * 144 + 1 * 16 + j] <= X[8 * 16 + (i - 1) * 144 + j] + i_1[15 - j]);
				model.addConstr(X[i * 144 + 1 * 16 + j] <= 2 - X[8 * 16 + (i - 1) * 144 + j] - (int)i_1[15 - j]);
				// ci = ai + bi
				model.addConstr(X[i * 144 + 2 * 16 + j] >= X[i * 144 + 1 * 16 + j] - X[i * 144 + 0 * 16 + j]);
				model.addConstr(X[i * 144 + 2 * 16 + j] >= X[i * 144 + 0 * 16 + j] - X[i * 144 + 1 * 16 + j]);
				model.addConstr(X[i * 144 + 2 * 16 + j] <= X[i * 144 + 1 * 16 + j] + X[i * 144 + 0 * 16 + j]);
				model.addConstr(X[i * 144 + 2 * 16 + j] <= 2 - X[i * 144 + 1 * 16 + j] - X[i * 144 + 0 * 16 + j]);
				// di = ((ci << 2) & ((ci << 1) + ci
				if (j > 13) {
					model.addConstr(X[i * 144 + 3 * 16 + j] == X[i * 144 + 2 * 16 + j]);
				}
				else {
					model.addQConstr(D[(i - 1) * 16 + j] == X[i * 144 + 2 * 16 + j + 1] * X[i * 144 + 2 * 16 + j + 2]);
					model.addConstr(X[i * 144 + 3 * 16 + j] >= D[(i - 1) * 16 + j] - X[i * 144 + 2 * 16 + j]);
					model.addConstr(X[i * 144 + 3 * 16 + j] >= X[i * 144 + 2 * 16 + j] - D[(i - 1) * 16 + j]);
					model.addConstr(X[i * 144 + 3 * 16 + j] <= D[(i - 1) * 16 + j] + X[i * 144 + 2 * 16 + j]);
					model.addConstr(X[i * 144 + 3 * 16 + j] <= 2 - D[(i - 1) * 16 + j] - X[i * 144 + 2 * 16 + j]);
				}
				// ei = (di <<< 3) + (di <<< 9) + (di <<< 14)
				model.addConstr(E[(i - 1) * 16 + j] >= X[i * 144 + 3 * 16 + ((3 + j) % 16)] - X[i * 144 + 3 * 16 + ((9 + j) % 16)]);
				model.addConstr(E[(i - 1) * 16 + j] >= X[i * 144 + 3 * 16 + ((9 + j) % 16)] - X[i * 144 + 3 * 16 + ((3 + j) % 16)]);
				model.addConstr(E[(i - 1) * 16 + j] <= X[i * 144 + 3 * 16 + ((3 + j) % 16)] + X[i * 144 + 3 * 16 + ((9 + j) % 16)]);
				model.addConstr(E[(i - 1) * 16 + j] <= 2 - X[i * 144 + 3 * 16 + ((3 + j) % 16)] - X[i * 144 + 3 * 16 + ((9 + j) % 16)]);
				model.addConstr(X[i * 144 + 4 * 16 + j] >= E[(i - 1) * 16 + j] - X[i * 144 + 3 * 16 + ((14 + j) % 16)]);
				model.addConstr(X[i * 144 + 4 * 16 + j] >= X[i * 144 + 3 * 16 + ((14 + j) % 16)] - E[(i - 1) * 16 + j]);
				model.addConstr(X[i * 144 + 4 * 16 + j] <= E[(i - 1) * 16 + j] + X[i * 144 + 3 * 16 + ((14 + j) % 16)]);
				model.addConstr(X[i * 144 + 4 * 16 + j] <= 2 - E[(i - 1) * 16 + j] - X[i * 144 + 3 * 16 + ((14 + j) % 16)]);
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

		/* 密文约束 */
		// Lr|Rr = c
		bitset<16> Lr_bit = Lr, Rr_bit = Rr;
		for (int i = 15; i >= 0; i--) {
			model.addConstr(X[144 * r + 7 * 16 + i] == (int)Lr_bit[15 - i]);
			model.addConstr(X[144 * r + 8 * 16 + i] == (int)Rr_bit[15 - i]);
		}

		// 求解
		model.set(GRB_DoubleParam_MIPGap, GRB_INFINITY);
		model.set(GRB_IntParam_PoolSearchMode, 2);
		model.set(GRB_IntParam_PoolSolutions, 2000000000);
		model.set(GRB_IntParam_RINS, 0);
		model.set(GRB_IntParam_MIPFocus, 3);
		model.set(GRB_IntParam_VarBranch, 2);
		// model.write("nRounds.lp");
		auto start = high_resolution_clock::now();
		model.optimize();
		auto stop = high_resolution_clock::now();

		int solCount = model.get(GRB_IntAttr_SolCount);
		if (solCount > 0) {
			ofstream outfile;
			outfile.open(r + "RoundResult.csv", ios::out | ios::app);
			outfile << "K0, K1, K2, K3" << endl;
			bitset<16> K0, K1, K2, K3;
			char K0_c[6], K1_c[6], K2_c[6], K3_c[4];
			for (int i = 0; i < solCount; i++) {
				model.set(GRB_IntParam_SolutionNumber, i);
				for (int j = 0; j < 16; j++) {
					K0[15 - j] = 0;
					K1[15 - j] = 0;
					K2[15 - j] = 0;
					K3[15 - j] = 0;
					if (K[j].get(GRB_DoubleAttr_Xn) > 0.5) {
						K0[15 - j] = 1;
					}
					if (r > 2) {
						if (K[16 + j].get(GRB_DoubleAttr_Xn) > 0.5) {
							K1[15 - j] = 1;
						}
						if (r > 4) {
							if (K[16 * 2 + j].get(GRB_DoubleAttr_Xn) > 0.5) {
								K2[15 - j] = 1;
							}
							if (r > 6) {
								if (K[16 * 3 + j].get(GRB_DoubleAttr_Xn) > 0.5) {
									K3[15 - j] = 1;
								}
							}
						}
					}
				}
				sprintf(K0_c, "%04X, ", K0);
				sprintf(K1_c, "%04X, ", K1);
				sprintf(K2_c, "%04X, ", K2);
				sprintf(K3_c, "%04X", K3);
				outfile << K0_c << ", " << K1_c << ", " << K2_c << ", " << K3_c << endl;
			}
			outfile.close();
		}

		auto duration = duration_cast<microseconds>(stop - start);
		cout << "Time: " << duration.count() / 1000000 << "s" << endl;
	}
	catch (GRBException e) {
		cout << "Gurobi错误码：" << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "未知错误" << endl;
	}
	

	return;
}