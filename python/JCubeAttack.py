from gurobipy import *

def JEval( r ):
    m = Model("JSuperPoly")
    for i in range(48):
        m.addVar(lb=0.0, ub=1.0, obj=0.0, vtype=GRB.BINARY, name="s^0_" + str(i))
    for i in range(16):
        si = m.getVarByName("s^0_" + str(i))
        li = m.addVar(lb=0.0, ub=1.0, obj=0.0, vtype=GRB.BINARY, name="l^0_" + str(i))
        m.addConstr(si == li)
        si = m.getVarByName("s^0_" + str(i + 16))
        ri = m.addVar(lb=0.0, ub=1.0, obj=0.0, vtype=GRB.BINARY, name="r^0_" + str(i))
        m.addConstr(si == ri)
    for i in range(64):
        m.addVar(lb=0.0, ub=1.0, obj=0.0, vtype=GRB.BINARY, name="k^0_" + str(i))
    
    return m

def main():
    m = JEval(1)
    m.write("angiejc.lp")

if __name__ == '__main__':
    main()