#include "Tiaozhansai2022.h"

#include <iostream>
#include <stdio.h>

using namespace std;

int main(void)
{
	u16 seedkey[4];
	u16 pt[2], ct[2], r = 0;

	// pt[0]ΪL0��pt[1]ΪR0
	// pt[0] = 0; pt[1] = 46811;
	// seedkey[0] = 0x0123;
	cout << "����pt[0], pt[1](ʮ������)��";
	cin.setf(ios_base::hex, ios_base::basefield);
	cin >> pt[0] >> pt[1];
	cout << "������Կ(ʮ������)��";
	cin >> seedkey[0] >> seedkey[1] >> seedkey[2] >> seedkey[3];
	cin.setf(ios_base::dec, ios_base::basefield);
	cout << "������";
	cin >> r;
	/*seedkey[0] = 0x0123;
	seedkey[1] = 0x4567;
	seedkey[2] = 0x89ab;
	seedkey[3] = 0xcdef;*/

	Enc(pt, ct, seedkey, r);

	//cout << hex << ct[0] << ct[1] << endl;
	// ct[0]ΪRi��ct[1]ΪLi
	printf("���ģ�%04X%04X\n", ct[0], ct[1]);

	// system("pause");
	return 0;
}