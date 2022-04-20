from itertools import permutations
class BasicTools:
    @staticmethod
    def generalizedPermutation(in_vars, permTable):
        """
        Example:
            >>> BasicTools.generalizedPermutation(['x0','x1','x2'], [2,2,0,0,1,1])
            ['x2', 'x2', 'x0', 'x0', 'x1', 'x1']
            >>>
        """
        out = [None for i in range(0, len(permTable))]
        for j in range(0, len(permTable)):
            assert permTable[j] in range(0, len(in_vars)), "index value is not compatible with in variables!"
            out[j] = in_vars[permTable[j]]
        return out

    @staticmethod
    def leftRotation(in_vars, offset):
        assert offset < len(in_vars), "Are you rotating too much?"
        out = [None for i in range(0, len(in_vars))]
        if offset == 0:
            out[0:] = in_vars[0:]
        else:
            out[0:-offset] = in_vars[offset:]
        return out

    @staticmethod
    def rightRotation(in_vars, offset):
        assert offset < len(in_vars), "Are you rotating too much?"
        out = [None for i in range(0, len(in_vars))]
        if offset == 0:
            out[0:] = in_vars[0:]
        else:
            out[offset:] = in_vars[0:-offset]
        return out

    @staticmethod
    def leftCyclicRotation(in_vars, offset):
        out = [None for i in range(0, len(in_vars))]
        for i in range(0, len(in_vars)):
            out[i] = in_vars[(i+offset) % len(in_vars)]

        return out

    @staticmethod
    def rightCyclicRotation(in_vars, offset):
        out = [None for i in range(0, len(in_vars))]
        for i in range(0, len(in_vars)):
            out[i] = in_vars[(i-offset) % len(in_vars)]

        return out


    @staticmethod
    def dotProduct(x, y, size):
        """
        comput the dot product of to binary strings:
        dotProduct(0101, 1100, 4) = 0*1 + 1*1 + 0*0 + 1*0 = 1

        Example:
            >>> bin(15)
            '0b1111'
            >>> bin(11)
            '0b1011'
            >>> BasicTools.dotProduct(15, 11, 4)
            1
        """
        a = bin(x)[2:].zfill(size)
        b = bin(y)[2:].zfill(size)
        t = [int(a[i])&int(b[i]) for i in range(0, size)]
        return reduce((lambda x,y: x^y), t)



    @staticmethod
    def wordToBinaryString(word, size):
        """
        Example:
            >>> BasicTools.wordToBinaryString(0xF1, 8)
            '11110001'
        """
        return bin(word)[2:].zfill(size)

    @staticmethod
    def allVectorsOfDim(d):
        """
        compute the set of all vectors in space {0,1}^d
        Example:
            >>> BasicTools.allVectorsOfDim(3)
            {(0, 0, 0),
             (0, 0, 1),
             (0, 1, 0),
             (0, 1, 1),
             (1, 0, 0),
             (1, 0, 1),
             (1, 1, 0),
             (1, 1, 1)}
        """
        out = set({})
        for i in range(0, 2**d):
            s = bin(i)[2:].zfill(d)
            v = tuple([int(s[j]) for j in range(0, len(s))])
            out.add(v)

        return out

    @staticmethod
    def getVariables_From_Constraints(C):
        V = set([])
        for s in C:
            temp = s.strip()
            temp = temp.replace('+', ' ')
            temp = temp.replace('-', ' ')
            temp = temp.replace('>=', ' ')
            temp = temp.replace('<=', ' ')
            temp = temp.replace('=', ' ')
            temp = temp.split()
            for v in temp:
                if not v.isdecimal():
                    V.add(v)

        return V
    
    @staticmethod
    def var_dict(var_list,value_list):
        assert len(var_list)==len(value_list)
        d={}
        for i in range(len(var_list)):
            d[var_list[i]]=value_list[i]

        return d
    def e_ij_is_feasible(d):
        basic_pattern=set([(0,0,0),(1,0,1),(0,1,1),(1,1,1)])
        l=[0,1,2,3]
        p=list(permutations(l,3))
        for t in range(len(p)):  #for all combinations of [e_ij,e_ik,e_jk]
            i=p[t][0]
            j=p[t][1]
            k=p[t][2]
            ij=str(min(i,j))+str(max(i,j))
            ik=str(min(i,k))+str(max(i,k))
            jk=str(min(j,k))+str(max(j,k))
            var_list=['e_'+ij , 'e_'+ik,'e_'+jk]
            v=tuple([d[var_list[0]],d[var_list[1]],d[var_list[2]]])
            #if not (v in basic_pattern): #some combination violetes the basic pattern
            #    print(var_list)
            #    print(v)
            if v==tuple([0,0,1]): #some combination meets a conflict
                return False
        return True

    
    def e_ij_feasible_patterns():
        '''
        There are 15 feasible patterns for e_ij:
        BasicTools.e_ij_feasible_patterns()
        {(1, 1, 1, 0, 0, 0), (0, 1, 1, 1, 1, 0), (1, 0, 0, 1, 1, 0), (1, 1, 1, 1, 0, 1),
         (1, 1, 0, 0, 1, 1), (1, 1, 1, 1, 1, 0), (1, 1, 1, 0, 1, 1), (0, 1, 0, 1, 0, 1),
         (1, 1, 0, 1, 1, 1), (0, 0, 0, 0, 0, 0), (1, 0, 1, 1, 1, 1), (1, 0, 1, 1, 0, 1),
         (1, 1, 1, 1, 1, 1), (0, 1, 1, 1, 1, 1), (0, 0, 1, 0, 1, 1)}
        '''
        var_list=['e_01', 'e_02', 'e_03', 'e_12', 'e_13', 'e_23']
        allPoints=list(BasicTools.allVectorsOfDim(6))
        feasible_patterns=set()
        for p in allPoints:
            d=BasicTools.var_dict(var_list,p)
            if BasicTools.e_ij_is_feasible(d):
                feasible_patterns.add(p)
        return feasible_patterns
    def e_SB_ij_MC_e_ij_A_patterns():        
        feasible_patterns=set()
        feasible_patterns.add(tuple([0,0,0,0,0,0,0,0,0,0]))#four input 0's -> four output 0's, AH=0, AL=0

        all4dimPoints=list(BasicTools.allVectorsOfDim(4))
        for p in all4dimPoints:
            if BasicTools.count1s(p)>=1:
                feasible_patterns.add(p+tuple([1,1,1,1,0,0]))#non-allzero input -> four output 1's, AH=0, AL=0

        all8dimPoints=list(BasicTools.allVectorsOfDim(8))
        for p in all8dimPoints:
            if BasicTools.count1s(p)<5:
                continue
            assert BasicTools.count1s(p[4:8])>=1 and BasicTools.count1s(p[0:4])>=1
            if (4-BasicTools.count1s(p[4:8]))==1:# one 0
                feasible_patterns.add(p+tuple([0,1]))
            elif (4-BasicTools.count1s(p[4:8]))==2: # two 0's
                feasible_patterns.add(p+tuple([1,0]))
            elif (4-BasicTools.count1s(p[4:8]))==3: # three 0's
                feasible_patterns.add(p+tuple([1,1]))
            else:
                assert BasicTools.count1s(p[4:8])==4
                feasible_patterns.add(p+tuple([0,0])) #actually already added in the feasible pattern
            
        return feasible_patterns
    def e_SB_ij_MC_e_ij_patterns():        
        feasible_patterns=set()
        feasible_patterns.add(tuple([0,0,0,0,0,0,0,0]))#four input 0's -> four output 0's

        all4dimPoints=list(BasicTools.allVectorsOfDim(4))
        
        all8dimPoints=list(BasicTools.allVectorsOfDim(8))
        for p in all8dimPoints:
            if BasicTools.count1s(p)>=5:
                
                assert BasicTools.count1s(p[4:8])>=1 and BasicTools.count1s(p[0:4])>=1
            
                feasible_patterns.add(p) 
            
        return feasible_patterns

    def e_SB_ij_MC_e_ij_A_ij_patterns():
        #four input e_ij, four output e_ij, four active A_ij corresponding to four output e_ij
        feasible_patterns=set()
        feasible_patterns.add(tuple([0,0,0,0,0,0,0,0,0,0,0,0]))#four input 0's -> four output 0's, A_0=0,..., A_3=0

        all4dimPoints=list(BasicTools.allVectorsOfDim(4))
        for p in all4dimPoints:
            if BasicTools.count1s(p)>=1:
                feasible_patterns.add(p+tuple([1,1,1,1,0,0,0,0]))#non-allzero input -> four output 1's, A_0=0,..., A_3=0

        all8dimPoints=list(BasicTools.allVectorsOfDim(8))
        for p in all8dimPoints:
            if BasicTools.count1s(p)<5:
                continue
            assert BasicTools.count1s(p[4:8])>=1 and BasicTools.count1s(p[0:4])>=1
            
            A_list=[0,0,0,0]
            for k in range(4):
                if p[4+k]==0:
                    A_list[k]=1
            feasible_patterns.add(p+tuple(A_list))
            
            
        return feasible_patterns

    def A_ij_realAH_realAL_ij_patterns():
        #for each byte, from A_01,A_02,A_03,A_12,A_13,A_23 deduce realAH, realAL. Eliminate the repeated probability calculation.
        feasible_patterns=set()
        all6dimPoints=list(BasicTools.allVectorsOfDim(6))
        for p in all6dimPoints:
            if BasicTools.count1s(p)==0:
                feasible_patterns.add(p+tuple([0,0]))
            elif BasicTools.count1s(p)==1:
                feasible_patterns.add(p+tuple([0,1]))
            elif BasicTools.count1s(p)==2:
                feasible_patterns.add(p+tuple([1,0]))
            elif BasicTools.count1s(p)==3:
                if (p in set([tuple([1,1,0,1,0,0]),tuple([1,0,1,0,1,0]),tuple([0,1,1,0,0,1]),tuple([0,0,0,1,1,1])])):#only three x value involved
                    feasible_patterns.add(p+tuple([1,0]))
                else:
                    feasible_patterns.add(p+tuple([1,1]))
            else:
                feasible_patterns.add(p+tuple([1,1]))
        return feasible_patterns

    
    def deA_realAHAL_finalAHAL_patterns():
        #constraints on [deA_01, deA_02, deA_03, deA_12, deA_13, deA_23, realAH, realAL, finalAH, finalAL]
        feasible_patterns=set()
        deA=BasicTools.e_ij_feasible_patterns()
        
        for p in deA:
            assert BasicTools.count0s(p) in set([0,1,2,3,6])
            if BasicTools.count0s(p)==0:#x0,x1,x2,x3 is not equalt to each other, [finalAH, finalAL]=[realAH, realAL]
                feasible_patterns.add(p+tuple([0,0,0,0]))
                feasible_patterns.add(p+tuple([0,1,0,1]))
                feasible_patterns.add(p+tuple([1,0,1,0]))
                feasible_patterns.add(p+tuple([1,1,1,1]))
            elif BasicTools.count0s(p)==6:#x0=x1=x2=x3
                feasible_patterns.add(p+tuple([0,0,0,0]))
            elif BasicTools.count0s(p)==3 or BasicTools.count0s(p)==2:
                feasible_patterns.add(p+tuple([0,0,0,0]))
                feasible_patterns.add(p+tuple([0,1,0,1]))
                feasible_patterns.add(p+tuple([1,0,0,1]))
                feasible_patterns.add(p+tuple([1,1,0,1]))
            elif BasicTools.count0s(p)==1:
                feasible_patterns.add(p+tuple([0,0,0,0]))
                feasible_patterns.add(p+tuple([0,1,0,1]))
                feasible_patterns.add(p+tuple([1,0,0,1]))#
                feasible_patterns.add(p+tuple([1,0,1,0]))#preserve two cases, discuss specially later
                feasible_patterns.add(p+tuple([1,1,1,0]))
        return feasible_patterns

    def dummy0_filter_onlyone0_in_deA():
        feasible_patterns=set()
        deA=BasicTools.e_ij_feasible_patterns()
        for p in deA:
            if BasicTools.count0s(p)==1:
                feasible_patterns.add(p+tuple([0]))
            else:
                feasible_patterns.add(p+tuple([1]))
        return feasible_patterns
    def dummy1_filter_relatedtwo1_in_A_ij():
        feasible_patterns=set()
        A_ij=list(BasicTools.allVectorsOfDim(6))
        required_A_ij=set([tuple([1,1,0,0,0,0]),\
                           tuple([1,0,1,0,0,0]),\
                           tuple([0,1,1,0,0,0]),\
                           tuple([1,0,0,1,0,0]),\
                           tuple([1,0,0,0,1,0]),\
                           tuple([0,1,0,1,0,0]),\
                           tuple([0,1,0,0,0,1]),\
                           tuple([0,0,1,0,0,1]),\
                           tuple([0,0,1,0,1,0]),\
                           tuple([0,0,0,1,1,0]),\
                           tuple([0,0,0,1,0,1]),\
                           tuple([0,0,0,0,1,1])])
        for p in A_ij:
            if p in required_A_ij:
                feasible_patterns.add(p+tuple([0]))
            else:
                feasible_patterns.add(p+tuple([1]))
        return feasible_patterns
    
    def dummy0_dummy1_deA_A_finalAHAL():
    #constraints on [dummy0,dummy1,deA_ij,A_ki,A_kj,finalAH,finalAL]
        feasible_patterns=set()
        all5dimPoints=list(BasicTools.allVectorsOfDim(5))
        for p in all5dimPoints:
            if p==tuple([0,0,0,1,1]):
                feasible_patterns.add(p+tuple([0,1]))
            else:
                feasible_patterns.add(p+tuple([0,0]))
                feasible_patterns.add(p+tuple([0,1]))
                feasible_patterns.add(p+tuple([1,0]))
                feasible_patterns.add(p+tuple([1,1]))
        return feasible_patterns
    def count1s(t):
        r=0
        for i in range(len(t)):
            if t[i]==1:
                r=r+1
        return r
    def count0s(t):
        r=0
        for i in range(len(t)):
            if t[i]==0:
                r=r+1
        return r

    def e_ij_Delta_A_patterns():
        #if there are two 0's in e_ij pattern, Delta=0; for all 0's and all 1's, discuss specially
        res=set()
        e_ij_feasible_patterns=BasicTools.e_ij_feasible_patterns()
        assert len(e_ij_feasible_patterns)==15
        
        for p in e_ij_feasible_patterns:
            assert len(p)==6
            if BasicTools.count0s(p)==2:
                res.add(p+tuple([0,0])) #two 0's, Delta=0, A=0
            elif BasicTools.count0s(p)==6: #all 0's
                res.add(p+tuple([0,0])) 
            elif BasicTools.count0s(p)==0: #all 1's
                res.add(p+tuple([0,1])) #for Delta=0, one active byte is consumed
                res.add(p+tuple([1,0])) #for Delta=1, do not consider the (1-2^{-8}) probability
            else:
                assert BasicTools.count0s(p)==3 or BasicTools.count0s(p)==1
                res.add(p+tuple([1,0]))                    
        return res
    def e_ij_dummy_Delta_Ad_patterns():
        #constraints on [e_ij,d,Delta,A]. 
        #if there are two 0's in e_ij pattern, Delta=0; for all 0's and all 1's, discuss specially
        res=set()
        e_ij_feasible_patterns=BasicTools.e_ij_feasible_patterns()
        assert len(e_ij_feasible_patterns)==15
        
        for p in e_ij_feasible_patterns:
            assert len(p)==6
            if BasicTools.count0s(p)==2:
                res.add(p+tuple([0,0,0])) #d=0 or 1, Delta=0, A=0
                res.add(p+tuple([1,0,0]))
            elif BasicTools.count0s(p)==6: #all 0's
                res.add(p+tuple([0,0,0])) #d=0 or 1
                res.add(p+tuple([1,0,0]))
            elif BasicTools.count0s(p)==0: #all 1's
                res.add(p+tuple([1,0,1])) #for d=1,Delta=0, one active byte is consumed
                res.add(p+tuple([0,0,0]))#for d=0,Delta=0, Delta is preserved by MC
                res.add(p+tuple([1,1,0])) #for Delta=1, d must be 1, and do not consider the (1-2^{-8}) probability
            else:
                assert BasicTools.count0s(p)==3 or BasicTools.count0s(p)==1
                res.add(p+tuple([1,1,0]))#for Delta=1, d must be 1
        return res
    def e_01_23_02_feasible():
        var_list=['e_01', 'e_02', 'e_03', 'e_12', 'e_13', 'e_23']
        all_eij_peasible=BasicTools.e_ij_feasible_patterns()
        res=set()
        for p in all_eij_peasible:
            res.add(tuple([p[0],p[-1],p[1]]))
        return res

    def deA_A_finalAHAL_patterns():
        deA_Ind=BasicTools.e_ij_feasible_patterns()
        A_Ind=BasicTools.allVectorsOfDim(6)
        feasible_patterns=set()
        rel_ind=[set([0,1]),set([0,2]),set([0,3]),set([1,2]),set([1,3]),set([2,3])]
        for deA in deA_Ind:
            assert BasicTools.count1s(deA) in [0,3,4,5,6]
            if BasicTools.count1s(deA)==0:
                feasible_patterns.add(deA+tuple([0,0,0,0,0,0,0,0]))
            elif BasicTools.count1s(deA)==3: #x_i   x_j,x_k,x_t
                for A in A_Ind:
                    if BasicTools.count1s(A)==0:
                        feasible_patterns.add(deA+A+tuple([0,0]))
                    elif BasicTools.count1s(A) in [1,2,3]:
                        feasible_patterns.add(deA+A+tuple([0,1]))
            elif BasicTools.count1s(deA)==4: #x_i,x_j   x_k,x_t
                for A in A_Ind:
                    if BasicTools.count1s(A)==0:
                        feasible_patterns.add(deA+A+tuple([0,0]))
                    elif BasicTools.count1s(A) in [1,2,3,4]:
                        feasible_patterns.add(deA+A+tuple([0,1]))
            elif BasicTools.count1s(deA)==5: #x_i,x_j   x_k   x_t
                for A in A_Ind:
                    if BasicTools.count1s(A)==0:
                        feasible_patterns.add(deA+A+tuple([0,0]))
                    elif BasicTools.count1s(A)==1:
                        feasible_patterns.add(deA+A+tuple([0,1]))
                    elif BasicTools.count1s(A)==2:
                        rel=set()
                        merg=set()
                        for i in range(6):
                            if deA[i]==0:
                                merg=merg | rel_ind[i]
                            if A[i]==1:
                                rel=rel | rel_ind[i]
                        assert len(merg)==2
                        if merg in rel:
                            feasible_patterns.add(deA+A+tuple([0,1]))
                        else:
                            feasible_patterns.add(deA+A+tuple([1,0]))
                    elif BasicTools.count1s(A) in [3,4,5]:
                        feasible_patterns.add(deA+A+tuple([1,0]))
            elif BasicTools.count1s(deA)==6: # different four value
                for A in A_Ind:
                    if BasicTools.count1s(A)==0:
                        feasible_patterns.add(deA+A+tuple([0,0]))
                    elif BasicTools.count1s(A)==1:
                        feasible_patterns.add(deA+A+tuple([0,1]))
                    elif BasicTools.count1s(A)==2:
                        feasible_patterns.add(deA+A+tuple([1,0]))
                    elif BasicTools.count1s(A)==3:
                        rel=set()
                        for i in range(6):
                            if A[i]==1:
                                rel=rel | rel_ind[i]
                        assert len(rel) in [3,4]
                        if len(rel)==4:
                            feasible_patterns.add(deA+A+tuple([1,1]))
                        else:
                            feasible_patterns.add(deA+A+tuple([1,0]))
                    elif BasicTools.count1s(A) in [4,5,6]:
                        feasible_patterns.add(deA+A+tuple([1,1]))

        return feasible_patterns

    def l_r_o_fAH_fAL_patterns():
        '''
        mixture quadruples (aaaa,aacc,abab,abba) and (abcd) are encoded by 3 bits:
        aaaa  000
        aacc  001
        abab  010
        abba  100
        abcd  101
        Give out feasible patterns for XOR operation related variables:[left,right,output,fAH,fAL]
        Totally 5*5*5=125 cases
        '''
        feasible_patterns=set()
        PatternCode=[[0,0,0],[0,0,1],[0,1,0],[1,0,0],[1,0,1]]
        PatternStrL =[['a_l','a_l','a_l','a_l'],\
                      ['a_l','a_l','c_l','c_l'],\
                      ['a_l','b_l','a_l','b_l'],\
                      ['a_l','b_l','b_l','a_l'],\
                      ['a_l','b_l','c_l','d_l']]
        PatternStrR =[['a_r','a_r','a_r','a_r'],\
                      ['a_r','a_r','c_r','c_r'],\
                      ['a_r','b_r','a_r','b_r'],\
                      ['a_r','b_r','b_r','a_r'],\
                      ['a_r','b_r','c_r','d_r']]
        PatternStrO =[['a_o','a_o','a_o','a_o'],\
                      ['a_o','a_o','c_o','c_o'],\
                      ['a_o','b_o','a_o','b_o'],\
                      ['a_o','b_o','b_o','a_o'],\
                      ['a_o','b_o','c_o','d_o']]
        for l in range(len(PatternCode)):
            leftPatternCode=PatternCode[l]
            for r in range(len(PatternCode)):                
                rightPatternCode=PatternCode[r]
                for o in range(len(PatternCode)):
                    outPatternCode=PatternCode[o]
                    numActive=0
                    zeroSum=list()
                    merged=set()
                    
                    feasible= True
                    for i,j in [[0,1],[0,2],[0,3],[1,2],[1,3],[2,3]]:
                        realXORstr_i = [PatternStrL[l][i], PatternStrR[r][i]]
                        realXORstr_j = [PatternStrL[l][j], PatternStrR[r][j]]
                        if (realXORstr_i==realXORstr_j) and (PatternStrO[o][i]!=PatternStrO[o][j]):
                            feasible = False #实际字符串相等，想要字符串不等，不可能的，扔掉;排除掉一种分支数为1的情况
                            break 
                        if (realXORstr_i==realXORstr_j) and (PatternStrO[o][i]==PatternStrO[o][j]):
                            merged = merged | {i,j}
                            continue #实际字符串相等，想要字符串相等，不用管
                        if (realXORstr_i!=realXORstr_j) and (PatternStrO[o][i]!=PatternStrO[o][j]):
                            continue #实际字符串不等，想要字符串不等，不用管
                        if (realXORstr_i!=realXORstr_j) and (PatternStrO[o][i]==PatternStrO[o][j]):
                            #实际字符串不等，想要字符串相等
                            if (realXORstr_i[0]==realXORstr_j[0]) or (realXORstr_i[1]==realXORstr_j[1]):
                                feasible = False
                                break #只有一个分支值不同是不能得到相同输出值的；排除掉两种分支数为1的情况
                            else:
                                #两个分支上的值都不等
                                assert (realXORstr_i[0]!=realXORstr_j[0]) and (realXORstr_i[1]!=realXORstr_j[1])
                                
                                related={i,j}
                                currentZeroSum = set(realXORstr_i + realXORstr_j) #这四个值相加为0
                                
                                if not (currentZeroSum in zeroSum):
                                    zeroSum.append(currentZeroSum)
                                    
                                    if related.issubset(merged):
                                        continue
                                    else:
                                        
                                        numActive =  numActive + 1
                                merged = merged | related
                                    
                    assert numActive <= 3
                    
                    if feasible:
                        currentFeasiblePattern = leftPatternCode + rightPatternCode +outPatternCode + \
                                                 [int(bin(numActive)[2:].zfill(2)[0])] + [int(bin(numActive)[2:].zfill(2)[1])]
                        feasible_patterns.add(tuple(currentFeasiblePattern))

        return feasible_patterns

    def TwobitCodes_l_r_o_fAH_fAL_patterns():
        '''
        exchanging mixture quadruples (aaaa,abab,abba) and (abcd) are encoded by 2bits:
        aaaa 00
        abab 01
        abba 10
        abcd 11
        Give out feasible patterns for XOR operation related variables:[left,right,output,fAH,fAL]
        Totally 4*4*4=64 cases
        '''
        feasible_patterns = set()
        PatternCode = [[0,0],[0,1],[1,0],[1,1]]
        PatternStrL = [['a_l', 'a_l', 'a_l', 'a_l'],\
                       ['a_l', 'b_l', 'a_l', 'b_l'],\
                       ['a_l', 'b_l', 'b_l', 'a_l'],\
                       ['a_l', 'b_l', 'c_l', 'd_l']]
    
        PatternStrR =[['a_r','a_r','a_r','a_r'],\
                      ['a_r','b_r','a_r','b_r'],\
                      ['a_r','b_r','b_r','a_r'],\
                      ['a_r','b_r','c_r','d_r']]
        PatternStrO =[['a_o','a_o','a_o','a_o'],\
                      ['a_o','b_o','a_o','b_o'],\
                      ['a_o','b_o','b_o','a_o'],\
                      ['a_o','b_o','c_o','d_o']]
        for l in range(len(PatternCode)):
            leftPatternCode=PatternCode[l]
            for r in range(len(PatternCode)):                
                rightPatternCode=PatternCode[r]
                for o in range(len(PatternCode)):
                    outPatternCode=PatternCode[o]
                    numActive=0
                    zeroSum=list()
                    merged=set()
                    
                    feasible= True
                    for i,j in [[0,1],[0,2],[0,3],[1,2],[1,3],[2,3]]:
                        realXORstr_i = [PatternStrL[l][i], PatternStrR[r][i]]
                        realXORstr_j = [PatternStrL[l][j], PatternStrR[r][j]]
                        if (realXORstr_i==realXORstr_j) and (PatternStrO[o][i]!=PatternStrO[o][j]):
                            feasible = False #实际字符串相等，想要字符串不等，不可能的，扔掉;排除掉一种分支数为1的情况
                            break 
                        if (realXORstr_i==realXORstr_j) and (PatternStrO[o][i]==PatternStrO[o][j]):
                            merged = merged | {i,j}
                            continue #实际字符串相等，想要字符串相等，不用管
                        if (realXORstr_i!=realXORstr_j) and (PatternStrO[o][i]!=PatternStrO[o][j]):
                            continue #实际字符串不等，想要字符串不等，不用管
                        if (realXORstr_i!=realXORstr_j) and (PatternStrO[o][i]==PatternStrO[o][j]):
                            #实际字符串不等，想要字符串相等
                            if (realXORstr_i[0]==realXORstr_j[0]) or (realXORstr_i[1]==realXORstr_j[1]):
                                feasible = False
                                break #只有一个分支值不同是不能得到相同输出值的；排除掉两种分支数为1的情况
                            else:
                                #两个分支上的值都不等
                                assert (realXORstr_i[0]!=realXORstr_j[0]) and (realXORstr_i[1]!=realXORstr_j[1])
                                
                                related={i,j}
                                currentZeroSum = set(realXORstr_i + realXORstr_j) #这四个值相加为0
                                
                                if not (currentZeroSum in zeroSum):
                                    zeroSum.append(currentZeroSum)
                                    
                                    if related.issubset(merged):
                                        continue
                                    else:
                                        
                                        numActive =  numActive + 1
                                merged = merged | related
                                    
                    assert numActive <= 3
                    
                    if feasible:
                        currentFeasiblePattern = leftPatternCode + rightPatternCode +outPatternCode + \
                                                 [int(bin(numActive)[2:].zfill(2)[0])] + [int(bin(numActive)[2:].zfill(2)[1])]
                        feasible_patterns.add(tuple(currentFeasiblePattern))

        return feasible_patterns

    def exchanging_SixbitCodes_l_r_o_fAH_fAL_patterns():
        '''
        exchanging mixture quadruples (aaaa,abab,abba) and (abcd) are encoded by 6bits:
        aaaa 000000
        abab 101101
        abba 110011
        abcd 111111
        Give out feasible patterns for XOR operation related variables:[left,right,output,fAH,fAL]
        Totally 4*4*4=64 cases
        '''
        feasible_patterns = []
        PatternCode = [[0,0,0,0,0,0],[1,0,1,1,0,1],[1,1,0,0,1,1],[1,1,1,1,1,1]]
        PatternStrL = [['a_l', 'a_l', 'a_l', 'a_l'],\
                       ['a_l', 'b_l', 'a_l', 'b_l'],\
                       ['a_l', 'b_l', 'b_l', 'a_l'],\
                       ['a_l', 'b_l', 'c_l', 'd_l']]
    
        PatternStrR =[['a_r','a_r','a_r','a_r'],\
                      ['a_r','b_r','a_r','b_r'],\
                      ['a_r','b_r','b_r','a_r'],\
                      ['a_r','b_r','c_r','d_r']]
        PatternStrO =[['a_o','a_o','a_o','a_o'],\
                      ['a_o','b_o','a_o','b_o'],\
                      ['a_o','b_o','b_o','a_o'],\
                      ['a_o','b_o','c_o','d_o']]
        for l in range(len(PatternCode)):
            leftPatternCode=PatternCode[l]
            for r in range(len(PatternCode)):                
                rightPatternCode=PatternCode[r]
                for o in range(len(PatternCode)):
                    outPatternCode=PatternCode[o]
                    numActive=0
                    zeroSum=list()
                    merged=set()
                    
                    feasible= True
                    for i,j in [[0,1],[0,2],[0,3],[1,2],[1,3],[2,3]]:
                        realXORstr_i = [PatternStrL[l][i], PatternStrR[r][i]]
                        realXORstr_j = [PatternStrL[l][j], PatternStrR[r][j]]
                        if (realXORstr_i==realXORstr_j) and (PatternStrO[o][i]!=PatternStrO[o][j]):
                            feasible = False #实际字符串相等，想要字符串不等，不可能的，扔掉;排除掉一种分支数为1的情况
                            break 
                        if (realXORstr_i==realXORstr_j) and (PatternStrO[o][i]==PatternStrO[o][j]):
                            merged = merged | {i,j}
                            continue #实际字符串相等，想要字符串相等，不用管
                        if (realXORstr_i!=realXORstr_j) and (PatternStrO[o][i]!=PatternStrO[o][j]):
                            continue #实际字符串不等，想要字符串不等，不用管
                        if (realXORstr_i!=realXORstr_j) and (PatternStrO[o][i]==PatternStrO[o][j]):
                            #实际字符串不等，想要字符串相等
                            if (realXORstr_i[0]==realXORstr_j[0]) or (realXORstr_i[1]==realXORstr_j[1]):
                                feasible = False
                                break #只有一个分支值不同是不能得到相同输出值的；排除掉两种分支数为1的情况
                            else:
                                #两个分支上的值都不等
                                assert (realXORstr_i[0]!=realXORstr_j[0]) and (realXORstr_i[1]!=realXORstr_j[1])
                                
                                related={i,j}
                                currentZeroSum = set(realXORstr_i + realXORstr_j) #这四个值相加为0
                                
                                if not (currentZeroSum in zeroSum):
                                    zeroSum.append(currentZeroSum)
                                    
                                    if related.issubset(merged):
                                        continue
                                    else:
                                        
                                        numActive =  numActive + 1
                                merged = merged | related
                                    
                    assert numActive <= 3
                    
                    if feasible:
                        currentFeasiblePattern = leftPatternCode + rightPatternCode +outPatternCode + \
                                                 [int(bin(numActive)[2:].zfill(2)[0])] + [int(bin(numActive)[2:].zfill(2)[1])]
                        feasible_patterns.append(tuple(currentFeasiblePattern))

        return feasible_patterns

    def all_SixbitCodes_l_r_o_fAH_fAL_patterns():
        '''
        all quadruples (aaaa,abab,abba) and (abcd) are encoded by 6bits:
        aaaa 000000
        abab 101101
        abba 110011
        abcd 111111
        aacc 011110
        abbb 111000
        abaa 100110
        abcc 111110
        abcb 111101
        abbc 111011
        aaba 010101
        abca 110111
        abac 101111
        aabc 011111
        aaab 001011
        Give out feasible patterns for XOR operation related variables:[left,right,output,fAH,fAL]
        Totally 15*15*15=3375 cases
        '''
        feasible_patterns = []
        PatternCode = [[0,0,0,0,0,0],[1,0,1,1,0,1],[1,1,0,0,1,1],[1,1,1,1,1,1],\
                       [0, 1, 1, 1, 1, 0], [1, 1, 1, 0, 0, 0], [1, 0, 0, 1, 1, 0], [1, 1, 1, 1, 1, 0], \
                       [1, 1, 1, 1, 0, 1], [1, 1, 1, 0, 1, 1], [0, 1, 0, 1, 0, 1], [1, 1, 0, 1, 1, 1], \
                       [1, 0, 1, 1, 1, 1], [0, 1, 1, 1, 1, 1], [0, 0, 1, 0, 1, 1]] 
        PatternStrL = [['a_l', 'a_l', 'a_l', 'a_l'],\
                       ['a_l', 'b_l', 'a_l', 'b_l'],\
                       ['a_l', 'b_l', 'b_l', 'a_l'],\
                       ['a_l', 'b_l', 'c_l', 'd_l'],\
                       ['a_l','a_l','c_l','c_l'],\
                       ['a_l','b_l','b_l','b_l'],\
                       ['a_l','b_l','a_l','a_l'],\
                       ['a_l','b_l','c_l','c_l'],\
                       ['a_l','b_l','c_l','b_l'],\
                       ['a_l','b_l','b_l','c_l'],\
                       ['a_l','a_l','b_l','a_l'],\
                       ['a_l','b_l','c_l','a_l'],\
                       ['a_l','b_l','a_l','c_l'],\
                       ['a_l','a_l','b_l','c_l'],\
                       ['a_l','a_l','a_l','b_l']]
    
        PatternStrR =[['a_r','a_r','a_r','a_r'],\
                      ['a_r','b_r','a_r','b_r'],\
                      ['a_r','b_r','b_r','a_r'],\
                      ['a_r','b_r','c_r','d_r'],\
                      ['a_r','a_r','c_r','c_r'],\
                       ['a_r','b_r','b_r','b_r'],\
                       ['a_r','b_r','a_r','a_r'],\
                       ['a_r','b_r','c_r','c_r'],\
                       ['a_r','b_r','c_r','b_r'],\
                       ['a_r','b_r','b_r','c_r'],\
                       ['a_r','a_r','b_r','a_r'],\
                       ['a_r','b_r','c_r','a_r'],\
                       ['a_r','b_r','a_r','c_r'],\
                       ['a_r','a_r','b_r','c_r'],\
                       ['a_r','a_r','a_r','b_r']]
        PatternStrO =[['a_o','a_o','a_o','a_o'],\
                      ['a_o','b_o','a_o','b_o'],\
                      ['a_o','b_o','b_o','a_o'],\
                      ['a_o','b_o','c_o','d_o'],\
                      ['a_o','a_o','c_o','c_o'],\
                       ['a_o','b_o','b_o','b_o'],\
                       ['a_o','b_o','a_o','a_o'],\
                       ['a_o','b_o','c_o','c_o'],\
                       ['a_o','b_o','c_o','b_o'],\
                       ['a_o','b_o','b_o','c_o'],\
                       ['a_o','a_o','b_o','a_o'],\
                       ['a_o','b_o','c_o','a_o'],\
                       ['a_o','b_o','a_o','c_o'],\
                       ['a_o','a_o','b_o','c_o'],\
                       ['a_o','a_o','a_o','b_o']]
        for l in range(len(PatternCode)):
            leftPatternCode=PatternCode[l]
            for r in range(len(PatternCode)):                
                rightPatternCode=PatternCode[r]
                for o in range(len(PatternCode)):
                    outPatternCode=PatternCode[o]
                    numActive=0
                    zeroSum=list()
                    merged=set()
                    
                    feasible= True
                    for i,j in [[0,1],[0,2],[0,3],[1,2],[1,3],[2,3]]:
                        realXORstr_i = [PatternStrL[l][i], PatternStrR[r][i]]
                        realXORstr_j = [PatternStrL[l][j], PatternStrR[r][j]]
                        if (realXORstr_i==realXORstr_j) and (PatternStrO[o][i]!=PatternStrO[o][j]):
                            feasible = False #实际字符串相等，想要字符串不等，不可能的，扔掉;排除掉一种分支数为1的情况
                            break 
                        if (realXORstr_i==realXORstr_j) and (PatternStrO[o][i]==PatternStrO[o][j]):
                            merged = merged | {i,j}
                            continue #实际字符串相等，想要字符串相等，不用管
                        if (realXORstr_i!=realXORstr_j) and (PatternStrO[o][i]!=PatternStrO[o][j]):
                            continue #实际字符串不等，想要字符串不等，不用管
                        if (realXORstr_i!=realXORstr_j) and (PatternStrO[o][i]==PatternStrO[o][j]):
                            #实际字符串不等，想要字符串相等
                            if (realXORstr_i[0]==realXORstr_j[0]) or (realXORstr_i[1]==realXORstr_j[1]):
                                feasible = False
                                break #只有一个分支值不同是不能得到相同输出值的；排除掉两种分支数为1的情况
                            else:
                                #两个分支上的值都不等
                                assert (realXORstr_i[0]!=realXORstr_j[0]) and (realXORstr_i[1]!=realXORstr_j[1])
                                
                                related={i,j}
                                currentZeroSum = set(realXORstr_i + realXORstr_j) #这四个值相加为0
                                
                                if not (currentZeroSum in zeroSum):
                                    zeroSum.append(currentZeroSum)
                                    
                                    if related.issubset(merged):
                                        continue
                                    else:
                                        
                                        numActive =  numActive + 1
                                merged = merged | related
                                    
                    assert numActive <= 3
                    
                    if feasible:
                        currentFeasiblePattern = leftPatternCode + rightPatternCode +outPatternCode + \
                                                 [int(bin(numActive)[2:].zfill(2)[0])] + [int(bin(numActive)[2:].zfill(2)[1])]
                        feasible_patterns.append(currentFeasiblePattern)

        return feasible_patterns
    

    def all_FourbitCodes_l_r_o_fAH_fAL_patterns():
        '''
        all quadruples are encoded by 4bits:
        aaaa 0000
        abab 0001
        abba 0010
        abcd 0011
        aacc 0100
        abbb 0101
        abaa 0110
        abcc 0111
        abcb 1000
        abbc 1001
        aaba 1010
        abca 1011
        abac 1100
        aabc 1101
        aaab 1110
        Give out feasible patterns for XOR operation related variables:[left,right,output,fAH,fAL]
        Totally 15*15*15=3375 cases
        '''
        feasible_patterns = []
        PatternCode = [[0,0,0,0],[0,0,0,1],[0,0,1,0],[0,0,1,1],[0,1,0,0],[0,1,0,1],[0,1,1,0],[0,1,1,1],\
                       [1,0,0,0],[1,0,0,1],[1,0,1,0],[1,0,1,1],[1,1,0,0],[1,1,0,1],[1,1,1,0]] 
        PatternStrL = [['a_l', 'a_l', 'a_l', 'a_l'],\
                       ['a_l', 'b_l', 'a_l', 'b_l'],\
                       ['a_l', 'b_l', 'b_l', 'a_l'],\
                       ['a_l', 'b_l', 'c_l', 'd_l'],\
                       ['a_l','a_l','c_l','c_l'],\
                       ['a_l','b_l','b_l','b_l'],\
                       ['a_l','b_l','a_l','a_l'],\
                       ['a_l','b_l','c_l','c_l'],\
                       ['a_l','b_l','c_l','b_l'],\
                       ['a_l','b_l','b_l','c_l'],\
                       ['a_l','a_l','b_l','a_l'],\
                       ['a_l','b_l','c_l','a_l'],\
                       ['a_l','b_l','a_l','c_l'],\
                       ['a_l','a_l','b_l','c_l'],\
                       ['a_l','a_l','a_l','b_l']]
    
        PatternStrR =[['a_r','a_r','a_r','a_r'],\
                      ['a_r','b_r','a_r','b_r'],\
                      ['a_r','b_r','b_r','a_r'],\
                      ['a_r','b_r','c_r','d_r'],\
                      ['a_r','a_r','c_r','c_r'],\
                       ['a_r','b_r','b_r','b_r'],\
                       ['a_r','b_r','a_r','a_r'],\
                       ['a_r','b_r','c_r','c_r'],\
                       ['a_r','b_r','c_r','b_r'],\
                       ['a_r','b_r','b_r','c_r'],\
                       ['a_r','a_r','b_r','a_r'],\
                       ['a_r','b_r','c_r','a_r'],\
                       ['a_r','b_r','a_r','c_r'],\
                       ['a_r','a_r','b_r','c_r'],\
                       ['a_r','a_r','a_r','b_r']]
        PatternStrO =[['a_o','a_o','a_o','a_o'],\
                      ['a_o','b_o','a_o','b_o'],\
                      ['a_o','b_o','b_o','a_o'],\
                      ['a_o','b_o','c_o','d_o'],\
                      ['a_o','a_o','c_o','c_o'],\
                       ['a_o','b_o','b_o','b_o'],\
                       ['a_o','b_o','a_o','a_o'],\
                       ['a_o','b_o','c_o','c_o'],\
                       ['a_o','b_o','c_o','b_o'],\
                       ['a_o','b_o','b_o','c_o'],\
                       ['a_o','a_o','b_o','a_o'],\
                       ['a_o','b_o','c_o','a_o'],\
                       ['a_o','b_o','a_o','c_o'],\
                       ['a_o','a_o','b_o','c_o'],\
                       ['a_o','a_o','a_o','b_o']]
        for l in range(len(PatternCode)):
            leftPatternCode=PatternCode[l]
            for r in range(len(PatternCode)):                
                rightPatternCode=PatternCode[r]
                for o in range(len(PatternCode)):
                    outPatternCode=PatternCode[o]
                    numActive=0
                    zeroSum=list()
                    merged=set()
                    
                    feasible= True
                    for i,j in [[0,1],[0,2],[0,3],[1,2],[1,3],[2,3]]:
                        realXORstr_i = [PatternStrL[l][i], PatternStrR[r][i]]
                        realXORstr_j = [PatternStrL[l][j], PatternStrR[r][j]]
                        if (realXORstr_i==realXORstr_j) and (PatternStrO[o][i]!=PatternStrO[o][j]):
                            feasible = False #实际字符串相等，想要字符串不等，不可能的，扔掉;排除掉一种分支数为1的情况
                            break 
                        if (realXORstr_i==realXORstr_j) and (PatternStrO[o][i]==PatternStrO[o][j]):
                            merged = merged | {i,j}
                            continue #实际字符串相等，想要字符串相等，不用管
                        if (realXORstr_i!=realXORstr_j) and (PatternStrO[o][i]!=PatternStrO[o][j]):
                            continue #实际字符串不等，想要字符串不等，不用管
                        if (realXORstr_i!=realXORstr_j) and (PatternStrO[o][i]==PatternStrO[o][j]):
                            #实际字符串不等，想要字符串相等
                            if (realXORstr_i[0]==realXORstr_j[0]) or (realXORstr_i[1]==realXORstr_j[1]):
                                feasible = False
                                break #只有一个分支值不同是不能得到相同输出值的；排除掉两种分支数为1的情况
                            else:
                                #两个分支上的值都不等
                                assert (realXORstr_i[0]!=realXORstr_j[0]) and (realXORstr_i[1]!=realXORstr_j[1])
                                
                                related={i,j}
                                currentZeroSum = set(realXORstr_i + realXORstr_j) #这四个值相加为0
                                
                                if not (currentZeroSum in zeroSum):
                                    zeroSum.append(currentZeroSum)
                                    
                                    if related.issubset(merged):
                                        continue
                                    else:
                                        
                                        numActive =  numActive + 1
                                merged = merged | related
                                    
                    assert numActive <= 3
                    
                    if feasible:
                        currentFeasiblePattern = leftPatternCode + rightPatternCode +outPatternCode + \
                                                 [int(bin(numActive)[2:].zfill(2)[0])] + [int(bin(numActive)[2:].zfill(2)[1])]
                        feasible_patterns.append(currentFeasiblePattern)

        return feasible_patterns
    
    def fAHAL_minus1(fAHAL):
        assert fAHAL == tuple([1,0]) or fAHAL ==tuple([1,1])
        if fAHAL == tuple([1,0]):
            return tuple([0,1])
        if fAHAL == tuple([1,1]):
            return tuple([1,0])
    def A_fAHfAL_Delta_HL():
        feasible_patterns = []
        points = BasicTools.deA_A_finalAHAL_patterns()
        for p in points:
            deA = p[0:6]
            A = p[6:12]
            fAHAL = p[12:14]
            if (BasicTools.count1s(deA)==5) or (BasicTools.count1s(deA)==6): #case D and E
                if (A[0]==1) and (A[-1]==1):
                    
                    feasible_patterns.append(tuple(A+fAHAL+tuple([0])+BasicTools.fAHAL_minus1(fAHAL)))
                    feasible_patterns.append(tuple(A+fAHAL+tuple([1])+fAHAL))
                else:
                    feasible_patterns.append(tuple(A+fAHAL+tuple([0])+fAHAL))
                    feasible_patterns.append(tuple(A+fAHAL+tuple([1])+fAHAL))
            else:
                feasible_patterns.append(tuple(A+fAHAL+tuple([0])+fAHAL))
                feasible_patterns.append(tuple(A+fAHAL+tuple([1])+fAHAL))

        feasible_patterns = set(feasible_patterns)
        return feasible_patterns

    def deA_Ind_output_e_Ind_fAHAL():
        feasible_patterns=set()
        deA=BasicTools.e_ij_feasible_patterns()
        
        for p in deA:
            assert BasicTools.count1s(p) in set([0,3,4,5,6])
            if BasicTools.count1s(p)==0:
                feasible_patterns.add(p+tuple([0,0,0,0,0,0,0,0]))
            elif BasicTools.count1s(p)==3:
                feasible_patterns.add(p+tuple([0,0,0,0,0,0,0,1]))
                output_e_Ind = []
                for i in range(6):
                    if p[i] == 0:
                        output_e_Ind.append(0)
                    else:
                        output_e_Ind.append(1)
                assert BasicTools.count1s(output_e_Ind)==3
                feasible_patterns.add(p+tuple(output_e_Ind + [0,0]))
            elif BasicTools.count1s(p)==4:
                feasible_patterns.add(p+tuple([0,0,0,0,0,0,0,1]))
                output_e_Ind = []
                for i in range(6):
                    if p[i] == 0:
                        output_e_Ind.append(0)
                    else:
                        output_e_Ind.append(1)
                assert BasicTools.count1s(output_e_Ind)==4
                feasible_patterns.add(p+tuple(output_e_Ind + [0,0]))
            elif BasicTools.count1s(p)==5:

                for i in range(6):
                    if p[i]==0:
                        count = 0
                        output_e_Ind = BasicTools.e_ij_feasible_patterns()
                        for item in output_e_Ind:
                            if item[i]==0:
                                hw = BasicTools.count1s(item)
                                if hw==0:
                                    feasible_patterns.add(p+item +tuple([1,0]))
                                    count = count +1
                                elif hw==5:
                                    feasible_patterns.add(p+item +tuple([0,0]))
                                    count = count +1
                                elif ((hw==4) or (hw ==3)):
                                    feasible_patterns.add(p+item +tuple([0,1]))
                                    count = count +1
                        assert count ==5                
                        break
                
            elif BasicTools.count1s(p)==6:
                output_e_Ind = BasicTools.e_ij_feasible_patterns()
                for item in output_e_Ind:
                    hw=BasicTools.count1s(item)
                    assert hw in set([0,3,4,5,6])
                    if hw == 6:
                        feasible_patterns.add(p+item +tuple([0,0]))
                    elif hw == 5:
                        feasible_patterns.add(p+item +tuple([0,1]))
                    elif hw == 4:
                        feasible_patterns.add(p+item +tuple([1,0]))
                    elif hw == 3:
                        feasible_patterns.add(p+item +tuple([1,0]))
                    elif hw == 0:
                        feasible_patterns.add(p+item +tuple([1,1]))
                    
        feasible_patterns_list=[]
        for p in feasible_patterns:
            feasible_patterns_list.append(list(p))
        
        return feasible_patterns

    def lab_dop_eij():
        feasible_patterns_list=[]
        eij=BasicTools.e_ij_feasible_patterns()
        feasible_patterns_list.append(tuple([0,0,1,1,1,1,1,1]))
        feasible_patterns_list.append(tuple([0,0,0,0,0,0,0,0]))
        feasible_patterns_list.append(tuple([0,0,1,0,1,1,0,1]))
        feasible_patterns_list.append(tuple([0,0,1,1,0,0,1,1]))
        feasible_patterns_list.append(tuple([0,0,0,1,1,1,1,0]))
        for p in eij:
            feasible_patterns_list.append(tuple([0,1])+p)
            feasible_patterns_list.append(tuple([1,0])+p)
            feasible_patterns_list.append(tuple([1,1])+p)
            
        return feasible_patterns_list   
    
