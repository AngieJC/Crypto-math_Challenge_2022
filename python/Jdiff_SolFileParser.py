from BasicTools import *
from Jdiff import *
class Jdiff_SolFileParser:
    def __init__(self, solutionFile):
        solFile = open(solutionFile,'r')
        self.var_value_map = dict()
        print('in init()')
        for line in solFile:
            if line[0] != '#':
                temp = line
                temp = temp.replace('-', ' ')
                temp = temp.split()
                self.var_value_map[temp[0]] = int(temp[1])

    def getSolutions(self):
        return self.var_value_map

    def getBitPatternsFrom(self, vars_sequence):
        pattern = ''
        for key in vars_sequence:
            assert key in self.var_value_map.keys()
            pattern = pattern + str(self.var_value_map[key])

        return pattern
    
    def printValue(self,var):
        varValue = self.getBitPatternsFrom(var)
        print(varValue)

    def Jdiff_get_intermediates_to_Round(self,r):
        for i in range(1,r + 1):
            print(str(i)+'-round')
            Lin = Jdiff.genVars_Round(i-1)[0:16]
            Rin = Jdiff.genVars_Round(i-1)[16:32]
            aftRk1 = Jdiff.genVars_aftRk1_Round(i-1)
            Act_Rk1 = Jdiff.genVars_Act_Rk1_Round(i-1)
            Act_Rk2 = Jdiff.genVars_Act_Rk2_Round(i-1)
            Act_S = Jdiff.genVars_Act_S_Round(i-1)
            bfS = Jdiff.genVars_bfS_Round(i-1)
            inS = Jdiff.genVars_inS_Round(i-1)
            aftS = Jdiff.genVars_aftS_Round(i-1)
            rot3 =  BasicTools.leftCyclicRotation(aftS, 3)
            rot9 =  BasicTools.leftCyclicRotation(aftS,9)
            rot14 = BasicTools.leftCyclicRotation(aftS, 3)
            aftP = Jdiff.genVars_aftP_Round(i-1)
            aftRk2 = Jdiff.genVars_aftRk2_Round(i-1)
            Lout = Jdiff.genVars_Round(i)[0:16]
            Rout = Jdiff.genVars_Round(i)[16:32]
            
            print('leftIn      ',end=' ')
            self.printValue(Lin)
            print('rightIn     ',end=' ')
            self.printValue(Rin)
            

            '''
            print('aftRk1      ',end=' ')
            self.printValue(aftRk1)
            print('Act_Rk1     ',end=' ')
            self.printValue(Act_Rk1)
            print('rightIn     ',end=' ')
            self.printValue(Rin)
            print('bfS         ',end=' ' )
            self.printValue(bfS)
            print('bfS<<1      ',end=' ' )
            self.printValue(bfS[1:-1])
            print('bfS<<2      ',end=' ' )
            self.printValue(bfS[2:-1])
            print('inS         ',end=' ')
            self.printValue(inS)
            print('Act_S       ',end=' ')
            self.printValue(Act_S)
            print('bfS         ',end=' ' )
            self.printValue(bfS)
            print('aftS        ',end=' ')
            self.printValue(aftS)
            print('aftS<<<3    ',end=' ')
            self.printValue(rot3)
            print('aftS<<<9    ',end=' ')
            self.printValue(rot9)
            print('aftS<<<14   ',end=' ')
            self.printValue(rot14)
            print('aftP        ',end=' ')
            self.printValue(aftP)
            print('leftIn      ',end=' ')
            self.printValue(Lin)
            print('rightOut    ',end=' ')
            self.printValue(Rout)
            print('aftP        ',end=' ')
            self.printValue(aftP)
            print('aftRk2      ',end=' ')
            self.printValue(aftRk2)
            print('Act_Rk2     ',end=' ')
            self.printValue(Act_Rk2)
            print('rightIn     ',end=' ')
            self.printValue(Rin)
            print('leftOut     ',end=' ')
            self.printValue(Lout)
            '''
def main():
    r = 13
    sol = Jdiff_SolFileParser("Jdiff_" + str(r) + "r.sol")
    sol.Jdiff_get_intermediates_to_Round(r)

if __name__=='__main__':
    main()