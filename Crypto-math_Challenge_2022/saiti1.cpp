#include "Tiaozhansai2022.h"

#include <iostream>
#include <stdio.h>

using namespace std;

int main(void)
{
	u16 seedkey[4];
	u16 pt[2], ct[2], r = 0;

	// pt[0]为L0，pt[1]为R0
	// pt[0] = 0; pt[1] = 46811;
	// seedkey[0] = 0x0123;
	cout << "输入pt[0], pt[1](十六进制)：";
	cin.setf(ios_base::hex, ios_base::basefield);
	cin >> pt[0] >> pt[1];
	cout << "输入密钥(十六进制)：";
	cin >> seedkey[0] >> seedkey[1] >> seedkey[2] >> seedkey[3];
	cin.setf(ios_base::dec, ios_base::basefield);
	cout << "轮数：";
	cin >> r;
	/*seedkey[0] = 0x0123;
	seedkey[1] = 0x4567;
	seedkey[2] = 0x89ab;
	seedkey[3] = 0xcdef;*/

	Enc(pt, ct, seedkey, r);

	//cout << hex << ct[0] << ct[1] << endl;
	// ct[0]为Ri，ct[1]为Li
	printf("密文：%04X%04X\n", ct[0], ct[1]);

	// system("pause");
	return 0;
}