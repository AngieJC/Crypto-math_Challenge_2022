#pragma once
#include <iostream>
#include <stdio.h>
#include <cmath>

using namespace std;

void analysis2Rounds() {
	// a = 0, b = 8000
	cout << "选择明文\tL0=0\tR0=1" << endl << "\t\tL0*=0\tR0*=HEX(8001)" << endl << "输入密文Enc(L0|R0)(十六进制)：";
	uint32_t c = 0, c_ = 0;
	cin.setf(ios_base::hex, ios_base::basefield);
	cin >> c;
	cout << "输入密文Enc(L0*|R0*)(十六进制)：";
	cin >> c_;

	// 将c拆成两部分
	uint16_t L2 = 0, L2_ = 0;
	memcpy(&L2, ((uint8_t*)&c) + 2, 2);
	memcpy(&L2_, ((uint8_t*)&c_) + 2, 2);

	L2_ ^= L2; // 获得密文差分
	L2_ &= 0x0820; // 与掩码相乘，只保留有用信息
	int RK_6_1 = -1, RK_12_1 = -1;
	switch (L2_)
	{
	case 0x0820: // 均为0
		RK_6_1 = 0;
		RK_12_1 = 0;
		break;
	case 0x0800: // RK_12_1=0, RK_6_1=1
		RK_6_1 = 1;
		RK_12_1 = 0;
		break;
	case 0x0020: // RK_12_1=1, RK_6_1=0
		RK_6_1 = 0;
		RK_12_1 = 1;
		break;
	case 0x0000: // RK_12_1=1, RK_6_1=1
		RK_6_1 = 1;
		RK_12_1 = 1;
		break;
	default:
		break;
	}
	printf("密钥K0：****,%d***,**%d*,****\n", RK_12_1, RK_6_1);
	printf("----------------------------------------------\n");

	// a = 0, b = 4000
	cout << "选择明文\tL0*=0\tR0*=HEX(4001)" << endl << "输入密文Enc(L0*|R0*)(十六进制)：";
	c_ = 0;
	cin >> c_;

	// 将c拆成两部分
	L2_ = 0;
	memcpy(&L2_, ((uint8_t*)&c_) + 2, 2);

	L2_ ^= L2; // 获得密文差分
	L2_ &= 0x0410; // 与掩码相乘，只保留有用信息
	int RK_5_1 = -1, RK_11_1 = -1;
	switch (L2_)
	{
	case 0x0410: // 均为0
		RK_5_1 = 0;
		RK_11_1 = 0;
		break;
	case 0x0400: // RK_11_1=0, RK_5_1=1
		RK_5_1 = 1;
		RK_11_1 = 0;
		break;
	case 0x0010: // RK_11_1=1, RK_5_1=0
		RK_5_1 = 0;
		RK_11_1 = 1;
		break;
	case 0x0000: // RK_11_1=1, RK_5_1=1
		RK_5_1 = 1;
		RK_11_1 = 1;
		break;
	default:
		break;
	}
	printf("密钥K0：****,%d%d**,**%d%d,****\n", RK_12_1, RK_11_1, RK_6_1, RK_5_1);
	printf("----------------------------------------------\n");

	// a = 0, b = 2000
	cout << "选择明文\tL0*=0\tR0*=HEX(2001)" << endl << "输入密文Enc(L0*|R0*)(十六进制)：";
	c_ = 0;
	cin >> c_;

	// 将c拆成两部分
	L2_ = 0;
	memcpy(&L2_, ((uint8_t*)&c_) + 2, 2);

	L2_ ^= L2; // 获得密文差分
	L2_ &= 0x0208; // 与掩码相乘，只保留有用信息
	int RK_4_1 = -1, RK_10_1 = -1;
	switch (L2_)
	{
	case 0x0208: // 均为0
		RK_4_1 = 0;
		RK_10_1 = 0;
		break;
	case 0x0200: // RK_10_1=0, RK_4_1=1
		RK_4_1 = 1;
		RK_10_1 = 0;
		break;
	case 0x0008: // RK_10_1=1, RK_4_1=0
		RK_4_1 = 0;
		RK_10_1 = 1;
		break;
	case 0x0000: // RK_10_1=1, RK_4_1=1
		RK_4_1 = 1;
		RK_10_1 = 1;
		break;
	default:
		break;
	}
	printf("密钥K0：****,%d%d%d*,**%d%d,%d***\n", RK_12_1, RK_11_1, RK_10_1, RK_6_1, RK_5_1, RK_4_1);
	printf("----------------------------------------------\n");

	// a = 0, b = 1000
	cout << "选择明文\tL0*=0\tR0*=HEX(1001)" << endl << "输入密文Enc(L0*|R0*)(十六进制)：";
	c_ = 0;
	cin >> c_;

	// 将c拆成两部分
	L2_ = 0;
	memcpy(&L2_, ((uint8_t*)&c_) + 2, 2);

	L2_ ^= L2; // 获得密文差分
	L2_ &= 0x0104; // 与掩码相乘，只保留有用信息
	int RK_3_1 = -1, RK_9_1 = -1;
	switch (L2_)
	{
	case 0x0104: // 均为0
		RK_3_1 = 0;
		RK_9_1 = 0;
		break;
	case 0x0100: // RK_9_1=0, RK_3_1=1
		RK_3_1 = 1;
		RK_9_1 = 0;
		break;
	case 0x0004: // RK_9_1=1, RK_3_1=0
		RK_3_1 = 0;
		RK_9_1 = 1;
		break;
	case 0x0000: // RK_9_1=1, RK_3_1=1
		RK_3_1 = 1;
		RK_9_1 = 1;
		break;
	default:
		break;
	}
	printf("密钥K0：****,%d%d%d%d,**%d%d,%d%d**\n", RK_12_1, RK_11_1, RK_10_1, RK_9_1, RK_6_1, RK_5_1, RK_4_1, RK_3_1);
	printf("----------------------------------------------\n");

	// a = 0, b = 0800
	cout << "选择明文\tL0*=0\tR0*=HEX(0801)" << endl << "输入密文Enc(L0*|R0*)(十六进制)：";
	c_ = 0;
	cin >> c_;

	// 将c拆成两部分
	L2_ = 0;
	memcpy(&L2_, ((uint8_t*)&c_) + 2, 2);

	L2_ ^= L2; // 获得密文差分
	L2_ &= 0x0082; // 与掩码相乘，只保留有用信息
	int RK_2_1 = -1, RK_8_1 = -1;
	switch (L2_)
	{
	case 0x0082: // 均为0
		RK_2_1 = 0;
		RK_8_1 = 0;
		break;
	case 0x0080: // RK_8_1=0, RK_2_1=1
		RK_2_1 = 1;
		RK_8_1 = 0;
		break;
	case 0x0002: // RK_8_1=1, RK_2_1=0
		RK_2_1 = 0;
		RK_8_1 = 1;
		break;
	case 0x0000: // RK_8_1=1, RK_2_1=1
		RK_2_1 = 1;
		RK_8_1 = 1;
		break;
	default:
		break;
	}
	printf("密钥K0：****,%d%d%d%d,%d*%d%d,%d%d%d*\n", RK_12_1, RK_11_1, RK_10_1, RK_9_1, RK_8_1, RK_6_1, RK_5_1, RK_4_1, RK_3_1, RK_2_1);
	printf("----------------------------------------------\n");

	// a = 0, b = 0400
	cout << "选择明文\tL0*=0\tR0*=HEX(0401)" << endl << "输入密文Enc(L0*|R0*)(十六进制)：";
	c_ = 0;
	cin >> c_;

	// 将c拆成两部分
	L2_ = 0;
	memcpy(&L2_, ((uint8_t*)&c_) + 2, 2);

	L2_ ^= L2; // 获得密文差分
	L2_ &= 0x0041; // 与掩码相乘，只保留有用信息
	int RK_1_1 = -1, RK_7_1 = -1;
	switch (L2_)
	{
	case 0x0041: // 均为0
		RK_1_1 = 0;
		RK_7_1 = 0;
		break;
	case 0x0040: // RK_7_1=0, RK_1_1=1
		RK_1_1 = 1;
		RK_7_1 = 0;
		break;
	case 0x0001: // RK_7_1=1, RK_1_1=0
		RK_1_1 = 0;
		RK_7_1 = 1;
		break;
	case 0x0000: // RK_7_1=1, RK_1_1=1
		RK_1_1 = 1;
		RK_7_1 = 1;
		break;
	default:
		break;
	}
	printf("密钥K0：****,%d%d%d%d,%d%d%d%d,%d%d%d%d\n", RK_12_1, RK_11_1, RK_10_1, RK_9_1, RK_8_1, RK_7_1, RK_6_1, RK_5_1, RK_4_1, RK_3_1, RK_2_1, RK_1_1);
	printf("----------------------------------------------\n");

	// a = 0, b = 0200
	cout << "选择明文\tL0*=0\tR0*=HEX(0201)" << endl << "输入密文Enc(L0*|R0*)(十六进制)：";
	c_ = 0;
	cin >> c_;

	// 将c拆成两部分
	L2_ = 0;
	memcpy(&L2_, ((uint8_t*)&c_) + 2, 2);

	L2_ ^= L2; // 获得密文差分
	L2_ &= 0x8000; // 与掩码相乘，只保留有用信息
	int RK_16_1 = -1;
	switch (L2_)
	{
	case 0x8000: // 为0
		RK_16_1 = 0;
		break;
	case 0x0000: // RK_16_1=1
		RK_16_1 = 1;
		break;
	default:
		break;
	}
	printf("密钥K0：%d***,%d%d%d%d,%d%d%d%d,%d%d%d%d\n", RK_16_1, RK_12_1, RK_11_1, RK_10_1, RK_9_1, RK_8_1, RK_7_1, RK_6_1, RK_5_1, RK_4_1, RK_3_1, RK_2_1, RK_1_1);
	printf("----------------------------------------------\n");

	// a = 0, b = 0100
	cout << "选择明文\tL0*=0\tR0*=HEX(0101)" << endl << "输入密文Enc(L0*|R0*)(十六进制)：";
	c_ = 0;
	cin >> c_;

	// 将c拆成两部分
	L2_ = 0;
	memcpy(&L2_, ((uint8_t*)&c_) + 2, 2);

	L2_ ^= L2; // 获得密文差分
	L2_ &= 0x4000; // 与掩码相乘，只保留有用信息
	int RK_15_1 = -1;
	switch (L2_)
	{
	case 0x4000: // 为0
		RK_15_1 = 0;
		break;
	case 0x0000: // RK_15_1=1
		RK_15_1 = 1;
		break;
	default:
		break;
	}
	printf("密钥K0：%d%d**,%d%d%d%d,%d%d%d%d,%d%d%d%d\n", RK_16_1, RK_15_1, RK_12_1, RK_11_1, RK_10_1, RK_9_1, RK_8_1, RK_7_1, RK_6_1, RK_5_1, RK_4_1, RK_3_1, RK_2_1, RK_1_1);
	printf("----------------------------------------------\n");

	// a = 0, b = 0080
	cout << "选择明文\tL0*=0\tR0*=HEX(0081)" << endl << "输入密文Enc(L0*|R0*)(十六进制)：";
	c_ = 0;
	cin >> c_;

	// 将c拆成两部分
	L2_ = 0;
	memcpy(&L2_, ((uint8_t*)&c_) + 2, 2);

	L2_ ^= L2; // 获得密文差分
	L2_ &= 0x2000; // 与掩码相乘，只保留有用信息
	int RK_14_1 = -1;
	switch (L2_)
	{
	case 0x2000: // 为0
		RK_14_1 = 0;
		break;
	case 0x0000: // RK_14_1=1
		RK_14_1 = 1;
		break;
	default:
		break;
	}
	printf("密钥K0：%d%d%d*,%d%d%d%d,%d%d%d%d,%d%d%d%d\n", RK_16_1, RK_15_1, RK_14_1, RK_12_1, RK_11_1, RK_10_1, RK_9_1, RK_8_1, RK_7_1, RK_6_1, RK_5_1, RK_4_1, RK_3_1, RK_2_1, RK_1_1);
	printf("----------------------------------------------\n");

	// a = 0, b = 0040
	cout << "选择明文\tL0*=0\tR0*=HEX(0041)" << endl << "输入密文Enc(L0*|R0*)(十六进制)：";
	c_ = 0;
	cin >> c_;

	// 将c拆成两部分
	L2_ = 0;
	memcpy(&L2_, ((uint8_t*)&c_) + 2, 2);

	L2_ ^= L2; // 获得密文差分
	L2_ &= 0x1000; // 与掩码相乘，只保留有用信息
	int RK_13_1 = -1;
	switch (L2_)
	{
	case 0x1000: // 为0
		RK_13_1 = 0;
		break;
	case 0x0000: // RK_13_1=1
		RK_13_1 = 1;
		break;
	default:
		break;
	}
	printf("密钥K0：%d%d%d%d,%d%d%d%d,%d%d%d%d,%d%d%d%d\n", RK_16_1, RK_15_1, RK_14_1, RK_13_1, RK_12_1, RK_11_1, RK_10_1, RK_9_1, RK_8_1, RK_7_1, RK_6_1, RK_5_1, RK_4_1, RK_3_1, RK_2_1, RK_1_1);
	printf("----------------------------------------------\n");
	
	int key = (int)(pow(2, 0) * RK_1_1 + pow(2, 1) * RK_2_1 + pow(2, 2) * RK_3_1 + pow(2, 3) * RK_4_1 + pow(2, 4) * RK_5_1 + pow(2, 5) * RK_6_1 + pow(2, 6) * RK_7_1 + pow(2, 7) * RK_8_1 + pow(2, 8) * RK_9_1 + pow(2, 9) * RK_10_1 + pow(2, 10) * RK_11_1 + pow(2, 11) * RK_12_1 + pow(2, 12) * RK_13_1 + pow(2, 13) * RK_14_1 + pow(2, 14) * RK_15_1 + pow(2, 15) * RK_16_1);
	printf("密钥K0：0X%04X\n", key);

	//uint16_t RK0AndT_13 = L1 ^ R0;
	//printf("密钥K0：0X%04X\n", RK0AndT_13);
	return;
}
