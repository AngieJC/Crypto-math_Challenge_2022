/*
* Author:	AngieJC
* Mail:		htk90uggk@outlook.com
*/

#include <iostream>
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

	analysisnRounds(r);

	return 0;
}