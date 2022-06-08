#pragma once
#include <iostream>
#include <cstring>
#include <stdio.h>
#include "CryptographicAlgorithm.h"

using namespace std;

void analysis3_4Rounds(int r) {
	uint16_t c[2], p[2] = { 0xffff, 0xffff }, c_[2];
	uint32_t cinc;
	u16 seedkey[4];
	int flag1 = 0, flag2;
	switch (r)
	{
	case 3:
		// 命名规则：p_I[立方]_[左半/右半]_[索引]_[轮数]
		int p_I3_R_0_3, p_I9_R_0_3, p_I10_R_1_3, p_I19_L_0_3, p_I25_L_0_3, p_I26_R_1_3;

		// 获取各个超级多项式的取值
		cout << "分别对以下明文进行加密：" << endl;
		cout << "00000000" << endl << "10000000" << endl << "00800000" << endl << "00400000" << endl << "00001000" << endl << "00000080" << endl << "00000040" << endl;
		cout << "输入对应密文：" << endl;
		uint32_t c0, c1, c2, c3, c4, c5, c6;
		cin.setf(ios_base::hex, ios_base::basefield);
		cin >> c0 >> c1 >> c2 >> c3 >> c4 >> c5 >> c6;
		p_I3_R_0_3  = ((c0 & 0x00008000) ^ (c1 & 0x00008000)) ? 1 : 0;
		p_I9_R_0_3  = ((c0 & 0x00008000) ^ (c1 & 0x00008000)) ? 1 : 0;
		p_I10_R_1_3 = ((c0 & 0x00004000) ^ (c1 & 0x00004000)) ? 1 : 0;
		p_I19_L_0_3 = ((c0 & 0x80000000) ^ (c1 & 0x80000000)) ? 1 : 0;
		p_I25_L_0_3 = ((c0 & 0x80000000) ^ (c1 & 0x80000000)) ? 1 : 0;
		p_I26_R_1_3 = ((c0 & 0x40000000) ^ (c1 & 0x40000000)) ? 1 : 0;

		cout << "加密ffffffff" << endl;
		cout << "输入对应密文：" << endl;
		cin >> cinc;
		memcpy(&c_[0], (u16*)(&cinc) + 1, 2);
		memcpy(&c_[1], (u16*)(&cinc), 2);
		for (seedkey[0] = 0; flag1 <= 0xffff; seedkey[0]++, flag1++) {
			flag2 = 0;
			for (seedkey[1] = 0; flag2 <= 0xffff; seedkey[1]++, flag2++) {
				if ((((seedkey[0] ^ 0x1000) >> 12) ^ (1 - ((seedkey[0] ^ 0x8000) >> 15)) ^ ((seedkey[1] ^ 0x1000) >> 12)) == p_I3_R_0_3) {
					if ((((seedkey[1] ^ 0x0040) >> 6) ^ (1 - ((seedkey[0] ^ 0x8000) >> 15)) ^ ((1 - ((seedkey[0] ^ 0x1000) >> 12)) & ((seedkey[0] ^ 0x0040) >> 6) & ((seedkey[1] ^ 0x1000) >> 12))) == p_I9_R_0_3) {
						if ((((seedkey[1] ^ 0x0020) >> 5) ^ (1 - ((seedkey[0] ^ 0x4000) >> 14)) ^ ((1 - ((seedkey[0] ^ 0x0800) >> 11)) & ((seedkey[0] ^ 0x0020) >> 5) & ((seedkey[1] ^ 0x0800) >> 11))) == p_I10_R_1_3) {
							if ((((seedkey[0] ^ 0x8000) >> 15) ^ (1 - ((seedkey[0] ^ 0x1000) >> 13)) ^ ((seedkey[1] ^ 0x8000) >> 15)) == p_I19_L_0_3) {
								if ((((seedkey[0] ^ 0x8000) >> 15) ^ (1 - ((seedkey[0] ^ 0x0040) >> 6)) ^ ((1 - ((seedkey[0] ^ 0x1000) >> 13)) & ((seedkey[1] ^ 0x8000) >> 15))& ((seedkey[1] ^ 0x1000) >> 12)) == p_I25_L_0_3) {
									if ((((seedkey[0] ^ 0x4000) >> 14) ^ (1 - ((seedkey[0] ^ 0x0020) >> 5)) ^ ((1 - ((seedkey[0] ^ 0x0800) >> 11)) & ((seedkey[1] ^ 0x4000) >> 14)) ^ ((seedkey[1] ^ 0x0800) >> 11)) == p_I26_R_1_3) {
										Enc(p, c, seedkey, 3);
										if (c_[0] == c[0] && c_[1] == c[1]) {
											cout << seedkey[0] << " " << seedkey[1] << endl;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		break;
	case 4:
		break;
	default:
		break;
	}
}