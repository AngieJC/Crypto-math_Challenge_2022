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

void And(GRBModel * m, GRBVar* a, GRBVar* b, GRBVar* c);
GRBVar And(GRBModel * m, GRBVar* a, GRBVar* b);
void Xor(GRBModel* m, GRBVar* a, GRBVar* b, GRBVar* c);
GRBVar Xor(GRBModel* m, GRBVar* a, GRBVar* b);
GRBVar GetNOT(GRBModel* m, GRBVar* a);

void cubeAttack(int r) {
	uint16_t L0 = 0x1111, R0 = 0x1111, Lr = 0, Rr = 0;
	uint32_t c = 0;
	cout << "加密11111111" << endl;
	cout << "输入对应密文：" << endl;
	cin.setf(ios_base::hex, ios_base::basefield);
	cin >> c;
	// 将密文拆解成Lr和Rr
	memcpy(&Rr, ((uint16_t*)&c), 2);
	memcpy(&Lr, ((uint16_t*)&c + 1), 2);

	try {
		// MILP环境
		GRBEnv env = GRBEnv(true);
		char logfile_str[32];
		sprintf(logfile_str, "%dRounds.log", r);
		env.set("LogFile", logfile_str);
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
		vector<GRBVar> D(14 * r);
		vector<GRBVar> E(16 * r);
		for (int i = 0; i < 14 * r; i++) {
			D[i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
		}
		for (int i = 0; i < 16 * r; i++) {
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
					model.addQConstr(D[(i - 1) * 14 + j] == X[i * 144 + 2 * 16 + j + 1] * X[i * 144 + 2 * 16 + j + 2]);
					model.addConstr(X[i * 144 + 3 * 16 + j] >= D[(i - 1) * 14 + j] - X[i * 144 + 2 * 16 + j]);
					model.addConstr(X[i * 144 + 3 * 16 + j] >= X[i * 144 + 2 * 16 + j] - D[(i - 1) * 14 + j]);
					model.addConstr(X[i * 144 + 3 * 16 + j] <= D[(i - 1) * 14 + j] + X[i * 144 + 2 * 16 + j]);
					model.addConstr(X[i * 144 + 3 * 16 + j] <= 2 - D[(i - 1) * 14 + j] - X[i * 144 + 2 * 16 + j]);
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

		// 超级多项式约束
		if (r == 3) {
			// 命名规则：p_I[立方]_[左半/右半]_[索引]_[轮数]
			int p_I3_R_0_3, p_I9_R_0_3, p_I10_R_1_3, p_I19_L_0_3, p_I25_L_0_3, p_I26_R_1_3;

			// 获取各个超级多项式的取值
			cout << "分别对以下明文进行加密：" << endl;
			cout << "00000000" << endl << "10000000" << endl << "00400000" << endl << "00200000" << endl << "00001000" << endl << "00000040" << endl << "00000020" << endl;
			cout << "输入对应密文：" << endl;
			uint32_t c0, c1, c2, c3, c4, c5, c6;
			cin >> c0 >> c1 >> c2 >> c3 >> c4 >> c5 >> c6;
			p_I3_R_0_3 = ((c0 & 0x00008000) ^ (c1 & 0x00008000)) ? 1 : 0;
			p_I9_R_0_3 = ((c0 & 0x00008000) ^ (c2 & 0x00008000)) ? 1 : 0;
			p_I10_R_1_3 = ((c0 & 0x00004000) ^ (c3 & 0x00004000)) ? 1 : 0;
			p_I19_L_0_3 = ((c0 & 0x80000000) ^ (c4 & 0x80000000)) ? 1 : 0;
			p_I25_L_0_3 = ((c0 & 0x80000000) ^ (c5 & 0x80000000)) ? 1 : 0;
			p_I26_R_1_3 = ((c0 & 0x40000000) ^ (c6 & 0x40000000)) ? 1 : 0;

			// p(x,v)0
			GRBVar notZero = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			model.addConstr(K[0] == 1 - notZero);
			GRBVar k3Xork0 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			Xor(&model, &K[3], &notZero, &k3Xork0);
			GRBVar k3Xork0Xork19 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			Xor(&model, &k3Xork0, &K[19], &k3Xork0Xork19);
			model.addConstr(k3Xork0Xork19 == p_I3_R_0_3);
			// p(x,v)1
			GRBVar notThree = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			model.addConstr(K[3] == 1 - notThree);
			GRBVar k3Andk9 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar k3Andk9Andk19 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			And(&model, &notThree, &K[9], &k3Andk9);
			And(&model, &k3Andk9, &K[19], &k3Andk9Andk19);
			GRBVar k25Xork0 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			Xor(&model, &K[25], &notZero, &k25Xork0);
			GRBVar k25Xork0Xork3Andk9Andk19 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			Xor(&model, &k25Xork0, &k3Andk9Andk19, &k25Xork0Xork3Andk9Andk19);
			model.addConstr(k25Xork0Xork3Andk9Andk19 == p_I9_R_0_3);
			// p(x,v)2
			GRBVar notOne = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar notFour = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			model.addConstr(K[1] == 1 - notOne);
			model.addConstr(K[4] == 1 - notFour);
			GRBVar k4Andk10 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar k4Andk10Andk20 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			And(&model, &notFour, &K[10], &k4Andk10);
			And(&model, &k4Andk10, &K[20], &k4Andk10Andk20);
			GRBVar k26Xork1 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			Xor(&model, &K[26], &notOne, &k26Xork1);
			GRBVar k26Xork1Xork4Andk10Andk20 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			Xor(&model, &k26Xork1, &k4Andk10Andk20, &k26Xork1Xork4Andk10Andk20);
			model.addConstr(k26Xork1Xork4Andk10Andk20 == p_I10_R_1_3);
			// p(x,v)3
			GRBVar k0Xork3 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar k0Xork3Xork16 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			Xor(&model, &K[0], &notThree, &k0Xork3);
			Xor(&model, &k0Xork3, &K[16], &k0Xork3Xork16);
			model.addConstr(k0Xork3Xork16 == p_I19_L_0_3);
			// p(x,v)4
			GRBVar notNine = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar k0Xork9 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar k3Andk16 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar k3Andk16Andk19 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar k0Xork9Xork3Andk16Andk19 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			And(&model, &notThree, &K[16], &k3Andk16);
			And(&model, &k3Andk16, &K[19], &k3Andk16Andk19);
			Xor(&model, &K[0], &notNine, &k0Xork9);
			Xor(&model, &k0Xork9, &k3Andk16Andk19, &k0Xork9Xork3Andk16Andk19);
			model.addConstr(k0Xork9Xork3Andk16Andk19 == p_I25_L_0_3);
			// p(x,v)5
			GRBVar notTen = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar k1Xork10 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar k4Andk17 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar k4Andk17Andk20 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			GRBVar k1Xork10Xork4Andk17Andk20 = model.addVar(0.0, 1.0, 1.0, GRB_BINARY);
			model.addConstr(K[10] == 1 - notTen);
			Xor(&model, &K[1], &notTen, &k1Xork10);
			And(&model, &notFour, &K[17], &k4Andk17);
			And(&model, &k4Andk17, &K[20], &k4Andk17Andk20);
			Xor(&model, &k1Xork10, &k4Andk17Andk20, &k1Xork10Xork4Andk17Andk20);
			model.addConstr(k1Xork10Xork4Andk17Andk20 == p_I26_R_1_3);
		}
		else if (r == 4) {
			int p_I2_R_0_4, p_I18_L_0_4;
			cout << "分别对以下明文进行加密：" << endl;
			cout << "00000000" << endl << "20000000" << endl << "00002000" << endl;
			cout << "输入对应密文：" << endl;
			uint32_t c0, c1, c2;
			cin >> c0 >> c1 >> c2;
			p_I2_R_0_4 = ((c0 & 0x00008000) ^ (c1 & 0x00008000)) ? 1 : 0;
			p_I18_L_0_4 = ((c0 & 0x80000000) ^ (c2 & 0x80000000)) ? 1 : 0;

			// p(x,v) = k_2 k_9 k_16 + k_18 + !k_9 k_16 k_25 + !k_9 + k_2 k_16 k_25 + k_2 k_9 !k_25 + !k_25 + k_16 + k_2 !k_12 k_28 + k_2 !k_6 k_22
			GRBVar not6 = GetNOT(&model, &K[6]);
			GRBVar not9 = GetNOT(&model, &K[9]);
			GRBVar not12 = GetNOT(&model, &K[12]);
			GRBVar not25 = GetNOT(&model, &K[25]);
			GRBVar k2Andk9 = And(&model, &K[2], &K[9]);
			GRBVar k2Andk9Andk16 = And(&model, &k2Andk9, &K[16]);
			GRBVar k2Andk9Andk16Xork18 = Xor(&model, &k2Andk9Andk16, &K[18]);
			GRBVar k9Andk16 = And(&model, &not9, &K[16]);
			GRBVar k9Andk16Andk25 = And(&model, &k9Andk16, &K[25]);
			GRBVar k2Andk9Andk16Xork18Xork9Andk16Andk25 = Xor(&model, &k2Andk9Andk16Xork18, &k9Andk16Andk25);
			GRBVar k2Andk9Andk16Xork18Xork9Andk16Andk25Xork9 = Xor(&model, &k2Andk9Andk16Xork18Xork9Andk16Andk25, &not9);
			GRBVar k2Andk16 = And(&model, &K[2], &K[16]);
			GRBVar e = And(&model, &k2Andk16, &K[25]);
			// GRBVar* k2Andk9 = And(&model, &K[2], &K[9]);
			GRBVar f = And(&model, &k2Andk9, &not25);
			GRBVar k2Andk12 = And(&model, &not12, &K[2]);
			GRBVar i = And(&model, &k2Andk12, &K[28]);
			GRBVar k2Andk6 = And(&model, &not6, &K[2]);
			GRBVar j = And(&model, &k2Andk6, &K[22]);
			GRBVar dXore = Xor(&model, &k2Andk9Andk16Xork18Xork9Andk16Andk25Xork9, &e);
			GRBVar eXorf = Xor(&model, &dXore, &f);
			GRBVar fXorg = Xor(&model, &f, &not25);
			GRBVar gXorh = Xor(&model, &fXorg, &K[16]);
			GRBVar hXori = Xor(&model, &gXorh, &i);
			GRBVar iXorj = Xor(&model, &hXori, &j);
			model.addConstr(iXorj == p_I2_R_0_4);

			// p(x,v) = k_9 + !k_2 !k_9 k_25 + !k_2 !k_9 !k_16 + k_25+k_9 !k_16 !k_25 + !k_2 !k_16 !k_25 + !k_2 + !k_0 + !k_12 !k_16 k_28 + !k_6 !k_16 k_22
			GRBVar not0 = GetNOT(&model, &K[0]);
			GRBVar not2 = GetNOT(&model, &K[2]);
			GRBVar not16 = GetNOT(&model, &K[16]);
			GRBVar not2Andnot9 = And(&model, &not2, &not9);
			GRBVar k = And(&model, &not2Andnot9, &K[25]);
			GRBVar l = And(&model, &not2Andnot9, &not16);
			GRBVar k9Andnot16 = And(&model, &K[9], &not16);
			GRBVar m = And(&model, &k9Andnot16, &not25);
			GRBVar not2Andnot16 = And(&model, &not2, &not16);
			GRBVar n = And(&model, &not2Andnot16, &not25);
			GRBVar not12Andnot16 = And(&model, &not12, &not16);
			GRBVar o = And(&model, &not12Andnot16, &K[28]);
			GRBVar not6Andnot16 = And(&model, &not6, &not16);
			GRBVar p = And(&model, &not6Andnot16, &K[22]);
			GRBVar k9Xork = Xor(&model, &K[9], &k);
			GRBVar kXorl = Xor(&model, &k9Xork, &l);
			GRBVar lXork25 = Xor(&model, &kXorl, &K[25]);
			GRBVar k25Xorm = Xor(&model, &m, &lXork25);
			GRBVar nXorm = Xor(&model, &k25Xorm, &n);
			GRBVar nXornot2 = Xor(&model, &not2, &nXorm);
			GRBVar not0Xornot2 = Xor(&model, &nXornot2, &not0);
			GRBVar not0Xoro = Xor(&model, &o, &not0Xornot2);
			GRBVar pXoro = Xor(&model, &not0Xoro, &p);
			model.addConstr(pXoro == p_I18_L_0_4);
		}

		// 求解
		model.set(GRB_DoubleParam_MIPGap, GRB_INFINITY);
		model.set(GRB_IntParam_PoolSearchMode, 2);
		model.set(GRB_IntParam_PoolSolutions, 2000000000);
		model.set(GRB_IntParam_RINS, 0);
		model.set(GRB_IntParam_MIPFocus, 3);
		model.set(GRB_IntParam_VarBranch, 2);
		model.write("nRounds.lp");
		auto start = high_resolution_clock::now();
		model.optimize();
		auto stop = high_resolution_clock::now();

		int solCount = model.get(GRB_IntAttr_SolCount);
		if (solCount > 0) {
			ofstream outfile;
			char outfile_str[32];
			sprintf(outfile_str, "%dRoundResult.csv", r);
			remove(outfile_str);
			outfile.open(outfile_str, ios::out | ios::app);
			outfile << "K0, K1, K2, K3" << endl;
			bitset<16> K0, K1, K2, K3;
			char K0_c[8], K1_c[8], K2_c[8], K3_c[8];
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
				sprintf(K0_c, "%04X", K0);
				sprintf(K1_c, "%04X", K1);
				sprintf(K2_c, "%04X", K2);
				sprintf(K3_c, "%04X", K3);
				outfile << K0_c << ", " << K1_c << ", " << K2_c << ", " << K3_c << endl;
				memset(K0_c, 0, sizeof(char) * 8);
				memset(K1_c, 0, sizeof(char) * 8);
				memset(K2_c, 0, sizeof(char) * 8);
				memset(K3_c, 0, sizeof(char) * 8);
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

void And(GRBModel* m, GRBVar* a, GRBVar* b, GRBVar* c) {
	m->addQConstr(*c == (*a) * (*b));
}

inline GRBVar And(GRBModel* m, GRBVar* a, GRBVar* b)
{
	GRBVar c = m->addVar(0.0, 1.0, 1.0, GRB_BINARY);
	m->addQConstr(c == (*a) * (*b));
	return c;
}

void Xor(GRBModel* m, GRBVar* a, GRBVar* b, GRBVar* c) {
	m->addConstr(*c >= *a - *b);
	m->addConstr(*c >= *b - *a);
	m->addConstr(*c <= *b + *a);
	m->addConstr(*c <= 2 - *b - *a);
}

inline GRBVar Xor(GRBModel* m, GRBVar* a, GRBVar* b)
{
	GRBVar c = m->addVar(0.0, 1.0, 1.0, GRB_BINARY);
	m->addConstr(c >= *a - *b);
	m->addConstr(c >= *b - *a);
	m->addConstr(c <= *b + *a);
	m->addConstr(c <= 2 - *b - *a);
	return c;
}

inline GRBVar GetNOT(GRBModel* m, GRBVar* a)
{
	GRBVar NOTa = m->addVar(0.0, 1.0, 1.0, GRB_BINARY);
	m->addConstr(NOTa == 1 - *a);
	return NOTa;
}
