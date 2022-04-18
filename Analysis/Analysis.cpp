/*
* Author:	AngieJC
* Mail:		htk90uggk@outlook.com
*/

#include <iostream>
#include <cstring>
#include <stdio.h>

#include "Rounds.h"
#include "CryptographicAlgorithm.h"

using namespace std;

int main() {
	int r = 0;
	cout << "输入要分析的轮数：";
	cin >> r;

	/*switch (r)
	{
	case 1:
		analysis1Round();
		break;
	case 2:
		analysis2Rounds();
		break;
	default:
		if (r > 2) {
			analysisnRounds(r);
		}
		break;
	}*/

	if (r == -1) { // 生成测试用例
		u16 seedkey[4];
		u16 pt[2], ct[2], round = 0;
		pt[0] = 0x0123; pt[1] = 0x89ab;
		cout << "加密轮数：";
		cin >> round;
		seedkey[0] = 0x0123;
		seedkey[1] = 0x4567;
		seedkey[2] = 0x89ab;
		seedkey[3] = 0xcdef;
		for (int i = 0; i < 128; i++) {
			Enc(pt, ct, seedkey, round);
			printf("%04X%04X\n", ct[0], ct[1]);
			pt[0]++;
			pt[1]--;
		}
		return 0;
	}

	analysisnRounds(r);

	return 0;
}
