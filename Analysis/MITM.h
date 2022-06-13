#pragma once
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

#include "CryptographicAlgorithm.h"

struct KeyNode { // 每一个键对应一个链表，链表节点存储可行密钥和下一个可行密钥的指针
	u16 k = 0;
	KeyNode* next = NULL;
};

struct Key { // 6轮中所有可行的密钥
	u16 k[4] = { 0 };
	// Key* next = NULL;
};

void MITM4();
void MITM6();

void MITM(int r) {
	if (r == 4) {
		MITM4();
	}
	else if (r == 6) {
		MITM6();
	}
}

void MITM4()
{
	cout << "加密0xffffffff" << endl;
	cout << "输入对应密文：" << endl;
	cin.setf(ios_base::hex, ios_base::basefield);
	uint32_t c_;
	cin >> c_;
	u16 c[2];
	memcpy(c, (u16*)( & c_) + 1, 2);
	memcpy(c + 1, (u16*)( & c_), 2);
	
	auto start = high_resolution_clock::now(); // 开始计时
	// 建表
	unordered_map<uint32_t, KeyNode*> cAndKeys;
	u16 p[2] = { 0xffff, 0xffff }, c2[2]; // c2是加密2轮后的中间变量
	u16 guesskey[4] = {0x0000};
	int i = 0;
	uint32_t c2_ = 0; // c2_是c2左右拼接起来
	for (guesskey[0] = 0, i = 0; i <= 0xffff; guesskey[0]++, i++) {
		Enc(p, c2, guesskey, 2);
		memcpy(((u16*)&c2_) + 1, &c2[0], 2);
		memcpy(&c2_ , &c2[1], 2);
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
	u16 p2[2]; // p2是解密2轮后的中间变量
	i = 0;
	uint32_t p2_ = 0; // p2_是p2左右拼接起来
	for (guesskey[1] = 0, i = 0; i < 0xffff; guesskey[1]++, i++) {
		Dec(p2, c, guesskey, 4, 2);
		memcpy(((u16*)&p2_) + 1, &p2[0], 2);
		memcpy(&p2_, &p2[1], 2);
		if (cAndKeys.find(p2_) != cAndKeys.end()) {
			// 命中，输出k0, k1
			printf("----------------\n");
			printf("K1: %04x\nK0: \n", guesskey[1]);
			KeyNode* temp = cAndKeys[p2_];
			while (temp) {
				printf("%04x\n", temp->k);
				temp = temp->next;
			}
		}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "Time: " << duration.count() / 1000000 << "s" << endl;
}

void MITM6()
{
	cout << "加密：\n0x00000000\n0xffffffff" << endl;
	cout << "输入对应密文：" << endl;
	cin.setf(ios_base::hex, ios_base::basefield);
	uint32_t c_, c_verify_;
	cin >> c_ >> c_verify_;
	u16 c[2], c_verify[2], p_verify[2] = {0xffff, 0xffff};
	memcpy(c, (u16*)(&c_) + 1, 2);
	memcpy(c + 1, (u16*)(&c_), 2);
	memcpy(c_verify, (u16*)(&c_verify_) + 1, 2);
	memcpy(c_verify + 1, (u16*)(&c_verify_), 2);

	auto start = high_resolution_clock::now(); // 开始计时
	// 建表
	unordered_map<uint32_t, KeyNode*> cAndKeys;
	u16 p2[2], guesskey[4] = { 0x0000 }; // p2是解密2轮后的中间变量
	uint32_t p2_; // p2_是p2左右拼接起来
	int i = 0;
	for (; i < 0xffff; i++, guesskey[2]++) {
		Dec(p2, c, guesskey, 6, 2);
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
	u16 p[2] = {0x0000, 0x0000}, c4[2]; // c4是加密4轮后的中间变量
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

	u16 p_verify_now[2] = { 0xffff, 0xffff }, c_verify_now[2];
	for (int i = 0; i < keys.size(); i++) {
		Key* nowKey = keys[i];
		u16 area1, area2, area3, area4;
		for (area1 = 0; area1 <= 1; area1++) {
			// guesskey[0] = nowKey->k[0] ^ area1;
			for (area2 = 0; area2 <= 0b1111; area2++) {
				// guesskey[0] ^= (area2 << 2);
				for (area3 = 0; area3 <= 0b11111; area3++) {
					// guesskey[0] ^= (area3 << 7);
					for (area4 = 0; area4 <= 0b111; area4++) {
						// guesskey[0] ^= (area4 << 13);
						guesskey[0] = nowKey->k[0] ^ area1 ^ (area2 << 2) ^ (area3 << 7) ^ (area4 << 13);
						guesskey[1] = nowKey->k[1];
						guesskey[2] = nowKey->k[2];
						Enc(p_verify_now, c_verify_now, guesskey, 6);
						if (c_verify_now[0] == c_verify[0] && c_verify_now[1] == c_verify[1]) {
							printf("K0:%04x\tK1:%04x\tK2:%04x\n", guesskey[0], guesskey[1], guesskey[2]);
						}
					}
				}
			}
		}
	}
	
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "Time: " << duration.count() / 1000000 << "s" << endl;
}
