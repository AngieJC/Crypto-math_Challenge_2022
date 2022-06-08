#pragma once
#include <stdio.h>
#include <stdint.h>

#define TOTALROUND 65535//ע�����㷨����������������Թ����������������֡��˴���40Ϊ�����ɸ���
#define u16 uint16_t
#define LCS(x,a) ((x)<<a ^ (x) >> (16-a))//��16���ش�ѭ������aλ



void KeySchedual(u16 seedkey[], u16 roundkey[])//��Կ��չ�㷨
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

void RoundFun(u16 s[], u16 roundkey[], int round)//�ֺ���,sΪ����״̬��roundkeyΪ����Կ��roundΪ��ǰ�ֱ�ţ���0��ʼ�ǣ�
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

void Enc(u16 pt[], u16 ct[], u16 seedkey[], int round)//���ܺ�����ptΪ���ģ�ctΪ���ģ�seedkeyΪ������Կ��roundΪ������
{
	u16 s[2];
	u16 roundkey[TOTALROUND];
	int r;

	if (round > TOTALROUND)
	{
		printf("error: ��Կ��չ����С���������������ͷ�ļ���15��ʹTOTALROUND��ֵ����������\n");
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