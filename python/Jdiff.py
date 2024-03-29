# add constraints before and after &RK in one round &
# between 2 adjacent rounds with same subK &
# between 2 rounds with interval of 8
# at 2022/04/20
# by AngieJC
# htk90uggk@outlook.com

from ast import In
from operator import mod
from BasicTools import *
from ConstraintGenerator import *

andbox_diff=set([(0,0,0,0),(0,1,0,1),(0,1,1,1),(1,0,1,1),(1,0,0,1),(1,1,0,1),(1,1,1,1)])
andbox_diff_pattern=[(-1, 0, 0, 1, 0),\
(0, -1, 0, 1, 0),\
(1, 1, 0, -1, 0),\
(0, 0, -1, 1, 0)]

andbox_diff0=set([(0,0,0),(1,0,1),(1,1,1)])
andbox_diff0_pattern=[(0, -1, 1, 0),\
(1, 0, -1, 0),\
(-1, 0, 1, 0)]

xor_diff=set([(0,0,0),(0,1,1),(1,0,1),(1,1,0)])
xor_diff_pattern=[(-1, 1, 1, 0),\
(1, 1, -1, 0),\
(-1, -1, -1, 2),\
(1, -1, 1, 0)]

xor3_diff=set([(0, 0, 0, 0), (1, 0, 0, 1), (0, 1, 0, 1), (1, 1, 0, 0), (0, 0, 1, 1), (1, 0, 1, 0), (0, 1, 1, 0), (1, 1, 1, 1)])
xor3_diff_pattern=[(1, -1, 1, 1, 0),\
(-1, -1, 1, -1, 2),\
(-1, -1, -1, 1, 2),\
(-1, 1, -1, -1, 2),\
(-1, 1, 1, 1, 0),\
(1, -1, -1, -1, 2),\
(1, 1, 1, -1, 0),\
(1, 1, -1, 1, 0)]

andSameRK_diff = set([(1, 0, 0, 0),(1, 0, 1, 0),(1, 1, 1, 1),(1, 1, 0, 0),(0, 0, 1, 0),(0, 0, 1, 1),(0, 0, 0, 0)])
andSameRK_diff_pattern = [(1, -1, 0, 0, 0),\
(0, 0, 1, -1, 0),\
(0, 1, 0, 0, 0),\
(0, 0, 0, 1, 0),\
(-1, 0, 0, 0, 1),\
(0, -1, -1, 1, 1),\
(0, 0, -1, 0, 1),\
(-1, 1, 0, -1, 1)]

andDiffRK_diff = set([(1, 0, 1, 1),(1, 0, 0, 0),(1, 1, 0, 0),(1, 1, 1, 0),(0, 0, 1, 1),(0, 0, 1, 0),(0, 0, 0, 0)])
andInveRK_diff_pattern = [(0, 0, 0, 1, 0),\
(1, -1, 0, 0, 0),\
(0, 1, 0, 0, 0),\
(0, 0, 1, -1, 0),\
(0, -1, 0, -1, 1),\
(-1, 0, 0, 0, 1),\
(-1, 1, -1, 1, 1),\
(0, 0, -1, 0, 1)]

adjacentAnd_diff = set([(0, 0, 0, 0, 0), (0, 0, 1, 0, 0), (0, 0, 1, 0, 1), (0, 1, 0, 0, 0), (0, 1, 0, 0, 1), (0, 1, 0, 1, 0), (0, 1, 0, 1, 1), (0, 1, 1, 0, 0), (0, 1, 1, 0, 1), (0, 1, 1, 1, 0), (0, 1, 1, 1, 1), (1, 0, 0, 0, 0), (1, 0, 0, 1, 0), (1, 0, 1, 0, 0), (1, 0, 1, 1, 1), (1, 1, 0, 0, 0), (1, 1, 0, 0, 1), (1, 1, 0, 1, 0), (1, 1, 0, 1, 1), (1, 1, 1, 0, 0), (1, 1, 1, 0, 1), (1, 1, 1, 1, 0), (1, 1, 1, 1, 1)])
adjacentAnd_diff_pattern = [(-1, 0, 0, 0, 0, 1),\
(0, -1, 0, 0, 0, 1),\
(0, 0, -1, 0, 0, 1),\
(0, 0, 0, -1, 0, 1),\
(0, 0, 0, 0, -1, 1),\
(0, 1, -1, -1, 1, 1),\
(0, 0, 0, 0, 1, 0),\
(0, 0, 1, 0, 0, 0),\
(0, 1, 0, 0, 0, 0),\
(0, 0, 0, 1, 0, 0),\
(1, 1, 0, -1, 0, 0),\
(-1, 1, 0, 1, -1, 1),\
(0, 1, 1, 0, -1, 0),\
(1, 0, 0, 0, 0, 0)]

adjacentAndandXor_diff = set()
adjacentAndandXor_diff_pattern = []

class Jdiff:
    def genVars_Round(r):
        return ['L_'+str(r)+'r_'+str(i) for i in range(16)]+['R_'+str(r)+'r_'+str(i) for i in range(16)]

    def genVars_aftRk1_Round(r):
        return ['aftRK1_'+str(r)+'r_'+str(i) for i in range(16)]

    def genVars_bfS_Round(r):
        return ['bfS'+str(r)+'r_'+str(i) for i in range(16)]

    def genVars_inS_Round(r):
        return ['inS'+str(r)+'r_'+str(i) for i in range(14)]
    
    def genVars_aftS_Round(r):
        return ['aftS'+str(r)+'r_'+str(i) for i in range(16)]
    

    def genVars_aftP_Round(r):
        return ['aftP'+str(r)+'r_'+str(i) for i in range(16)]

    def genVars_aftRk2_Round(r):
        return ['aftRK2_'+str(r)+'r_'+str(i) for i in range(16)]

    def genVars_Act_Rk1_Round(r):
        return ['ActRK1_'+str(r)+'r_'+str(i) for i in range(16)]
    def genVars_Act_Rk2_Round(r):
        return ['ActRK2_'+str(r)+'r_'+str(i) for i in range(16)]
    def genVars_Act_S_Round(r):
        return ['ActS_'+str(r)+'r_'+str(i) for i in range(14)]
    
    def get_bit_val(byte, index):
        if (byte[0] & (1 << index).to_bytes(2, byteorder='big')[0]) + (byte[1] & (1 << index).to_bytes(2, byteorder='big')[1]):
            return 1
        else:
            return 0

    def genConstraints_of_Round(r):
        assert r>=1
        constraints = list()

        Lin = Jdiff.genVars_Round(r-1)[0:16]
        Rin = Jdiff.genVars_Round(r-1)[16:32]
        aftRk1 = Jdiff.genVars_aftRk1_Round(r-1)
        Act_Rk1 = Jdiff.genVars_Act_Rk1_Round(r-1)
        Act_Rk2 = Jdiff.genVars_Act_Rk2_Round(r-1)
        Act_S = Jdiff.genVars_Act_S_Round(r-1)
        bfS = Jdiff.genVars_bfS_Round(r-1)
        inS = Jdiff.genVars_inS_Round(r-1)
        aftS = Jdiff.genVars_aftS_Round(r-1)
        rot3 =  BasicTools.leftCyclicRotation(aftS, 3)
        rot9 =  BasicTools.leftCyclicRotation(aftS,9)
        # rot14 = BasicTools.leftCyclicRotation(aftS, 3)
        rot14 = BasicTools.leftCyclicRotation(aftS, 14)
        aftP = Jdiff.genVars_aftP_Round(r-1)
        aftRk2 = Jdiff.genVars_aftRk2_Round(r-1)
        Lout = Jdiff.genVars_Round(r)[0:16]
        Rout = Jdiff.genVars_Round(r)[16:32]


        for i in range(16):
            constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([Lin[i],aftRk1[i],Act_Rk1[i]],andbox_diff0_pattern)

        
        for i in range(16):
            constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([aftRk1[i],Rin[i],bfS[i]],xor_diff_pattern)

        
        for i in range(14):
            constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([bfS[i+2],bfS[i+1],inS[i],Act_S[i]],andbox_diff_pattern)

        for i in range(14):
            constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([bfS[i],inS[i],aftS[i]],xor_diff_pattern)
            
        constraints = constraints + ConstraintGenerator.genEqualityConstraints(bfS[14:16],aftS[14:16])

        

        for i in range(16):
            constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([rot3[i],rot9[i],rot14[i],aftP[i]],xor3_diff_pattern)

        
        for i in range(16):
            constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([aftP[i],aftRk2[i],Act_Rk2[i]],andbox_diff0_pattern)

        
        for i in range(16):
            constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([Lin[i],aftP[i],Rout[i]],xor_diff_pattern)
        for i in range(16):
            constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([aftRk2[i],Rin[i],Lout[i]],xor_diff_pattern)

        # constraints in one round
        for i in range(16):
            constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([Lin[i], aftRk1[i], aftP[i], aftRk2[i]], andSameRK_diff_pattern)
        
        # constraints between 2 adjacent rounds
        if r % 2 == 0:
            Lin_i_1 = Jdiff.genVars_Round(r - 2)[0:16]
            aftRK1_i_1 = Jdiff.genVars_aftRk1_Round(r - 2)
            aftP_i_1 = Jdiff.genVars_aftP_Round(r - 2)
            aftRk2_i_1 = Jdiff.genVars_aftRk2_Round(r - 2)
            for i in range(16):
                constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([Lin_i_1[i], aftRK1_i_1[i], Lin[i], aftRk1[i]], andInveRK_diff_pattern)
                constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([Lin_i_1[i], aftRK1_i_1[i], aftP[i], aftRk2[i]], andInveRK_diff_pattern)
                constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([aftP_i_1[i], aftRk2_i_1[i], Lin[i], aftRk1[i]], andInveRK_diff_pattern)
                constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([aftP_i_1[i], aftRk2_i_1[i], aftP[i], aftRk2[i]], andInveRK_diff_pattern)
        
        # constraints between 2 rounds with interval of 8
        if r > 8:
            Lin_i_8 = Jdiff.genVars_Round(r - 9)[0:16]
            aftRK1_i_8 = Jdiff.genVars_aftRk1_Round(r - 9)
            aftP_i_8 = Jdiff.genVars_aftP_Round(r - 9)
            aftRK2_i_8 = Jdiff.genVars_aftRk2_Round(r - 9)
            b_i = int((r - 1) / 2 - 4).to_bytes(2, byteorder='big')
            for i in range(16):
                if Jdiff.get_bit_val(b_i, i):
                    constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([Lin_i_8[i], aftRK1_i_8[i], Lin[i], aftRk1[i]], andSameRK_diff_pattern)
                    constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([Lin_i_8[i], aftRK1_i_8[i], aftP[i], aftRk2[i]], andSameRK_diff_pattern)
                    constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([aftP_i_8[i], aftRK2_i_8[i], Lin[i], aftRk1[i]], andSameRK_diff_pattern)
                    constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([aftP_i_8[i], aftRK2_i_8[i], aftP[i], aftRk2[i]], andSameRK_diff_pattern)
                else:
                    constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([Lin_i_8[i], aftRK1_i_8[i], Lin[i], aftRk1[i]], andInveRK_diff_pattern)
                    constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([Lin_i_8[i], aftRK1_i_8[i], aftP[i], aftRk2[i]], andInveRK_diff_pattern)
                    constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([aftP_i_8[i], aftRK2_i_8[i], Lin[i], aftRk1[i]], andInveRK_diff_pattern)
                    constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([aftP_i_8[i], aftRK2_i_8[i], aftP[i], aftRk2[i]], andInveRK_diff_pattern)

        # two adjacent And box will constrain 5 bits
        # a     b     c
        # |__&__|__&__|
        #    |     |
        #    α     β
        for i in range(13):
            constraints = constraints + ConstraintGenerator.genFromConstraintTemplate([bfS[i], bfS[i + 1], bfS[i + 2], inS[i], inS[i + 1]], adjacentAnd_diff_pattern)

        return constraints

    def genObjectiveFun_to_Round(r):
        assert r>=1
        f = list([])
        
        for i in range(1,r+1):
            Act_Rk1 = Jdiff.genVars_Act_Rk1_Round(i-1)
            Act_Rk2 = Jdiff.genVars_Act_Rk2_Round(i-1)
            Act_S = Jdiff.genVars_Act_S_Round(i-1)
            f = f + Act_Rk1 + Act_Rk2 + Act_S

        f = ' + '.join(f)
        return f

    def genModel(r):
        V = set([])
        C = list([])

        #initialConstraint = ' + '.join(Jdiff.genVars_Round(0)) + ' >= 1'
        #C.append(initialConstraint)
        initialConstraint = ConstraintGenerator.genConsConstraints(Jdiff.genVars_Round(0), "01000101011001110000000100100011")
        C = C + initialConstraint

        for i in range(1,r+1):
            C = C + Jdiff.genConstraints_of_Round(i)


        myfile = open('Jdiff_' + str(r)+'r.lp','w')
        print('Minimize',file= myfile)
        ActiveVar = Jdiff.genObjectiveFun_to_Round(r)
        print(ActiveVar, file = myfile)

        print('\n', file = myfile)
        print('Subject To', file = myfile)

        for c in C:
            print(c, file = myfile)

        print('\n', file = myfile)
        print('Binary', file = myfile)
        V = BasicTools.getVariables_From_Constraints(C)
        for v in V:
            print(v, file = myfile)

def main():
    Jdiff.genModel(3)

if __name__=='__main__':
    main()
