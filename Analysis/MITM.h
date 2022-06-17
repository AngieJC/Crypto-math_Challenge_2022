#pragma once

#ifdef _LINUX
	#pragma comment(lib, "pthreadVC3.lib")
#endif // _LINUX


#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <bitset>
#include <time.h>
#include <cmath>
#include <chrono>
#include <pthread.h>

#include "CryptographicAlgorithm.h"

struct KeyNode { // 每一个键对应一个链表，链表节点存储可行密钥和下一个可行密钥的指针
	u16 k = 0;
	KeyNode* next = NULL;
};

struct KeyNode2 { // 每一个键对应一个链表，链表节点存储可行密钥和下一个可行密钥的指针
	u16 k2 = 0, k3 = 0;
	KeyNode2* next = NULL;
};

struct Key { // 6轮中所有可行的密钥
	u16 k[4] = { 0 };
	// Key* next = NULL;
};

struct args4MultiThread {
	// 哈希表
	unordered_map<uint32_t, KeyNode2*>* cAndKeys;
	// 匹配成功的密钥
	// vector<Key*>* keys;
	// 线程ID和总线程数
	u16 UID, nthreads;
	// 加密轮数
	int r;
	// 用于验证匹配的明密文对
	u16 p_verify1[2], c_verify1[2], p_verify2[2], c_verify2[2];
};

void MITM4();
void MITM5_6(int r);
void MITM7_8(int r);
void *verifyMultiThread(void * ptr);

void MITM(int r) {
	if (r == 4) {
		MITM4();
	}
	else if (r == 5 || r == 6) {
		MITM5_6(r);
	}
	else if (r == 7 || r == 8) {
		MITM7_8(r);
	}
}

void MITM4()
{
	cout << "加密：\nffffffff\n11111111" << endl;
	cout << "输入对应密文：" << endl;
	cin.setf(ios_base::hex, ios_base::basefield);
	uint32_t c_, c_verify_;
	cin >> c_ >> c_verify_;
	u16 c[2], c_verify[2];
	memcpy(c, (u16*)(&c_) + 1, 2);
	memcpy(c + 1, (u16*)(&c_), 2);
	memcpy(c_verify, (u16*)(&c_verify_) + 1, 2);
	memcpy(c_verify + 1, (u16*)(&c_verify_), 2);

	auto start = high_resolution_clock::now(); // 开始计时
	// 建表
	unordered_map<uint32_t, KeyNode*> cAndKeys;
	u16 p[2] = { 0xffff, 0xffff }, c2[2]; // c2是加密2轮后的中间变量
	u16 guesskey[4] = { 0x0000 };
	int i = 0;
	uint32_t c2_ = 0; // c2_是c2左右拼接起来
	for (guesskey[0] = 0, i = 0; i <= 0xffff; guesskey[0]++, i++) {
		Enc(p, c2, guesskey, 2);
		memcpy(((u16*)&c2_) + 1, &c2[0], 2);
		memcpy(&c2_, &c2[1], 2);
		if (cAndKeys.find(c2_) == cAndKeys.end()) {
			// 键第一次出现
			cAndKeys[c2_] = (KeyNode*)malloc(sizeof(KeyNode));
			cAndKeys[c2_]->k = guesskey[0];
			cAndKeys[c2_]->next = NULL;
		}
		else {
			KeyNode* temp = cAndKeys[c2_];
			while (temp->next) {
				temp = temp->next;
			}
			temp->next = (KeyNode*)malloc(sizeof(KeyNode));
			temp->next->k = guesskey[0];
			temp->next->next = NULL;
		}
	}

	// 查表
	u16 p2[2], p_verify_now[2] = { 0x1111, 0x1111 }, c_verify_now[2]; // p2是解密2轮后的中间变量
	i = 0;
	uint32_t p2_ = 0; // p2_是p2左右拼接起来
	printf("------------------------\n");
	for (guesskey[1] = 0, i = 0; i < 0xffff; guesskey[1]++, i++) {
		Dec(p2, c, guesskey, 4, 2);
		memcpy(((u16*)&p2_) + 1, &p2[0], 2);
		memcpy(&p2_, &p2[1], 2);
		if (cAndKeys.find(p2_) != cAndKeys.end()) {
			// 命中，验证k0, k1
			KeyNode* temp = cAndKeys[p2_];
			while (temp) {
				guesskey[0] = temp->k;
				Enc(p_verify_now, c_verify_now, guesskey, 4);
				if (c_verify_now[0] == c_verify[0] && c_verify_now[1] == c_verify[1]) {
					printf("K0:%04x\tK1:%04x\n", guesskey[0], guesskey[1]);
				}
				temp = temp->next;
			}
		}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "Time: " << duration.count() / 1000000 << "s" << endl;
}

void MITM5_6(int r)
{
	cout << "加密：\n00000000\nffffffff\n11111111" << endl;
	cout << "输入对应密文：" << endl;
	cin.setf(ios_base::hex, ios_base::basefield);
	uint32_t c_, c_verify_, c_verify_2;
	cin >> c_ >> c_verify_ >> c_verify_2;
	u16 c[2], c_verify[2], c_verify2[2];
	memcpy(c, (u16*)(&c_) + 1, 2);
	memcpy(c + 1, (u16*)(&c_), 2);
	memcpy(c_verify, (u16*)(&c_verify_) + 1, 2);
	memcpy(c_verify + 1, (u16*)(&c_verify_), 2);
	memcpy(c_verify2, (u16*)(&c_verify_2) + 1, 2);
	memcpy(c_verify2 + 1, (u16*)(&c_verify_2), 2);

	auto start = high_resolution_clock::now(); // 开始计时
	// 建表
	unordered_map<uint32_t, KeyNode*> cAndKeys;
	u16 p2[2], guesskey[4] = { 0x0000 }; // p2是解密2轮后的中间变量
	uint32_t p2_; // p2_是p2左右拼接起来
	int i = 0;
	for (; i < 0xffff; i++, guesskey[2]++) {
		Dec(p2, c, guesskey, r, r - 4);
		memcpy(((u16*)&p2_) + 1, &p2[0], 2);
		memcpy(&p2_, &p2[1], 2);
		if (cAndKeys.find(p2_) == cAndKeys.end()) {
			// 键第一次出现
			cAndKeys[p2_] = (KeyNode*)malloc(sizeof(KeyNode));
			cAndKeys[p2_]->k = guesskey[2];
			cAndKeys[p2_]->next = NULL;
		}
		else {
			KeyNode* temp = cAndKeys[p2_];
			while (temp->next) {
				temp = temp->next;
			}
			temp->next = (KeyNode*)malloc(sizeof(KeyNode));
			temp->next->k = guesskey[2];
			temp->next->next = NULL;
		}
	}

	// 查表
	vector<Key*> keys;
	u16 p[2] = { 0x0000, 0x0000 }, c4[2]; // c4是加密4轮后的中间变量
	uint32_t c4_ = 0; // c4_是c4左右拼接起来
	u16 possibleK0[8] = { 0b0000000000000000, 0b0000000000000010, 0b0000000001000000, 0b0000000001000010, 0b0001000000000010, 0b0001000001000000, 0b0001000001000000, 0b0001000001000010 };
	for (int j = 0; j < 8; j++) {
		guesskey[0] = possibleK0[j];
		i = 0;
		guesskey[1] = 0;
		for (; i <= 0xffff; i++, guesskey[1]++) {
			Enc(p, c4, guesskey, 4);
			memcpy(((u16*)&c4_) + 1, &c4[0], 2);
			memcpy(&c4_, &c4[1], 2);
			if (cAndKeys.find(c4_) != cAndKeys.end()) {
				// 命中，暂存k0, k1, k2
				KeyNode* temp1 = cAndKeys[c4_];
				while (temp1) {
					Key* temp2 = (Key*)malloc(sizeof(Key));
					temp2->k[0] = guesskey[0];
					temp2->k[1] = guesskey[1];
					temp2->k[2] = temp1->k;
					keys.insert(end(keys), temp2);
					temp1 = temp1->next;
				}
			}
		}
	}
	// 第一次验证
	u16 p_verify_now[2] = { 0xffff, 0xffff }, c_verify_now[2];
	vector<Key*> accurateKeys;
	Key* temp;
	for (int i = 0; i < keys.size(); i++) {
		Key* nowKey = keys[i];
		u16 area1, area2, area3, area4;
		for (area1 = 0; area1 <= 1; area1++) {
			for (area2 = 0; area2 <= 0b1111; area2++) {
				for (area3 = 0; area3 <= 0b11111; area3++) {
					for (area4 = 0; area4 <= 0b111; area4++) {
						guesskey[0] = nowKey->k[0] ^ area1 ^ (area2 << 2) ^ (area3 << 7) ^ (area4 << 13);
						guesskey[1] = nowKey->k[1];
						guesskey[2] = nowKey->k[2];
						Enc(p_verify_now, c_verify_now, guesskey, r);
						if (c_verify_now[0] == c_verify[0] && c_verify_now[1] == c_verify[1]) {
							// printf("K0:%04x\tK1:%04x\tK2:%04x\n", guesskey[0], guesskey[1], guesskey[2]);
							temp = (Key*)malloc(sizeof(Key));
							temp->k[0] = guesskey[0];
							temp->k[1] = guesskey[1];
							temp->k[2] = guesskey[2];
							accurateKeys.insert(end(accurateKeys), temp);
						}
					}
				}
			}
		}
	}
	// 第二次验证
	printf("------------------------\n");
	u16 p_verify_now2[2] = { 0x1111, 0x1111 }, c_verify_now2[2];
	for (int i = 0; i < accurateKeys.size(); i++) {
		Key* nowKey = accurateKeys[i];
		guesskey[0] = nowKey->k[0];
		guesskey[1] = nowKey->k[1];
		guesskey[2] = nowKey->k[2];
		Enc(p_verify_now2, c_verify_now2, guesskey, r);
		if (c_verify_now2[0] == c_verify2[0] && c_verify_now2[1] == c_verify2[1]) {
			printf("K0:%04x\tK1:%04x\tK2:%04x\n", guesskey[0], guesskey[1], guesskey[2]);
		}
	}

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "Time: " << duration.count() / 1000000 << "s" << endl;
}


void MITM7_8(int r) {
	cout << "使用多少线程：";
	int nthreads = 0;
	cin >> nthreads;
	cout << "加密：\nffffffff\n11111111" << endl;
	cout << "输入对应密文：" << endl;
	cin.setf(ios_base::hex, ios_base::basefield);
	uint32_t c_verify_, c_verify_2;
	cin >> c_verify_ >> c_verify_2;
	u16 c_verify[2], c_verify2[2];
	memcpy(c_verify, (u16*)(&c_verify_) + 1, 2);
	memcpy(c_verify + 1, (u16*)(&c_verify_), 2);
	memcpy(c_verify2, (u16*)(&c_verify_2) + 1, 2);
	memcpy(c_verify2 + 1, (u16*)(&c_verify_2), 2);

	auto start = high_resolution_clock::now(); // 开始计时

	/*阶段1：加密4轮，建表c'->key[2, 3]*/
	// 4轮中间变量取0000000000000000  0000000000000100需在解密方向猜{0..15}\{2, 8}，需在加密方向猜{1, 4, 6, 10, 12, 13, 15}
	u16 m4[2] = {0b0000000000000000, 0b0000000000000100}, guess1, guess4, guess6, guess10, guess12_13, guess15, guessKey[4] = {0}, c[2];
	// 不需要猜的k1[2]和k1[8]将k1分成两3部分，第一部分2位{0, 1}，第二部分5位{3, 4, 5, 6, 7}，第三部分7位{9..15}
	uint32_t c_; // p_是p左右拼接
	int k0Flag = 0;
	unordered_map<uint32_t, KeyNode2*> cAndKeys;
	KeyNode2* temp = NULL;
	for (guess1 = 0; guess1 <= 0b1; guess1++) {
		for (guess4 = 0; guess4 <= 0b1; guess4++) {
			for (guess6 = 0; guess6 <= 0b1; guess6++) {
				for (guess10 = 0; guess10 <= 0b1; guess10++) {
					for (guess12_13 = 0; guess12_13 <= 0b11; guess12_13++) {
						for (guess15 = 0; guess15 <= 0b1; guess15++) {
							guessKey[2] = (guess1 << 14) ^ (guess4 << 11) ^ (guess6 << 9) ^ (guess10 << 5) ^ (guess12_13 << 2) ^ guess15;
							for (k0Flag = 0, guessKey[3] = 0; k0Flag <= 0xffff; k0Flag++, guessKey[3]++) { // 由于guessKey[3]最大值为0xffff，再增加就会等于0，直接使用guessKeyDecode[0]判断循环是否终止将会导致死循环
								Enc(m4, c, guessKey, 4, 4);
								memcpy(((u16*)&c_) + 1, &c[0], 2);
								memcpy(&c_, &c[1], 2);
								if (cAndKeys.find(c_) == cAndKeys.end()) {
									// 键第一次出现
									// 头插，第一个节点不存储任何信息，方便后续插入
									cAndKeys[c_] = (KeyNode2*)malloc(sizeof(KeyNode2));
									cAndKeys[c_]->next = (KeyNode2*)malloc(sizeof(KeyNode2));
									cAndKeys[c_]->next->k2 = guessKey[2];
									cAndKeys[c_]->next->k3 = guessKey[3];
								}
								else {
									temp = (KeyNode2*)malloc(sizeof(KeyNode2));
									temp->k2 = guessKey[2];
									temp->k3 = guessKey[3];
									temp->next = cAndKeys[c_]->next;
									cAndKeys[c_]->next = temp;
								}
							}
						}
					}
				}
			}
		}
	}
	cout << "建表完成，开始匹配" << endl;

	/*阶段2：在线阶段，m4解密4轮，得到明文后再访问加密应答器加密r轮得到c，然后查表*/
	// 创建多线程
	pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * nthreads);
	for (int i = 0; i < nthreads; i++) {
		args4MultiThread* args = (args4MultiThread*)malloc(sizeof(args4MultiThread));
		args->cAndKeys = &cAndKeys;
		args->UID = i;
		args->nthreads = nthreads;
		args->r = r;
		args->p_verify1[0] = 0xffff;
		args->p_verify1[1] = 0xffff;
		args->p_verify2[0] = 0x1111;
		args->p_verify2[1] = 0x1111;
		args->c_verify1[0] = c_verify[0];
		args->c_verify1[1] = c_verify[1];
		args->c_verify2[0] = c_verify2[0];
		args->c_verify2[1] = c_verify2[1];
		pthread_create(threads + i, NULL, verifyMultiThread, (void*)(args));
	}
	for (int i = 0; i < nthreads; i++) {
		pthread_join(threads[i], NULL);
	}


	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "Time: " << duration.count() / 1000000 << "s" << endl;
}

void* verifyMultiThread(void* ptr) {
	args4MultiThread* args = (args4MultiThread*)ptr;

	vector<Key*> keys;
	u16 m4[2] = { 0b0000000000000000, 0b0000000000000100 }, p[2] = { 0 }, c[2] = {0};
	u16 realKey[4] = { 0x0123, 0x4567, 0x89ab, 0xcdef }, guessKey[4] = {0};
	uint32_t c_;

	// 确定本线程需要遍历的空间
	// 所有线程一共需要遍历k1：01*3456789abcde*					*为不需要遍历，设为0
	// 首先确定总线程数需要占几个比特，那么每线程就少遍历几比特
	// 假定最多有32个线程，那么每个线程需要遍历01*3456789xxxxx*	xxxxx为线程号
	u16 threadLength = (u16)(log(args->nthreads) / log(2));
	u16 area2Bound = 0b111111111111 >> threadLength, area2Remove = 1 + threadLength;
	int key0flag = 0;
	for (u16 area1 = 0; area1 <= 0b11; area1++) {
		for (u16 area2 = 0; area2 <= area2Bound; area2++) {
			guessKey[1] = (area1 << 14) ^ (area2 << area2Remove) ^ (args->UID << 1);
			for (guessKey[0] = 0, key0flag = 0; key0flag <= 0xffff; key0flag++, guessKey[0]++) {
				// 至此确定了每一次猜的密钥长啥样，接下来要用这个密钥解密4轮再访问加密应答器
				Dec(p, m4, guessKey, 4, 4);

				// 这里应该是在线访问的，写到程序里快一些
				Enc(p, c, realKey, args->r); // 这里是真实密钥
				memcpy(((u16*)&c_) + 1, &c[0], 2);
				memcpy(&c_, &c[1], 2);

				// 查表
				if (args->cAndKeys->find(c_) != args->cAndKeys->end()) {
					// 命中，暂存k0, k1, k2, k3
					KeyNode2* temp1 = (*args->cAndKeys)[c_];
					while (temp1->next) {
						Key* temp2 = (Key*)malloc(sizeof(Key));
						temp2->k[0] = guessKey[0];
						temp2->k[1] = guessKey[1];
						temp2->k[2] = temp1->next->k2;
						temp2->k[3] = temp1->next->k3;
						keys.push_back(temp2);
						temp1 = temp1->next;
					}
				}
			}
		}
	}

	// 第一次验证
	u16 p_verify_now[2] = { 0xffff, 0xffff }, c_verify_now[2];
	vector<Key*> accurateKeys;
	Key* temp;
	for (long long i = 0; i < keys.size(); i++) {
		Key* nowKey = keys[i];
		u16 k1area1, k1area2, k2area1, k2area2, k2area3, k2area4, k2area5, k2area6;
		for (k1area1 = 0; k1area1 <= 0b1; k1area1++) {
			for (k1area2 = 0; k1area2 <= 0b1; k1area2++) {
				for (k2area1 = 0; k2area1 <= 0b1; k2area1++) {
					for (k2area2 = 0; k2area2 <= 0b11; k2area2++) {
						for (k2area3 = 0; k2area3 <= 0b1; k2area3++) {
							for (k2area4 = 0; k2area4 <= 0b111; k2area4++) {
								for (k2area5 = 0; k2area5 <= 0b1; k2area5++) {
									for (k2area6 = 0; k2area6 <= 0b1; k2area6++) {
										guessKey[0] = nowKey->k[0];
										guessKey[1] = (k1area1 << 13) ^ (k1area1 << 7) ^ nowKey->k[1];
										guessKey[2] = (k2area1 << 15) ^ (k2area2 << 12) ^ (k2area3 << 10) ^ (k2area4 << 6) ^ (k2area5 << 4) ^ (k2area6 << 1) ^ nowKey->k[2];
										guessKey[3] = nowKey->k[3];
										Enc(p_verify_now, c_verify_now, guessKey, args->r);
										if (c_verify_now[0] == args->c_verify1[0] && c_verify_now[1] == args->c_verify1[1]) {
											temp = (Key*)malloc(sizeof(Key));
											temp->k[0] = guessKey[0];
											temp->k[1] = guessKey[1];
											temp->k[2] = guessKey[2];
											temp->k[3] = guessKey[3];
											accurateKeys.push_back(temp);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// 第二次验证
	u16 p_verify_now2[2] = { 0x1111, 0x1111 }, c_verify_now2[2];
	for (long long i = 0; i < accurateKeys.size(); i++) {
		Key* nowKey = accurateKeys[i];
		guessKey[0] = nowKey->k[0];
		guessKey[1] = nowKey->k[1];
		guessKey[2] = nowKey->k[2];
		guessKey[3] = nowKey->k[3];
		Enc(p_verify_now2, c_verify_now2, guessKey, args->r);
		if (c_verify_now2[0] == args->c_verify2[0] && c_verify_now2[1] == args->c_verify2[1]) {
			printf("K0:%04x\tK1:%04x\tK2:%04x\tK3:%04x\n", guessKey[0], guessKey[1], guessKey[2], guessKey[3]);
		}
	}

	return NULL;
}