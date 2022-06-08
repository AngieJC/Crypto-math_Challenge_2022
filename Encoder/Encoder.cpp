#include <iostream>
#include <string>
#include "CryptographicAlgorithm.h"

using namespace std;

int main(int argc, char ** argv) {
    if (argc != 6) {
        cout << "用法：./a.out k0 k1 k2 k3 r" << endl;
        cout << "例子：./a.out 0123 4567 89ab cdef 4" << endl;
    }
    int r = atoi(argv[5]);
    cout << "加密" << r << "轮，" << "输入明文：" << endl;

    u16 seedkey[4];
    seedkey[0] = stoi(argv[1], nullptr, 16);
    seedkey[1] = stoi(argv[2], nullptr, 16);
    seedkey[2] = stoi(argv[3], nullptr, 16);
    seedkey[3] = stoi(argv[4], nullptr, 16);

    cin.setf(ios_base::hex, ios_base::basefield);
    uint32_t p_in, c_out;
    u16 p[2], c[2];
    while (1) {
        cin >> p_in;
        memcpy(&p[0], ((u16*)(&p_in) + 1), 2);
        memcpy(&p[1], (u16*)(&p_in), 2);
        Enc(p, c, seedkey, r);
        memcpy((u16*)&c_out + 1, &c[0], 2);
        memcpy(&c_out, &c[1], 2);
        printf("%08x\n", c_out);
    }
}