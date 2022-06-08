#pragma once
#include <stdio.h>
#include <stdint.h>

#define TOTALROUND 65535//注：本算法不设置最高轮数，以攻击的轮数进行评分。此处以40为例，可更改
#define u16 uint16_t
#define LCS(x,a) ((x)<<a ^ (x) >> (16-a))//将16比特串循环左移a位



void KeySchedual(u16 seedkey[], u16 roundkey[])//密钥扩展算法
{
	int idx;

#if (TOTALROUND > 8)
	for (idx = 0; idx < 4; idx++)
	{
		roundkey[2 * idx] = seedkey[idx];
		roundkey[2 * idx + 1] = ~seedkey[idx];
	}
	for (idx = 8; idx < TOTALROUND; idx++)
	{
		if ((idx & 1) == 0)
			roundkey[idx] = ~roundkey[idx - 8] ^ ((idx - 8) >> 1);
		else roundkey[idx] = ~roundkey[idx - 1];
	}
#else
	for (idx = 0; idx < TOTALROUND; idx++)
	{
		if ((idx & 1) == 0) roundkey[idx] = seedkey[idx >> 1];
		else roundkey[idx] = ~roundkey[idx - 1];
	}
#endif
}

void RoundFun(u16 s[], u16 roundkey[], int round)//轮函数,s为输入状态，roundkey为轮密钥，round为当前轮编号（从0开始记）
{
	u16 temp, Li, Ri;

	temp = s[0] & roundkey[round] ^ s[1] ^ round;
	temp = temp << 2 & temp << 1 ^ temp;//S
	temp = LCS(temp, 3) ^ LCS(temp, 9) ^ LCS(temp, 14);//P
	//s[0] ^= temp; s[1] ^= temp & roundkey[round];
	Li = s[1] ^ (roundkey[round] & temp);
	Ri = s[0] ^ temp;
	s[0] = Li;
	s[1] = Ri;
}

void Enc(u16 pt[], u16 ct[], u16 seedkey[], int round)//加密函数，pt为明文，ct为密文，seedkey为种子密钥，round为总轮数
{
	u16 s[2];
	u16 roundkey[TOTALROUND];
	int r;

	if (round > TOTALROUND)
	{
		printf("error: 密钥扩展轮数小于总轮数，请更改头文件第15行使TOTALROUND的值大于总轮数\n");
		return;
	}

	KeySchedual(seedkey, roundkey);

	s[0] = pt[0]; s[1] = pt[1];
	for (r = 0; r < round; r++)
	{
		RoundFun(s, roundkey, r);
		// 		printf("%.4x %.4x\n",s[0],s[1]);
	}
	ct[0] = s[0]; ct[1] = s[1];
}