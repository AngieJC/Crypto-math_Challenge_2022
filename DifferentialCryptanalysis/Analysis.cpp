#include "CryptographicAlgorithm.h"
#include <string.h>
#include <random>
#include <iostream>

#define mpairs				4096					// 明密文对数
#define analysisRounds		2						// 分析的轮数
#define encRounds			analysisRounds			// 加密的轮数，应当与analysisRounds一致
#define differentialRounds	analysisRounds - 1
#define plDifferential		0b0000000000000000		// 明文差分
#define prDifferential		0b1000000000000000
#define d1lDifferential		0b1000000000000000		// 密文部分解密(r-1轮)差分
#define d1rDifferential		0b0010000100000100
#define clDifferential		0b0100000000000000		// 由r-1轮差分加密一轮之后的差分，用于过滤密文
#define crDifferential		0b1100000000000000
#define maxKey				0b1111111111111111		// 解密1轮时，猜测的轮密钥为0b0000000000000000到0b1111111111111111

using namespace std;

std::mt19937 gen;
std::uniform_int_distribution<uint32_t> dis(0x0, 0xffffffff);
std::uniform_int_distribution<uint16_t> dis2(0x0, 0xffff);

void init_prng_mt() {
	unsigned int initial_seed = time(NULL);
	gen.seed(initial_seed);
	// printf("[+] PRNG initialized to 0x%08X\n", initial_seed);
}

int main(int argc, char** argv) {
	// ps为明文，cs为密文，ms为cs解密的明文
	uint32_t ps[mpairs] = { 0 };
	unsigned int initial_seed = time(NULL);
	init_prng_mt();
	for (int i = 0; i < mpairs; i++) {
		ps[i] = dis(gen);
	}
	uint32_t cs[mpairs] = { 0 }, cs_[mpairs] = { 0 };
	uint16_t p[2], c[2], c_[2];
	uint16_t seedkey[4] = { dis2(gen), dis2(gen), dis2(gen), dis2(gen) };
	printf("真实密钥：%04X%04X%04X%04X\n", seedkey[0], seedkey[1], seedkey[2], seedkey[3]);
	uint32_t survivedc[mpairs] = { 0 }, survivedc_[mpairs] = { 0 }, survivedCount = 0;
	uint16_t dec1rm[2], dec1rm_[2];
	int guessKs[maxKey] = { 0 };
	int maxcount = -1, maxcountK = -1;
	uint16_t p_test[2], p_test_[2], c_test[2], c_test_[2];
	uint16_t seedkey_test[4];
	int rightPairCount = 0;
	// 所有明文对加密
	for (int i = 0; i < mpairs; i++) {
		memcpy(&p[0], ((uint16_t*)&ps[i]) + 1, 2);
		memcpy(&p[1], ((uint16_t*)&ps[i]), 2);
		// seedkey[0] = dis(gen);
		Enc(p, c, seedkey, encRounds);
		p[0] ^= plDifferential;
		p[1] ^= prDifferential;
		Enc(p, c_, seedkey, encRounds);
		memcpy((uint16_t*)&cs[i] + 1, &c[0], 2);
		memcpy((uint16_t*)&cs[i], &c[1], 2);
		memcpy((uint16_t*)&cs_[i] + 1, &c_[0], 2);
		memcpy((uint16_t*)&cs_[i], &c_[1], 2);
	}

	// 过滤
	for (int i = 0; i < mpairs; i++) {
		memcpy(&c[0], (uint16_t*)&cs[i] + 1, 2);
		memcpy(&c[1], (uint16_t*)&cs[i], 2);
		memcpy(&c_[0], (uint16_t*)&cs_[i] + 1, 2);
		memcpy(&c_[1], (uint16_t*)&cs_[i], 2);
		if (((c[0] ^ c_[0]) == clDifferential) && ((c[1] ^ c_[1]) == crDifferential)) {
			survivedc[survivedCount] = cs[i];
			survivedc_[survivedCount] = cs_[i];
			survivedCount++;
		}
	}

	// 解密1轮
	for (int i = 0; i < mpairs; i++) {
		memcpy(&c[0], (uint16_t*)&cs[i] + 1, 2);
		memcpy(&c[1], (uint16_t*)&cs[i], 2);
		memcpy(&c_[0], (uint16_t*)&cs_[i] + 1, 2);
		memcpy(&c_[1], (uint16_t*)&cs_[i], 2);
		for (int guessK = 0; guessK <= maxKey; guessK++) {
			Dec1r(dec1rm, c, guessK, analysisRounds);
			Dec1r(dec1rm_, c_, guessK, analysisRounds);
			if ((((dec1rm[0] ^ dec1rm_[0]) == d1lDifferential) && ((dec1rm[1] ^ dec1rm_[1]) == d1rDifferential))) {
				guessKs[guessK]++;
			}
		}
	}
	for (int i = 0; i < maxKey; i++) {
		if (guessKs[i] > maxcount) {
			maxcount = guessKs[i];
			maxcountK = i;
		}
	}
	printf("猜测密钥：%04X\n", maxcountK);
	printf("统计次数：%d\n\n", maxcount);

	// 测试r-1轮加密是否以特定概率出现目标差分
	printf("%d轮加密验证\n", analysisRounds - 1);
	// 明文随机，密钥随机
	printf("明文随机，密钥随机，重复%d次\n", mpairs);
	for (int i = 0; i < mpairs; i++) {
		// 明文随机
		memcpy(&p_test[0], ((uint16_t*)&ps[i]) + 1, 2);
		memcpy(&p_test[1], ((uint16_t*)&ps[i]), 2);
		p_test_[0] = p_test[0] ^ plDifferential;
		p_test_[1] = p_test[1] ^ prDifferential;
		// 密钥随机
		seedkey_test[0] = dis(gen);
		seedkey_test[1] = dis(gen);
		seedkey_test[2] = dis(gen);
		seedkey_test[3] = dis(gen);
		Enc(p_test, c_test, seedkey_test, encRounds - 1);
		Enc(p_test_, c_test_, seedkey_test, encRounds - 1);
		//printf("%04X\n", c_test[0] ^ c_test_[0]);
		//printf("%04X\n", c_test[1] ^ c_test_[1]);
		//printf("%04X%04X\n", c_test[0] ^ c_test_[0], c_test[1] ^ c_test_[1]);
		if (((c_test[0] ^ c_test_[0]) == d1lDifferential) && ((c_test[1] ^ c_test_[1]) == d1rDifferential)) {
			rightPairCount++;
			//printf("%04X%04X%04X%04X\n", seedkey_test[0], seedkey_test[1], seedkey_test[2], seedkey_test[3]);
		}
	}
	printf("明密文差分均符合差分对数：%d，概率：%0.4f\n", rightPairCount, (float)((float)rightPairCount / mpairs));

	//// 明文随机，密钥固定
	//printf("明文随机，密钥固定(随机指定且不再变化)，重复%d次\n", mpairs);
	//rightPairCount = 0;
	//// 密钥固定
	//seedkey_test[0] = dis2(gen);
	//seedkey_test[1] = dis2(gen);
	//seedkey_test[2] = dis2(gen);
	//seedkey_test[3] = dis2(gen);
	//for (int i = 1; i < mpairs; i++) {
	//	memcpy(&p_test[0], ((uint16_t*)&ps[i]) + 1, 2);
	//	memcpy(&p_test[1], ((uint16_t*)&ps[i]), 2);
	//	p_test_[0] = p_test[0] ^ plDifferential;
	//	p_test_[1] = p_test[0] ^ prDifferential;
	//	Enc(p_test, c_test, seedkey_test, encRounds - 1);
	//	Enc(p_test_, c_test_, seedkey_test, encRounds - 1);
	//	if (((c_test[0] ^ c_test_[0]) == d1lDifferential) && ((c_test[1] ^ c_test_[1]) == d1rDifferential)) {
	//		rightPairCount++;
	//	}
	//}
	//printf("明密文差分均符合差分对数：%d，概率：%0.4f\n", rightPairCount, (float)((float)rightPairCount / mpairs));

	//// 明文随机，密钥真实
	//printf("明文随机，密钥真实(模拟真实分析情况)，重复%d次\n", mpairs);
	//rightPairCount = 0;
	//// 密钥真实
	//seedkey_test[0] = seedkey[0];
	//seedkey_test[1] = seedkey[1];
	//seedkey_test[2] = seedkey[2];
	//seedkey_test[3] = seedkey[3];
	//for (int i = 1; i < 4096; i++) {
	//	memcpy(&p_test[0], ((uint16_t*)&ps[i]) + 1, 2);
	//	memcpy(&p_test[1], ((uint16_t*)&ps[i]), 2);
	//	p_test_[0] = p_test[0] ^ plDifferential;
	//	p_test_[1] = p_test[0] ^ prDifferential;
	//	Enc(p_test, c_test, seedkey_test, encRounds - 1);
	//	Enc(p_test_, c_test_, seedkey_test, encRounds - 1);
	//	if (((c_test[0] ^ c_test_[0]) == d1lDifferential) && ((c_test[1] ^ c_test_[1]) == d1rDifferential)) {
	//		rightPairCount++;
	//	}
	//}
	//printf("明密文差分均符合差分对数：%d，概率：%0.4f\n\n", rightPairCount, (float)((float)rightPairCount / mpairs));

	return 0;
}