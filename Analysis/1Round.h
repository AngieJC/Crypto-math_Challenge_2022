#pragma once
#include <iostream>
#include <stdio.h>

using namespace std;

void analysis1Round() {
	cout << "选择明文为：L0=0, R0=DEC(46811)=HEX(B6DB)" << endl << "输入密文(十六进制)：";
	uint32_t c = 0;
	cin.setf(ios_base::hex, ios_base::basefield);
	cin >> c;

	// 将c拆成两部分
	uint16_t R0 = 46811, L1 = 0;
	memcpy(&L1, ((uint8_t*)&c) + 2, 2);

	uint16_t RK0AndT_13 = L1 ^ R0;
	printf("密钥K0：0X%04X\n", RK0AndT_13);

	return;
}