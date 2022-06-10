#include <iostream>
#include <string>
#include <cstring>

#include "CryptographicAlgorithm.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc != 7) {
        cout << "用法：./a.out k0 k1 k2 k3 r1 r2" << endl;
        cout << "例子：./a.out 0123 4567 89ab cdef 4 3" << endl;
        cout << "意味着输入的密文是使用0123 4567 89ab cdef经过4轮加密的，现在要解密3轮" << endl;
        return 1;
    }
    int r1 = atoi(argv[5]);
    int r2 = atoi(argv[6]);
    cout << "解密" << r2 << "轮，" << "输入密文：" << endl;

    u16 seedkey[4];
    seedkey[0] = stoi(argv[1], nullptr, 16);
    seedkey[1] = stoi(argv[2], nullptr, 16);
    seedkey[2] = stoi(argv[3], nullptr, 16);
    seedkey[3] = stoi(argv[4], nullptr, 16);

    cin.setf(ios_base::hex, ios_base::basefield);
    uint32_t p_out, c_in;
    u16 p[2], c[2];
    while (1) {
        cin >> c_in;
        memcpy(&c[0], ((u16*)(&c_in) + 1), 2);
        memcpy(&c[1], (u16*)(&c_in), 2);
        Dec(p, c, seedkey, r1, r2);
        memcpy((u16*)&p_out + 1, &p[0], 2);
        memcpy(&p_out, &p[1], 2);
        printf("%08x\n", p_out);
    }
}