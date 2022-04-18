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
		// pt[0] = 0x0123; pt[1] = 0x89ab;
		cout << "加密轮数：";
		cin >> round;
		seedkey[0] = 0xb0f5;
		seedkey[1] = 0x8c1e;
		seedkey[2] = 0x7685;
		seedkey[3] = 0xc167;
		ifstream ms;
		ms.open("m.txt", ios::in | ios::app);
		string line;
		stringstream str2digit;
		for (int i = 0; i < 128; i++) {
			getline(ms, line);
			str2digit.clear();
			str2digit.str("");
			str2digit << line;
			str2digit >> hex >> pt[0] >> pt[1];
			Enc(pt, ct, seedkey, round);
			printf("%04X%04X\n", ct[0], ct[1]);
		}
		ms.close();
		return 0;
	}
	else if (r <= 4) {
		cout << "模式1：穷举所有可能的密钥\t" << "模式2：多次计算得出唯一密钥" << endl << "输入模式：";
		int mode = 0;
		cin >> mode;
		if (mode == 1) {
			analysis1_4Rounds(r);
			cout << "可能的密钥已写入csv文件" << endl;
		}
		else if(mode == 2)
		{
			cout << "重复次数：";
			int n = 0;
			cin >> n;
			analysis1_4RoundsAccurate(r, n);
		}
		else {
			cout << "未能识别模式" << endl;
		}
	}

	return 0;
}
