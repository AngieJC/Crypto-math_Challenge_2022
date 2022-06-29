from gurobipy import *

def PrintSuperPoly(m):
    const = 0
    Monomial_char = ["0" for i in range(64)]
    SuperPoly = {}
    nSolutions = m.SolCount
    for solution in range(nSolutions):
        m.setParam(GRB.Param.SolutionNumber, solution)
        flag = 0
        for i in range(64):
            K = m.getVarByName("K_" + str(i))
            Kflag = m.getVarByName("flag_" + str(i))
            if K.Xn > 0.5:
                if Kflag.Xn < 0.5:
                    Monomial_char[i] = "1"
                elif Kflag.Xn > 0.5:
                    Monomial_char[i] = "2"
                flag = 1
        if flag == 0:
            const = const + 1
        else:
            Monomial_str = "".join(Monomial_char)
            Monomial_char = ["0" for i in range(64)]
            if SuperPoly.get(Monomial_str):
                SuperPoly[Monomial_str] = SuperPoly.get(Monomial_str) + 1
            else:
                SuperPoly[Monomial_str] = 1

    print("SuperPoly:")
    if const % 2 == 1:
        print(const, end="\t")
        print("1")
    for item in SuperPoly.items():
        key = item[0]
        times = item[1]
        if times % 2 == 1:
            print(times, end="")
            print("\t", end="")
            for i in range(64):
                if key[i] == "1":
                    print("k", end="")
                    print(i, end=" ")
                elif key[i] == "2":
                    print("!k", end="")
                    print(i, end=" ")
            print()

def KeySchedule(m, r, RK): # 利用主密钥K对轮密钥RK生成约束
    m.update()
    if r / 2 <= 4:
        for i in range(16):
            RK_i_2 = m.getVarByName("RK^" + str(r - 2) + "_" + str(i))
            RK_i_1 = m.getVarByName("RK^" + str(r - 1) + "_" + str(i))
            K_i_1_2 = m.getVarByName("K_" + str(int(((r - 1) / 2)) * 16 + i))
            flag = m.getVarByName("flag_" + str(int(((r - 1) / 2)) * 16 + i))
            m.addConstr(RK_i_1 - flag == 0)
            m.addConstr(RK_i_2 - K_i_1_2 + flag == 0)
            m.addConstr(K_i_1_2 - flag >= 0)

def KeyScheduleSingle(m, r, RK):
    m.update()
    if r / 2 <= 4:
        for i in range(16):
            # RK_i_1 = m.getVarByName("RK^" + str(r - 1) + "_" + str(i))
            K_i_1_2 = m.getVarByName("K_" + str(int(((r - 1) / 2)) * 16 + i))
            flag = m.getVarByName("flag_" + str(int(((r - 1) / 2)) * 16 + i))
            m.addConstr(RK[i] == K_i_1_2)
            m.addConstr(flag == 0)

# 所有未被使用的主密钥应当设置为0
def KeyNotUsed(m, r):
    for i in range(int((r + 1) / 2), 4):
        for j in range(16):
            K = m.getVarByName("K_" + str(i * 16 + j))
            flag = m.getVarByName("flag_" + str(i * 16 + j))
            m.addConstr(K == 0)
            m.addConstr(flag == 0)

# AND
def L1(m, a, b, c, copys):
    if len(copys) == 0:
        m.addConstr(c == a)
        m.addConstr(c == b)
    elif len(copys) == 2:
        m.addConstr(copys[0] <= a)
        m.addConstr(copys[1] <= a)
        m.addConstr(a <= copys[0] + copys[1])
        m.addConstr(c == copys[1])
        m.addConstr(c == b)
    elif len(copys) == 4:
        m.addConstr(copys[0] <= a)
        m.addConstr(copys[1] <= a)
        m.addConstr(a <= copys[0] + copys[1])
        m.addConstr(copys[2] <= b)
        m.addConstr(copys[3] <= b)
        m.addConstr(b <= copys[2] + copys[3])
        m.addConstr(c == copys[1])
        m.addConstr(c == copys[3])

# XOR
def L2(m, a, b, c, copys):
    if len(copys) == 0:
        m.addConstr(c >= b)
        m.addConstr(c == a + b)
    elif len(copys) == 2:
        m.addConstr(copys[0] <= a)
        m.addConstr(copys[1] <= a)
        m.addConstr(a <= copys[0] + copys[1])
        m.addConstr(c >= b)
        m.addConstr(c == copys[1] + b)
    elif len(copys) == 4:
        m.addConstr(copys[0] <= a)
        m.addConstr(copys[1] <= a)
        m.addConstr(a <= copys[0] + copys[1])
        m.addConstr(copys[2] <= b)
        m.addConstr(copys[3] <= b)
        m.addConstr(b <= copys[2] + copys[3])
        m.addConstr(c >= copys[3])
        m.addConstr(c == copys[1] + copys[3])

# 3比特XOR
def L3(m, a, b, c, d, copys):
    m.addConstr(copys[0] <= a)
    m.addConstr(copys[1] <= a)
    m.addConstr(a <= copys[0] + copys[1])
    m.addConstr(copys[2] <= b)
    m.addConstr(copys[3] <= b)
    m.addConstr(b <= copys[2] + copys[3])
    m.addConstr(copys[4] <= c)
    m.addConstr(copys[5] <= c)
    m.addConstr(c <= copys[4] + copys[5])
    m.addConstr(d == copys[1] + copys[3] + copys[5])
    m.addConstr(d >= copys[3] + copys[5])

def GenerateJModel(r):
    m = Model("JSuperPoly")
    L0 = []
    R0 = []
    K = []
    flag = []
    for i in range(16):
        L0.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="L^0_" + str(i)))
        R0.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="R^0_" + str(i)))
    for i in range(64):
        K.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="K_" + str(i)))
        flag.append(m.addVar(lb=0.0, ub=2.0, vtype=GRB.BINARY, name="flag_" + str(i)))
    u = L0 + R0

    for i in range(1, r + 1):
        RK = []
        a = []
        b = []
        constant_i_var = []
        c = []
        d = []
        e = []
        f = []
        g = []
        L = []
        R = []
        con_i = [0 for k in range(16)]
        i_bin_str = bin(i - 1).replace("0b", "")
        for k in range(len(i_bin_str)):
            con_i[k + 16 - len(i_bin_str)] = int(i_bin_str)
        for j in range(16):
            RK.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="RK^" + str(i - 1) + "_" + str(j)))
            a.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="a^" + str(i) + "_" + str(j)))
            b.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="b^" + str(i) + "_" + str(j)))
            if con_i[j] == 1:
                constant_i_var.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="i^" + str(i) + "_" + str(j)))
            c.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="c^" + str(i) + "_" + str(j)))
            e.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="e^" + str(i) + "_" + str(j)))
            f.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="f^" + str(i) + "_" + str(j)))
            g.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="g^" + str(i) + "_" + str(j)))
            L.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="L^" + str(i) + "_" + str(j)))
            R.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="R^" + str(i) + "_" + str(j)))
        for j in range(14):
            d.append(m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="d^" + str(i) + "_" + str(j)))

        # 密钥约束
        if i % 2 == 1 and i == r: # 奇数轮次的最后一轮，例如7轮加密中的第7轮，没有取反的第8轮
            KeyScheduleSingle(m, i, RK)
        elif i % 2 == 0:
            KeySchedule(m, i, RK)

        # 轮函数约束
        for j in range(16):
            # H1
            RK_copy1 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="RK^" + str(i - 1) + "_" + str(j) + "_copy1")
            RK_copy2 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="RK^" + str(i - 1) + "_" + str(j) + "_copy2")
            L_copy1 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="L^" + str(i - 1) + "_" + str(j) + "_copy1")
            L_copy2 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="L^" + str(i - 1) + "_" + str(j) + "_copy2")
            L1(m, RK[j], u[j], a[j], [RK_copy1, RK_copy2, L_copy1, L_copy2])
            RK[j] = RK_copy1
            u[j] = L_copy1

        for j in range(16):
            # H2
            R_copy1 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="R^" + str(i - 1) + "_" + str(j) + "_copy1")
            R_copy2 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="R^" + str(i - 1) + "_" + str(j) + "_copy2")
            if con_i[j] == 1:
                L2(m, u[j + 16], constant_i_var[0], b[j], [R_copy1, R_copy2])
                del(constant_i_var[0])
            else:
                m.addConstr(u[j + 16] >= R_copy1)
                m.addConstr(u[j + 16] >= R_copy2)
                m.addConstr(u[j + 16] <= R_copy1 + R_copy2)
                m.addConstr(R_copy2 == b[j])
            u[j + 16] = R_copy1

        for j in range(16):
            # H3
            L2(m, a[j], b[j], c[j], [])

        for j in range(14):
            # H4
            c_1_copy1 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="c^" + str(i) + "_" + str(j) + "+1_copy1")
            c_1_copy2 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="c^" + str(i) + "_" + str(j) + "+1_copy2")
            c_2_copy1 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="c^" + str(i) + "_" + str(j) + "+2_copy1")
            c_2_copy2 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="c^" + str(i) + "_" + str(j) + "+2_copy2")
            L1(m, c[j + 1], c[j + 2], d[j], [c_1_copy1, c_1_copy2, c_2_copy1, c_2_copy2])
            c[j + 1] = c_1_copy1
            c[j + 2] = c_2_copy1

        for j in range(16):
            # H5
            if j < 14:
                L2(m, c[j], d[j], e[j], [])
            else:
                m.addConstr(e[j] == c[j])

        # H6
        for j in range(16):
            e_3_copy1 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="e^" + str(i) + "_" + str(j) + "+3_copy1")
            e_3_copy2 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="e^" + str(i) + "_" + str(j) + "+3_copy2")
            e_9_copy1 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="e^" + str(i) + "_" + str(j) + "+9_copy1")
            e_9_copy2 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="e^" + str(i) + "_" + str(j) + "+9_copy2")
            e_14_copy1 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="e^" + str(i) + "_" + str(j) + "+14_copy1")
            e_14_copy2 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="e^" + str(i) + "_" + str(j) + "+14_copy2")
            L3(m, e[(j + 3) % 16], e[(j + 9) % 16], e[(j + 14) % 16], f[j], [e_3_copy1, e_3_copy2, e_9_copy1, e_9_copy2, e_14_copy1, e_14_copy2])
            e[(j + 3) % 16] = e_3_copy1
            e[(j + 9) % 16] = e_9_copy1
            e[(j + 14) % 16] = e_14_copy1
        for j in range(16):
            m.addConstr(e[j] == 0)

        # H7
        for j in range(16):
            f_copy1 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="f^" + str(i) + "_" + str(j) + "_copy1")
            f_copy2 = m.addVar(lb=0.0, ub=1.0, vtype=GRB.BINARY, name="f^" + str(i) + "_" + str(j) + "_copy2")
            L1(m, f[j], RK[j], g[j], [f_copy1, f_copy2])
            f[j] = f_copy1

        for j in range(16):
        # H8
            L2(m, g[j], u[16 + j], L[j], [])

        for j in range(16):
        # H9
            L2(m, f[j], u[j], R[j], [])

        u = L + R

    KeyNotUsed(m, r)

    return m


def SearchMonomial(m, r, I):
    for i in range(32):
        if i < 16:
            u0 = m.getVarByName("L^0_" + str(i))
        else:
            u0 = m.getVarByName("R^0_" + str(i - 16))
        if i in I:
            # 公开变量(明文)中立方元素全为1
            m.addConstr(u0 == 1)
        else:
            # 非立方元素全为0
            m.addConstr(u0 == 0)

    # L^{r}[i] = g^{r}[i] ^ R^{r-1}[i]
    for i in range(16):
        L = m.getVarByName("L^" + str(r) + "_" + str(i))
        R = m.getVarByName("R^" + str(r) + "_" + str(i))
        if i == 0:
            m.addConstr(L == 1)
            m.addConstr(R == 0)
        else:
            m.addConstr(L == 0)
            m.addConstr(R == 0)

    m.setParam("PoolSearchMode", 2)
    m.setParam("PoolSolutions", 2000000000)

    return m


def SearchCube(m, r):
    # L^{r}[i] = g^{r}[i] ^ R^{r-1}[i]
    for i in range(16):
        L = m.getVarByName("L^" + str(r) + "_" + str(i))
        R = m.getVarByName("R^" + str(r) + "_" + str(i))
        if i == 0:
            m.addConstr(L == 0)
            m.addConstr(R == 1)
        else:
            m.addConstr(L == 0)
            m.addConstr(R == 0)

    # 设置目标函数为最大化立方元素求和
    e = 0
    for i in range(16):
        e = e + m.getVarByName("L^0_" + str(i))
        e = e + m.getVarByName("R^0_" + str(i))
    m.setObjective(e, GRB.MAXIMIZE)

    m.setParam("PoolSearchMode", 1)
    m.setParam("PoolSolutions", 256)

    return m


def main(r, I):
    m = GenerateJModel(r)
    m = SearchMonomial(m, r, I)
    # m = SearchCube(m, r)
    m.write("JCubeAttack.lp")
    m.optimize()

    # 输出超级多项式
    print(I)
    PrintSuperPoly(m)


    nSolutions = min(m.SolCount, 8)
    for solution in range(nSolutions):
        m.setParam(GRB.Param.SolutionNumber, solution)
        m.write("JCubeAttack_" + str(solution) + ".sol")



if __name__ == '__main__':
    I = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 17, 18]
    # I = [18]
    main(5, I)