# 2022届密码数学竞赛代码
1. Analysis：主分析程序
2. BurpDiff：差分遍历，作废
3. Crypto-math_Challenge_2022：J算法官方实现
4. Decoder：解密应答器
5. DifferentialCryptanalysis：差分分析，作废
6. Encoder：加密应答器

### 依赖
1. Gurobi
2. MPI
3. pthread(Linux内置，Windows需要额外安装)
4. gurobipy(Python环境)
5. Windows下直接使用vs2022编译程序
6. Linux下分别在Analysis、Encoder和Decoder目录执行make

### 主程序Analysis
#### Windows
1. 需要Analysis.exe和pthreadVC3.dll在同一个文件夹
2. 运行指令：Analysis.exe
#### Linux
1. 运行指令：./Analysis
#### 注意，为提高7-12轮中间相遇攻击速度，加密应答器内置成Analysis的一个Encoder类，如需攻击其他密钥，请修改Encoder类的私有变量Key，在MITM.h第60行

### 加密应答器Encoder
* Windows:  Encoder.exe k0 k1 k2 k3 k4 r
* Linux:    ./Encoder k0 k1 k2 k3 k4 r
* 例如:     ./Encoder 0123 4567 89ab cdef 7 为使用0123 4567 89ab cdef加密7轮，随后每输入一个明文就会相应地输出一个密文

### 解密应答器Decoder
* Windows:  Decoder.exe k0 k1 k2 k3 k4 r1 r2
* Linux:    ./Decoder k0 k1 k2 k3 k4 r1 r2
* ./Decoder 0123 4567 89ab cdef 9 7 为使用0123 4567 89ab cdef把9轮密文解密7轮(解密到2轮)
* ./Decoder 0123 4567 89ab cdef 9 9 为使用0123 4567 89ab cdef把9轮密文解密成明文

### 超级多项式搜索工具python/JcubeAttack.py
1. python JcubeAttack.py
2. 立方的设置在JcubeAttack.py第345行
3. 默认搜索$L^r_0$的超级多项式，如需更改，参考JcubeAttack.py第287-292行
